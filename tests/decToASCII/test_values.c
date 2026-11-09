#include "../common/check.h"
extern void decToASCII(char *s, int v);

int main(void) {
    char buf[16];
    const int INT_MAX_ = 2147483647;
    const int INT_MIN_ = (int)0x80000000u;

    #define T(v, expect, code) do { decToASCII(buf, (v)); CHECK(t_streq(buf, (expect)), (code)); } while (0)

    T(0,            " 0000000000", 1);
    T(1,            " 0000000001", 2);
    T(2,            " 0000000002", 3);
    T(100,          " 0000000100", 4);
    T(255,          " 0000000255", 5);
    T(256,          " 0000000256", 6);
    T(-1,           "-0000000001", 7);
    T(-10,          "-0000000010", 8);
    T(1000000000,   " 1000000000", 9);
    T(INT_MAX_,     " 2147483647", 10);
    T(INT_MAX_ - 1, " 2147483646", 11);
    T(INT_MIN_,     "-2147483648", 12);
    T(INT_MIN_ + 1, "-2147483647", 13);
    T(-1000000000,  "-1000000000", 14);

    /* Wrap-around: INT_MAX + 1 == INT_MIN */
    T((int)((unsigned)INT_MAX_ + 1u), "-2147483648", 15);

    /* Exactly 12 bytes written: canary after the NUL must survive */
    t_fill(buf, 16, 0x55);
    decToASCII(buf, -42);
    CHECK(buf[11] == 0 && buf[12] == 0x55, 16);

    /* Sweep against an independent reference */
    for (int v = -3000; v <= 3000; v++) {
        char ref[12];
        unsigned int m = v < 0 ? 0u - (unsigned)v : (unsigned)v;
        ref[0] = v < 0 ? '-' : ' ';
        for (int i = 10; i >= 1; i--) { ref[i] = (char)('0' + m % 10); m /= 10; }
        ref[11] = 0;
        decToASCII(buf, v);
        CHECK(t_streq(buf, ref), 17);
    }
    return 0;
}
