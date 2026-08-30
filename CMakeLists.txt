cmake_minimum_required(VERSION 3.14)
#project(c_complier LANGUAGES C CXX ASM_NASM)
project(c_complier LANGUAGES C CXX)

# 只有找到 nasm 才启用 ASM_NASM
find_program(NASM_EXECUTABLE nasm)
if(NASM_EXECUTABLE)
    enable_language(ASM_NASM)
    message(STATUS "Found NASM: ${NASM_EXECUTABLE}")
else()
    message(WARNING "NASM not found. Assembly files will be skipped.")
endif()
# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用调试信息
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_DEBUG "-g")

# 查找工具
find_program(FLEX_EXECUTABLE flex REQUIRED)
find_program(BISON_EXECUTABLE bison REQUIRED)
find_program(NASM_EXECUTABLE nasm REQUIRED)
# 查找系统 sed
find_program(SED_EXECUTABLE sed REQUIRED)

# 仅在 macOS 上使用 gsed（如果存在）
if(APPLE)
    find_program(GSed_EXECUTABLE gsed)
    if(GSed_EXECUTABLE)
        set(SED_EXECUTABLE ${GSed_EXECUTABLE})
        message(STATUS "macOS detected: using gsed -> ${SED_EXECUTABLE}")
    else()
        message(WARNING "gsed not found on macOS. BSD sed may fail with -i. Run: brew install gnu-sed")
    endif()
endif()
# 源文件
set(SOURCES
        abstract_syntax_tree/AstNode.cpp
        symbol_table/symbol.cpp
        intermidiate_code/interCode.cpp
        asm/AsmGenerate.cpp
)

# 头文件目录
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/abstract_syntax_tree
        ${CMAKE_CURRENT_SOURCE_DIR}/symbol_table
        ${CMAKE_CURRENT_SOURCE_DIR}/intermidiate_code
        ${CMAKE_CURRENT_SOURCE_DIR}/asm
)

# ==================== Flex + Bison ====================

# Flex 目标
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/scanner.lex.c
        COMMAND ${FLEX_EXECUTABLE} -o ${CMAKE_CURRENT_BINARY_DIR}/scanner.lex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/scanner.l
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scanner.l
        COMMENT "Generating scanner.lex.c with flex"
)

# Bison 目标
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
        ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h
        COMMAND ${BISON_EXECUTABLE} -d ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
        -o ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
        COMMENT "Generating parser.tab.c/h with bison"
)
# Sed 修改 parser.tab.h
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h.modified
        COMMAND ${SED_EXECUTABLE} -i
        -e "1i#include \\\"./abstract_syntax_tree/AstNode.h\\\""
        -e "2i#include \\\"./symbol_table/symbol.h\\\""
        -e "3i#include \\\"./intermidiate_code/interCode.h\\\""
        -e "4i#include \\\"./asm/AsmGenerate.h\\\""
        ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h.modified
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
        COMMENT "Inserting includes into parser.tab.h"
)
# 强制依赖：确保 sed 在编译前执行
add_custom_target(modify_parser_header ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h.modified
)

# ==================== 主可执行文件 ====================

add_executable(scanner_parser_test
        ${SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
        ${CMAKE_CURRENT_BINARY_DIR}/scanner.lex.c
)
# ⭐️ 核心修改：强制 Bison/Flex 输出文件使用 C++ 编译 ⭐️
# 这解决了 C 文件编译时找不到 C++ 标准库 (iostream/string) 的问题
set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
        ${CMAKE_CURRENT_BINARY_DIR}/scanner.lex.c
        PROPERTIES LANGUAGE CXX
)
add_dependencies(scanner_parser_test modify_parser_header)

target_include_directories(scanner_parser_test PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}  # 包含 parser.tab.h
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ==================== 汇编 + 32位链接 ====================

# 编译 asm_io.asm → asm_io.o (NASM, ELF32)
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/asm_io.o
        COMMAND ${NASM_EXECUTABLE} -g -f elf32 -d ELF_TYPE
        ${CMAKE_CURRENT_SOURCE_DIR}/asm/asm_io.asm
        -o ${CMAKE_CURRENT_BINARY_DIR}/asm_io.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/asm/asm_io.asm
        COMMENT "Assembling asm_io.asm → asm_io.o"
)

