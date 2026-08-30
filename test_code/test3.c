int main(){
    int a = 0;
    int result = 1;
    int b = 0;
    while(a < 10){
        a = a+1;
        result = result*(a+1);
        if(result > 20){
            b= b+1;
        }
    }
    print_int(result); //39916800 (11! = 3628800*11, while 循环 a 从 0 到 9 共 10 次)
    print_int(b); //8

    //result 序列: 2 6 24 120 720 5040 40320 362880 3628800 39916800
    //b 序列: 0 0 1 2 3 4 5 6 7 8
}