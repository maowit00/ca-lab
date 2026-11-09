
    .text
    .globl func
    .type  func, @function
# int func(int a, int b)
# a0 = a, b0 = b  ->  returns a + b in a0
func:
    add  a0, a0, a1
    ret
    .size func, .-func

