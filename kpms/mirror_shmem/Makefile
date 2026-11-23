CC := $(TARGET_COMPILE)gcc
LD := $(TARGET_COMPILE)ld
OBJCOPY := $(TARGET_COMPILE)objcopy

KP_ROOT ?= ../../KernelPatch
HDR ?= ../hdr

CFLAGS := -ffreestanding -nostdlib -nostdinc -fno-builtin
CFLAGS += -I$(KP_ROOT)/kernel/include
CFLAGS += -I$(KP_ROOT)/kernel/patch/include
CFLAGS += -I$(HDR)/include
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
