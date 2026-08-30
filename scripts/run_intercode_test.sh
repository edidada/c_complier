#!/bin/bash
# 中间代码验收（T4）：正常编译模式，检查四元式序列关键指令
# 用法: run_intercode_test.sh <compiler> <test_dir>
# 约定：T4-*.c 合法程序（rc=0）；T4-*.expect 每行一个必含子串（grep -F）
set -u
COMPILER="$1"
TESTDIR="$2"
pass=0
fail=0

for f in "$TESTDIR"/T4-*.c; do
    name=$(basename "$f" .c)
    outfile="/tmp/intercode_out.$$"
    errfile="/tmp/intercode_err.$$"
    "$COMPILER" "$f" > "$outfile" 2>"$errfile"
    rc=$?
    if [ "$rc" -ne 0 ]; then
        echo "FAIL $name: 编译退出码 $rc"
        cat "$errfile"
        fail=$((fail + 1))
        continue
    fi
    ok=1
    while IFS= read -r sub; do
        [ -z "$sub" ] && continue
        if ! grep -qF "$sub" "$outfile"; then
            echo "FAIL $name: 四元式缺少 [$sub]"
            ok=0
        fi
    done < "$TESTDIR/$name.expect"
    if [ "$ok" -eq 1 ]; then
        echo "PASS $name"
    else
        echo "----- $name 实际四元式 -----"
        grep -E "^L[0-9]+:" "$outfile" | head -20
        fail=$((fail + 1))
        continue
    fi
    pass=$((pass + 1))
    rm -f "$outfile" "$errfile"
done

echo "=== 中间代码测试: $pass 通过, $fail 失败 ==="
[ "$fail" -eq 0 ]
