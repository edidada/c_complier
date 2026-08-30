#!/bin/bash
# 语法测试验收（T2）：--parse-only 模式
# 用法: run_parse_test.sh <compiler> <test_dir>
# 用例约定：
#   sN.c            合法程序：exit 0 且 stderr 无"语法错误"
#   sN.c + sN.err   非法程序：exit 1 且 stderr 含子串（"[N]前缀" 表示至少出现 N 次）
set -u
COMPILER="$1"
TESTDIR="$2"
pass=0
fail=0

for f in "$TESTDIR"/s*.c; do
    name=$(basename "$f" .c)
    errfile="/tmp/parse_err.$$"
    "$COMPILER" --parse-only "$f" > /dev/null 2>"$errfile"
    rc=$?

    if [ -f "$TESTDIR/$name.err" ]; then
        # ---- 非法程序 ----
        if [ "$rc" -ne 1 ]; then
            echo "FAIL $name: 期望退出码 1, 实际 $rc"
            cat "$errfile"
            fail=$((fail + 1))
            continue
        fi
        sub=$(cat "$TESTDIR/$name.err")
        count=1
        if [[ "$sub" =~ ^\[([0-9]+)\](.*) ]]; then
            count=${BASH_REMATCH[1]}
            sub=${BASH_REMATCH[2]}
        fi
        n=$(grep -cF "$sub" "$errfile")
        if [ "$n" -ge "$count" ]; then
            echo "PASS $name (错误x$n)"
        else
            echo "FAIL $name: 子串[$sub] 出现 $n 次 < 期望 $count 次"
            cat "$errfile"
            fail=$((fail + 1))
            continue
        fi
    else
        # ---- 合法程序 ----
        if [ "$rc" -ne 0 ]; then
            echo "FAIL $name: 合法程序退出码 $rc"
            cat "$errfile"
            fail=$((fail + 1))
            continue
        fi
        if grep -q "语法错误" "$errfile"; then
            echo "FAIL $name: 合法程序误报语法错误"
            cat "$errfile"
            fail=$((fail + 1))
            continue
        fi
        echo "PASS $name (合法)"
    fi
    pass=$((pass + 1))
    rm -f "$errfile"
done

echo "=== 语法测试: $pass 通过, $fail 失败 ==="
[ "$fail" -eq 0 ]
