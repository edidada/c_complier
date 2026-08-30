#!/bin/bash
# 语义测试验收（T3）：正常编译模式（类型检查生效）
# 用法: run_semantic_test.sh <compiler> <test_dir>
# 用例约定：
#   CN.c            合法程序：exit 0 且 stderr 无"语义错误"
#   CN.c + CN.err   非法程序：exit 1 且 stderr 含每行子串
#                   行首 "!" 表示该子串必须不出现（用于 C11 分层验证）
set -u
COMPILER="$1"
TESTDIR="$2"
pass=0
fail=0

for f in "$TESTDIR"/C*.c; do
    name=$(basename "$f" .c)
    errfile="/tmp/sem_err.$$"
    "$COMPILER" "$f" > /dev/null 2>"$errfile"
    rc=$?

    if [ -f "$TESTDIR/$name.err" ]; then
        # ---- 非法程序 ----
        ok=1
        while IFS= read -r line; do
            [ -z "$line" ] && continue
            if [ "${line:0:1}" = "!" ]; then
                sub="${line:1}"
                if grep -qF "$sub" "$errfile"; then
                    echo "FAIL $name: 不应出现 [$sub]"
                    ok=0
                fi
            else
                if ! grep -qF "$line" "$errfile"; then
                    echo "FAIL $name: 缺少子串 [$line]"
                    ok=0
                fi
            fi
        done < "$TESTDIR/$name.err"
        if [ "$ok" -eq 0 ]; then
            cat "$errfile"
            fail=$((fail + 1))
            continue
        fi
        if [ "$rc" -ne 1 ]; then
            echo "FAIL $name: 期望退出码 1, 实际 $rc"
            cat "$errfile"
            fail=$((fail + 1))
            continue
        fi
        echo "PASS $name (非法程序正确报错)"
    else
        # ---- 合法程序 ----
        if [ "$rc" -ne 0 ]; then
            echo "FAIL $name: 合法程序退出码 $rc"
            cat "$errfile"
            fail=$((fail + 1))
            continue
        fi
        if grep -q "语义错误" "$errfile"; then
            echo "FAIL $name: 合法程序误报语义错误"
            cat "$errfile"
            fail=$((fail + 1))
            continue
        fi
        echo "PASS $name (合法)"
    fi
    pass=$((pass + 1))
    rm -f "$errfile"
done

echo "=== 语义测试: $pass 通过, $fail 失败 ==="
[ "$fail" -eq 0 ]
