#include "../common/check.h"
extern int my_strtolower(char *s);
extern void my_strcpy(char *dst, char *src);

int main(void) {
    char buf[64];
    int n;

    my_strcpy(buf, (char *)"Hello, WORLD 123!");
    n = my_strtolower(buf);
    CHECK(t_streq(buf, "hello, world 123!"), 1);
    CHECK(n == 17, 2);

    /* Characters right outside 'A'..'Z' / 'a'..'z' must stay untouched */
    my_strcpy(buf, (char *)"@[`{_ ^\\]");
    n = my_strtolower(buf);
    CHECK(t_streq(buf, "@[`{_ ^\\]"), 3);
    CHECK(n == 9, 4);

    my_strcpy(buf, (char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    my_strtolower(buf);
    CHECK(t_streq(buf, "abcdefghijklmnopqrstuvwxyz"), 5);

    my_strcpy(buf, (char *)"");
    CHECK(my_strtolower(buf) == 0, 6);

    my_strcpy(buf, (char *)"already lower 0-9");
    my_strtolower(buf);
    CHECK(t_streq(buf, "already lower 0-9"), 7);
    return 0;
}
