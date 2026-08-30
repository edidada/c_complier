#!/bin/bash
# 错误分析验收（T7）：统一错误统计 / 分级 / 友好提示
# 用法: run_error_test.sh <compiler> <test_dir>
# 用例约定：T7-*.c 输入；T7-*.expect 断言文件：
#   MODE=lex|parse|compile  运行模式（默认 compile）
#   RC=0|1                  期望退出码（默认 1）
#   其余行：stderr 必含子串；"!" 前缀行必不含（如 !语义错误）
set -u
COMPILER="$1"
TESTDIR="$2"
pass=0
fail=0

for f in "$TESTDIR"/T7-*.c; do
    name=$(basename "$f" .c)
    errfile="/tmp/err_test.$$"
    mode=compile
    rc_exp=1
    if [ -f "$TESTDIR/$name.expect" ]; then
        mode=$(grep -E '^MODE=' "$TESTDIR/$name.expect" | head -1 | cut -d= -f2)
        rc_exp=$(grep -E '^RC=' "$TESTDIR/$name.expect" | head -1 | cut -d= -f2)
        [ -z "$mode" ] && mode=compile
        [ -z "$rc_exp" ] && rc_exp=1
    fi
    case "$mode" in
        lex)   "$COMPILER" --lex "$f" >/dev/null 2>"$errfile";;
        parse) "$COMPILER" --parse-only "$f" >/dev/null 2>"$errfile";;
        *)     "$COMPILER" "$f" >/dev/null 2>"$errfile";;
    esac
    rc=$?
    ok=1
    if [ "$rc" -ne "$rc_exp" ]; then
        echo "FAIL $name: 期望退出码 $rc_exp, 实际 $rc"
        ok=0
    fi
    while IFS= read -r line; do
        [ -z "$line" ] && continue
        case "$line" in
            MODE=*|RC=*) continue;;
        esac
        if [ "${line:0:1}" = "!" ]; then
            neg="${line:1}"
            if grep -qF "$neg" "$errfile"; then
                echo "FAIL $name: 不应出现 [$neg]"
                ok=0
            fi
        elif ! grep -qF "$line" "$errfile"; then
            echo "FAIL $name: 缺少子串 [$line]"
            ok=0
        fi
    done < "$TESTDIR/$name.expect"
    if [ "$ok" -eq 1 ]; then
        echo "PASS $name"
        pass=$((pass + 1))
    else
        echo "--- stderr ---"
        cat "$errfile"
        fail=$((fail + 1))
    fi
    rm -f "$errfile"
done

echo "=== 错误分析测试: $pass 通过, $fail 失败 ==="
[ "$fail" -eq 0 ]
