CC      := aarch64-linux-gnu-gcc
LD      := aarch64-linux-gnu-ld
OBJCOPY := aarch64-linux-gnu-objcopy

CFLAGS  := -ffreestanding -fno-stack-protector -fno-pic -mcmodel=large \
           -O2 -Wall -Wextra -nostdlib -nostdinc -fno-builtin
LDFLAGS := -T payload.ld -nostdlib

all: kxnd.bin

kxnd.o: kxnd.c
	$(CC) $(CFLAGS) -c $< -o $@

kxnd.elf: kxnd.o payload.ld
	$(LD) $(LDFLAGS) -o $@ kxnd.o

kxnd.bin: kxnd.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f kxnd.o kxnd.elf kxnd.bin

.PHONY: all clean
