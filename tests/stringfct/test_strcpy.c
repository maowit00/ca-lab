#include "../common/check.h"
extern void my_strcpy(char *dst, char *src);

int main(void) {
    char buf[16];

    t_fill(buf, 16, 'X');
    my_strcpy(buf, (char *)"Hello");
    CHECK(t_streq(buf, "Hello"), 1);
    CHECK(buf[5] == 0, 2);                 /* NUL copied */
    CHECK(buf[6] == 'X', 3);               /* nothing written past NUL */

    t_fill(buf, 16, 'X');
    my_strcpy(buf, (char *)"");
    CHECK(buf[0] == 0 && buf[1] == 'X', 4);   /* empty string */

    t_fill(buf, 16, 'X');
    my_strcpy(buf, (char *)"A,b.1");
    CHECK(t_streq(buf, "A,b.1"), 5);
    return 0;
}
