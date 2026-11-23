TARGET_COMPILE ?= aarch64-none-elf-
CC      := $(TARGET_COMPILE)gcc
LD      := $(TARGET_COMPILE)ld
OBJCOPY := $(TARGET_COMPILE)objcopy

# 子模块存放于仓库根目录KernelPatch文件夹
KP_DIR ?= ../KernelPatch

CFLAGS  := -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector
CFLAGS  += -I$(KP_DIR)/kpm/include
CFLAGS  += -Wall -Wextra
LDFLAGS := -r

SRC := mirror_shmem.c
OBJ := $(SRC:.c=.o)
OUT := mirror_shmem.kpm

all: $(OUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $@

clean:
	rm -f $(OBJ) $(OUT)
