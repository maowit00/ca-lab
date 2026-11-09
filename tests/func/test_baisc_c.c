extern int func(int a, int b);

int main(void) {
    if (func(2, 3) != 0)  return 1;
    if (func(-1, 1) != 0) return 2;
    return 0;
}
