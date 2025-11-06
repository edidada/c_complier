# my

```shell
/mnt/d/develops/git/github/c/c_complier/cmake-build-debug/scanner_parser_test ../test_code/test1.c
[i]>>>Read data from file:../test_code/test1.c 
ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!ERROR!!|____ Type: ROOT Content: Program Depth: 0
    |____ Type: STATEMENT Content: Single_Block Depth: 1
        |____ Type: DEFINITION Content: Def_Func_Body_Block Depth: 2
            |____ Type: MODIFY Content: INT_TYPE Depth: 3
            |____ Type: DEFINITION Content: Func_NParam Depth: 3
            |   |____ Type: ID Content: main Depth: 4
            |____ Type: BODY Content: Body Depth: 3
                |____ Type: STATEMENT Content: Some_Stmt Depth: 4
                    |____ Type: STATEMENT Content: Some_Stmt Depth: 5
                    |   Qy|____ Type: STATEMENT Content: Some_Stmt Depth: 6
                    |   |   |____ Type: STATEMENT Content: Some_Stmt Depth: 7
                    |   |   |   |____ Type: STATEMENT Content: Some_Stmt Depth: 8
                    |   |   |   |   |____ Type: STATEMENT Content: Some_Stmt Depth: 9
                    |   |   |   |   |   �x|____ Type: STATEMENT Content: Some_Stmt Depth: 10
                    |   |   |   |   |   |   |____ Type: STATEMENT Content: Def_Stmt Depth: 11
                    |   |   |   |   |   |       |____ Type: DEFINITION Content: Def_Var Depth: 12
                    |   |   |   |   |   |           |____ Type: MODIFY Content: INT_TYPE Depth: 13
                    |   |   |   |   |   |           |____ Type: DEFINITION Content: Def_Single_Var Depth: 13
                    |   |   |   |   |   |               Qw|____ Type: DEFINITION Content: Var_ONLY Depth: 14
                    |   |   |   |   |   |                   |____ Type: DEFINITION Content: Block_Single_Vardef Depth: 15
                    |   |   |   |   |   |                       |____ Type: ID Content: a Depth: 16
                    |   |   |   |   |   �v|____ Type: STATEMENT Content: Exp_Stmt Depth: 10
                    |   |   |   |   |       |____ Type: OPERATION Content: Assign Depth: 11
                    |   |   |   |   |           |____ Type: EXPRESSION Content: ID_Exp Depth: 12
                    |   |   |   |   |           |   |____ Type: DEFINITION Content: Single_ID Depth: 13
                    |   |   |   |   |           |       �u|____ Type: ID Content: a Depth: 14
                    |   |   |   |   |           |____ Type: EXPRESSION Content: Const_Exp Depth: 12
                    |   |   |   |   |               |____ Type: CONST_INT Content: 1 Depth: 13
                    |   |   |   |   |____ Type: STATEMENT Content: Def_Stmt Depth: 9
                    |   |   |   |       �t|____ Type: DEFINITION Content: Def_Var Depth: 10
                    |   |   |   |           |____ Type: MODIFY Content: INT_TYPE Depth: 11
                    |   |   |   |           |____ Type: DEFINITION Content: Def_Single_Var Depth: 11
                    |   |   |   |               |____ Type: DEFINITION Content: Var_ONLY Depth: 12
                    |   |   |   |                   |____ Type: DEFINITION Content: Block_Single_Vardef Depth: 13
                    |   |   |   |                       as|____ Type: ID Content: b Depth: 14
                    |   |   |   |____ Type: STATEMENT Content: Exp_Stmt Depth: 8
                    |   |   |       |____ Type: OPERATION Content: Assign Depth: 9
                    |   |   |           �r|____ Type: EXPRESSION Content: ID_Exp Depth: 10
                    |   |   |           |   |____ Type: DEFINITION Content: Single_ID Depth: 11
                    |   |   |           |       |____ Type: ID Content: b Depth: 12
                    |   |   |           !r|____ Type: EXPRESSION Content: Const_Exp Depth: 10
                    |   |   |               |____ Type: CONST_INT Content: 2 Depth: 11
                    |   |   |____ Type: STATEMENT Content: Exp_Stmt Depth: 7
                    |   |       |____ Type: OPERATION Content: Assign Depth: 8
                    |   |           |____ Type: EXPRESSION Content: ID_Exp Depth: 9
                    |   |           |   !q|____ Type: DEFINITION Content: Single_ID Depth: 10
                    |   |           |       |____ Type: ID Content: a Depth: 11
                    |   |           |____ Type: OPERATION Content: Addition Depth: 9
                    |   |               �p|____ Type: EXPRESSION Content: ID_Exp Depth: 10
                    |   |               |   |____ Type: DEFINITION Content: Single_ID Depth: 11
                    |   |               |       |____ Type: ID Content: a Depth: 12
                    |   |               �o|____ Type: EXPRESSION Content: ID_Exp Depth: 10
                    |   |                   |____ Type: DEFINITION Content: Single_ID Depth: 11
                    |   |                       |____ Type: ID Content: b Depth: 12
                    |   1o|____ Type: STATEMENT Content: Exp_Stmt Depth: 6
                    |       |____ Type: CALL Content: Call_Args_Func Depth: 7
                    |           |____ Type: ID Content: print_int Depth: 8
                    |           |____ Type: ID Content: Func_Single_Arg Depth: 8
                    |               |____ Type: EXPRESSION Content: ID_Exp Depth: 9
                    |                   An|____ Type: DEFINITION Content: Single_ID Depth: 10
                    |                       |____ Type: ID Content: a Depth: 11
                    |____ Type: STATEMENT Content: Exp_Stmt Depth: 5
                        �m|____ Type: CALL Content: Call_Args_Func Depth: 6
                            |____ Type: ID Content: print_int Depth: 7
                            |____ Type: ID Content: Func_Single_Arg Depth: 7
                                |____ Type: EXPRESSION Content: ID_Exp Depth: 8
                                    |____ Type: DEFINITION Content: Single_ID Depth: 9
                                        �l|____ Type: ID Content: b Depth: 10
Gen Program
Gen Single_Block
Gen Def_Func_Body_Block
Gen INT_TYPE
Gen Func_NParam
Gen main
Gen Body
Body_Gen Some_Stmt
Body_Gen Some_Stmt
Body_Gen Some_Stmt
Body_Gen Some_Stmt
Body_Gen Some_Stmt
Body_Gen Some_Stmt
Body_Gen Some_Stmt
Body_Gen Def_Stmt
Body_Gen Def_Var
Body_Gen Def_Single_Var

Process finished with exit code 132
```