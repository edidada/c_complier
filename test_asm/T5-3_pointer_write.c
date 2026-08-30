int main() {
    int a;
    int* p;
    p = &a;
    *p = 100;
    print_int(a);
    return 0;
}
