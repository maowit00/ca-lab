#include "../common/check.h"
extern int my_strlen(char *s);

int main(void) {
    CHECK(my_strlen((char *)"") == 0, 1);
    CHECK(my_strlen((char *)"a") == 1, 2);
    CHECK(my_strlen((char *)"Hello, World!") == 13, 3);
    CHECK(my_strlen((char *)"ab\0cd") == 2, 4);          /* stops at first NUL */
    CHECK(my_strlen((char *)"0123456789 0123456789") == 21, 5);
    return 0;
}
