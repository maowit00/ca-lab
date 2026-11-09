#include "../common/check.h"
extern void hexToASCII(char *s, unsigned int v);

static void ref_hex(char *s, unsigned int v) {
    s[0] = '0'; s[1] = 'x';
    for (int i = 0; i < 8; i++) {
        unsigned int nib = (v >> (28 - 4 * i)) & 0xF;
        s[2 + i] = (char)(nib < 10 ? '0' + nib : 'A' + (nib - 10));
    }
    s[10] = 0;
}

int main(void) {
    char buf[16], ref[16];

    /* Fixed examples */
    hexToASCII(buf, 0xF018);
    CHECK(t_streq(buf, "0x0000F018"), 1);
    hexToASCII(buf, 0);
    CHECK(t_streq(buf, "0x00000000"), 2);
    hexToASCII(buf, 0xFFFFFFFFu);
    CHECK(t_streq(buf, "0xFFFFFFFF"), 3);
    hexToASCII(buf, 0x12345678u);
    CHECK(t_streq(buf, "0x12345678"), 4);
    hexToASCII(buf, 0x9ABCDEF0u);
    CHECK(t_streq(buf, "0x9ABCDEF0"), 5);
    hexToASCII(buf, 0x80000000u);
    CHECK(t_streq(buf, "0x80000000"), 6);

    /* Exactly 11 bytes written: canary after the NUL must survive */
    t_fill(buf, 16, 0x55);
    hexToASCII(buf, 0xABCD);
    CHECK(buf[10] == 0 && buf[11] == 0x55, 7);

    /* Lab corner cases: whole 16-bit range, then wrap-around to 0 */
    unsigned int c = 0;
    do {
        hexToASCII(buf, c);
        ref_hex(ref, c);
        CHECK(t_streq(buf, ref), 8);
        c = (c + 1) & 0xFFFF;
    } while (c != 0);
    hexToASCII(buf, c);
    CHECK(t_streq(buf, "0x00000000"), 9);
    return 0;
}
