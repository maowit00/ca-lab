/* Tiny freestanding helpers for C tests (no libc available). */
#ifndef CHECK_H
#define CHECK_H

static int t_streq(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}

static void t_fill(char *p, int n, char v) {
    for (int i = 0; i < n; i++) p[i] = v;
}

/* Fail with a unique return code from main(). */
#define CHECK(cond, code) do { if (!(cond)) return (code); } while (0)

#endif
