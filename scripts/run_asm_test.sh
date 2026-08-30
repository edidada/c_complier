#!/bin/bash
# 汇编生成验收（T5）：编译 → 汇编 → 链接 32 位 → 运行 → 精确比对输出
# 用法: run_asm_test.sh <compiler> <test_dir>
# 约定：T5-*.c 合法程序（rc=0）；T5-*.expect 为期望标准输出（精确比对，忽略 \r）
set -u
COMPILER="$1"
TESTDIR="$2"
cd "$(dirname "$0")/.."

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
pass=0
fail=0

for f in "$TESTDIR"/T5-*.c; do
    name=$(basename "$f" .c)
    "$COMPILER" "$f" >/dev/null 2>&1
    rc=$?
    if [ "$rc" -ne 0 ]; then
        echo "FAIL $name: 编译退出码 $rc"
        fail=$((fail + 1))
        continue
    fi
    if ! nasm -f elf32 -d ELF_TYPE -i . asm/test.asm -o "$tmp/$name.o" 2>"$tmp/nasm.err"; then
        echo "FAIL $name: nasm 汇编失败"
        cat "$tmp/nasm.err"
        fail=$((fail + 1))
        continue
    fi
    if ! nasm -f elf32 -d ELF_TYPE asm/asm_io.asm -o "$tmp/io.o" 2>"$tmp/nasm_io.err"; then
        echo "FAIL $name: asm_io 汇编失败"
        cat "$tmp/nasm_io.err"
        fail=$((fail + 1))
        continue
    fi
    if ! gcc -m32 -no-pie "$tmp/$name.o" "$tmp/io.o" -o "$tmp/$name.bin" 2>"$tmp/ld.err"; then
        echo "FAIL $name: 链接失败"
        cat "$tmp/ld.err"
        fail=$((fail + 1))
        continue
    fi
    output=$("$tmp/$name.bin" 2>&1)
    expected=$(cat "$TESTDIR/$name.expect")
    # 规范化：忽略 \r（不同平台 printf 输出差异）
    output_n=$(printf "%s" "$output" | tr -d '\r')
    expected_n=$(printf "%s" "$expected" | tr -d '\r')
    if [ "$output_n" == "$expected_n" ]; then
        echo "PASS $name"
        pass=$((pass + 1))
    else
        echo "FAIL $name: 期望 [$expected_n] 实际 [$output_n]"
        fail=$((fail + 1))
    fi
done

echo "=== 汇编生成测试: $pass 通过, $fail 失败 ==="
[ "$fail" -eq 0 ]