# 编译 test.asm → test.o (NASM, ELF32)
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test.o
        COMMAND ${NASM_EXECUTABLE} -g -f elf32 -d ELF_TYPE -i ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/asm/test.asm
        -o ${CMAKE_CURRENT_BINARY_DIR}/test.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/asm/test.asm
        COMMENT "Assembling test.asm → test.o"
)

# 生成 test 可执行文件（32位）
add_custom_target(test_executable ALL
        COMMAND gcc -g -m32 -no-pie
        ${CMAKE_CURRENT_BINARY_DIR}/asm_io.o
        ${CMAKE_CURRENT_BINARY_DIR}/test.o
        -o ${CMAKE_CURRENT_BINARY_DIR}/test
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/asm_io.o
        ${CMAKE_CURRENT_BINARY_DIR}/test.o
        COMMENT "Linking 32-bit test executable"
)

# ==================== 运行测试（可选）================

add_custom_target(run_test
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/scanner_parser_test
        test_code/test1.c
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test
        DEPENDS scanner_parser_test test_executable
        COMMENT "Running scanner_parser_test and ./test"
)

# ==================== 清理（增强 clean）================

# CMake 自带 clean，但我们可以增强
add_custom_target(distclean
        COMMAND ${CMAKE_COMMAND} -E remove -f
        ${CMAKE_CURRENT_BINARY_DIR}/*.o
        ${CMAKE_CURRENT_BINARY_DIR}/*.lex.c
        ${CMAKE_CURRENT_BINARY_DIR}/*.tab.c
        ${CMAKE_CURRENT_BINARY_DIR}/*.tab.h
        ${CMAKE_CURRENT_BINARY_DIR}/scanner_parser_test
        ${CMAKE_CURRENT_BINARY_DIR}/test
        ${CMAKE_CURRENT_BINARY_DIR}/asm_io.o
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
        COMMENT "Deep clean: remove generated files"
)

add_executable(test_symbol_table
        symbol_table/test_symbol_table.cpp
        symbol_table/symbol.cpp
        abstract_syntax_tree/AstNode.cpp
)

add_executable(symbol_table_testcode
    symbol_table/symbol_table_testcode.cpp
    symbol_table/symbol.cpp
)

add_executable(test_func_symbol
        symbol_table/test_func_symbol.cpp
        abstract_syntax_tree/AstNode.cpp
        abstract_syntax_tree/DefFunASTNode.cpp
        symbol_table/symbol.cpp
        symbol_table/FuncSymbol.cpp
)

# ==================== CTest 端到端验收测试 (T0) ====================
enable_testing()
add_test(NAME e2e_test1 COMMAND bash
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_test.sh
        $<TARGET_FILE:scanner_parser_test>
        ${CMAKE_CURRENT_SOURCE_DIR}/test_code/test1.c
        "3\\n2")
add_test(NAME e2e_test2 COMMAND bash
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_test.sh
        $<TARGET_FILE:scanner_parser_test>
        ${CMAKE_CURRENT_SOURCE_DIR}/test_code/test2.c
        "3628800")
add_test(NAME e2e_test3 COMMAND bash
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_test.sh
        $<TARGET_FILE:scanner_parser_test>
        ${CMAKE_CURRENT_SOURCE_DIR}/test_code/test3.c
        "39916800\\n8")
add_test(NAME e2e_test4 COMMAND bash
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_test.sh
        $<TARGET_FILE:scanner_parser_test>
        ${CMAKE_CURRENT_SOURCE_DIR}/test_code/test4.c
        "100")
add_test(NAME e2e_test5 COMMAND bash
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_test.sh
        $<TARGET_FILE:scanner_parser_test>
        ${CMAKE_CURRENT_SOURCE_DIR}/test_code/test5.c
        "8")
add_test(NAME e2e_test6 COMMAND bash
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_test.sh
        $<TARGET_FILE:scanner_parser_test>
        ${CMAKE_CURRENT_SOURCE_DIR}/test_code/test6.c
        "1")

# ==================== CTest 词法测试 (T1) ====================
add_test(NAME lex_suite COMMAND bash
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_lex_test.sh
        $<TARGET_FILE:scanner_parser_test>
        ${CMAKE_CURRENT_SOURCE_DIR}/test_lex)

# ==================== CTest 语法测试 (T2) ====================
add_test(NAME parse_suite COMMAND bash
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_parse_test.sh
        $<TARGET_FILE:scanner_parser_test>
        ${CMAKE_CURRENT_SOURCE_DIR}/test_parse)