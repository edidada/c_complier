#!/bin/bash
# 中间代码验收（T4/T6）：正常编译模式，检查四元式序列关键指令
# 用法: run_intercode_test.sh <compiler> <test_dir> [pattern]
# 约定：T4-*.c / T6-*.c 合法程序（rc=0）；
#       expect 每行一个断言：普通行必含子串（grep -F）；"!" 前缀行必不含（阶段6优化验证）
set -u
COMPILER="$1"
TESTDIR="$2"
PATTERN="${3:-T[46]-*.c}"
pass=0
fail=0

for f in "$TESTDIR"/$PATTERN; do
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
        if [ "${sub:0:1}" = "!" ]; then
            neg="${sub:1}"
            if grep -qF "$neg" "$outfile"; then
                echo "FAIL $name: 四元式不应出现 [$neg]"
                ok=0
            fi
        elif ! grep -qF "$sub" "$outfile"; then
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
