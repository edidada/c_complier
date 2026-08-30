int add(int a, int b) {
    return a + b;
}
int f(int x) {
    return add(x, x * 2);
}
int main() {
    print_int(f(5));
    return 0;
}
