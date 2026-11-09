
    .text
    .globl main
main:
    addi sp, sp, -16
    sw   ra, 12(sp)

    li   a0, 2
    li   a1, 3
    call func
    li   t0, 5
    bne  a0, t0, fail

    li   a0, 0         # pass
    j    done
fail:
    li   a0, 1
done:
    lw   ra, 12(sp)
    addi sp, sp, 16
    ret
