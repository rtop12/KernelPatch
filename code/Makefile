TARGET_COMPILE ?= aarch64-linux-gnu-
CC      := $(TARGET_COMPILE)gcc
LD      := $(TARGET_COMPILE)ld
OBJCOPY := $(TARGET_COMPILE)objcopy

# 仓库内置 KernelPatch 头文件
KP_DIR := ../kp_kernel
# apt 安装的 ARM64 Linux 标准头文件
SYS_HDR := /usr/aarch64-linux-gnu/include

CFLAGS  := -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector
CFLAGS  += -I$(KP_DIR)/include
CFLAGS  += -I$(KP_DIR)/patch/include
CFLAGS  += -I$(SYS_HDR)
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
	$(OBJCOPY) --strip-debug $@

clean:
	rm -f $(OBJ) $(OUT)
