#!/bin/bash
# 词法测试验收（T1）：--lex 模式输出比对 + 词法错误检测
# 用法: run_lex_test.sh <compiler> <test_dir>
# 用例约定：lxN.c 输入；lxN.expect 期望 token 序列（"单词 词素" 两列）；
#           lxN.err 期望 stderr 中出现的子串。
set -u
COMPILER="$1"
TESTDIR="$2"
pass=0
fail=0

for f in "$TESTDIR"/l*.c; do
    name=$(basename "$f" .c)
    errfile="/tmp/lex_err.$$"
    out=$("$COMPILER" --lex "$f" 2>"$errfile")
    got=$(echo "$out" | awk '{$1=$1; print $1" "$2}' | grep -v '^$' | grep -v '^单词')

    # 比对 token 序列（忽略属性列）
    if [ -f "$TESTDIR/$name.expect" ]; then
        exp=$(cat "$TESTDIR/$name.expect")
        if [ "$got" == "$exp" ]; then
            echo "PASS $name (token)"
        else
            echo "FAIL $name: token 序列不匹配"
            echo "--- expect ---"; echo "$exp"
            echo "--- got ---"; echo "$got"
            fail=$((fail + 1))
            continue
        fi
    fi

    # 比对词法错误
    if [ -f "$TESTDIR/$name.err" ]; then
        exp_err=$(cat "$TESTDIR/$name.err")
        if grep -qF "$exp_err" "$errfile"; then
            echo "PASS $name (错误检测)"
        else
            echo "FAIL $name: 期望错误 [$exp_err] 未出现在 stderr"
            echo "--- stderr ---"; cat "$errfile"
            fail=$((fail + 1))
        fi
    fi
    pass=$((pass + 1))
    rm -f "$errfile"
done

echo "=== 词法测试: $pass 通过, $fail 失败 ==="
[ "$fail" -eq 0 ]
