#!/bin/bash
# T0 端到端验收脚本：编译 → 汇编 → 链接 → 运行 → 比对输出
# 用法: run_test.sh <compiler> <test.c> <expected_output>
set -e

COMPILER="$1"
TESTC="$2"
EXPECTED="$3"
EXPECTED_OUT=$(printf "%b" "$EXPECTED")

# 切到项目根（脚本位于 scripts/ 下）
cd "$(dirname "$0")/.."

"$COMPILER" "$TESTC" >/dev/null 2>&1
nasm -f elf32 -d ELF_TYPE -i . asm/test.asm -o build/_e2e_test.o
nasm -f elf32 -d ELF_TYPE asm/asm_io.asm -o build/_e2e_io.o
gcc -m32 -no-pie build/_e2e_test.o build/_e2e_io.o -o build/_e2e_run 2>/dev/null
OUTPUT=$(./build/_e2e_run)

if [ "$OUTPUT" == "$EXPECTED_OUT" ]; then
    echo "PASS: $(basename "$TESTC") -> '$OUTPUT'"
else
    echo "FAIL: $(basename "$TESTC") expected '$EXPECTED_OUT', got '$OUTPUT'"
    exit 1
fi
