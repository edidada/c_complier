int main() {
    int x;
    int* p;
    int** pp;
    x = 7;
    p = &x;
    pp = &p;
    **pp = 99;
    print_int(x);    // 99（多重指针写）
    print_int(*p);   // 99
    *p = 55;
    print_int(**pp); // 55（多重指针读）
    return 0;
}
