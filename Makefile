# Makefile: RISC-V GCC toolchain over QEMU

AS    = riscv64-linux-gnu-as
LD    = riscv64-linux-gnu-ld
ARCH  = rv32im
ABI   = ilp32
QEMU  = qemu-riscv32

PROG ?= hello

.PHONY: default clean
.SECONDARY:

default: $(PROG).run

%.run: %.exe
	$(QEMU) ./$<

%.exe: %.o
	$(LD) -m elf32lriscv -o $@ $<

%.debug: %.exe
	@$(QEMU) -g 1234 ./$< & QPID=$$!; \
	sleep 0.5; \
	gdb-multiarch -q \
	    -ex "set confirm off" \
	    -ex "set architecture riscv:rv32" \
	    -ex "target remote :1234" \
	    -ex "file $<" \
	    -ex "layout regs"; \
	kill $$QPID 2>/dev/null || true

%.o: %.s
	$(AS) -march=$(ARCH) -mabi=$(ABI) -g -o $@ $<

clean:
	rm -f *.o *.exe
