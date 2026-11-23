CC ?= aarch64-none-elf-gcc
LD ?= aarch64-none-elf-ld
OBJCOPY ?= aarch64-none-elf-objcopy

KP_ROOT ?= ../..
HDR_PATH ?= .

CFLAGS := -ffreestanding -nostdlib -nostdinc -fno-builtin
CFLAGS += -I$(KP_ROOT)/kernel/include
CFLAGS += -I$(KP_ROOT)/kernel/patch/include
CFLAGS += -I$(HDR_PATH)/include
CFLAGS += -Wall -Wextra

LDFLAGS := -r

SRC := mirror_shmem.c
OBJ := $(SRC:.c=.o)
OUT := mirror_shmem.kpm

all: $(OUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $@
	$(OBJCOPY) --strip-debug $@

clean:
	rm -f $(OBJ) $(OUT)
