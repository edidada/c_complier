int main() {
    int a[2][3];
    int i, j, s;
    a[0][0] = 1; a[0][1] = 2; a[0][2] = 3;
    a[1][0] = 4; a[1][1] = 5; a[1][2] = 6;
    s = 0; i = 0;
    while(i < 2) {
        j = 0;
        while(j < 3) {
            s = s + a[i][j];
            j = j + 1;
        }
        i = i + 1;
    }
    print_int(s);       // 21
    print_int(a[1][2]); // 6（变量下标）
    print_int(a[0][2]); // 3（变量下标）
    print_int(a[1][1]); // 5
    return 0;
}
