int set(int** q, int v) {
    **q = v;
    return 0;
}
int main() {
    int x;
    int* p;
    x = 1;
    p = &x;
    set(&p, 42);
    print_int(x);   // 42（int** 参数）
    return 0;
}
