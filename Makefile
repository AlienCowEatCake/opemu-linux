KERNEL_PATH ?= /lib/modules/$(shell uname -r)/build

MODULE_NAME  = opemu

$(MODULE_NAME)-objs := trap_hook.o \
                       optrap.o \
                       aesins.o \
                       pcmpstr.o \
                       fpins.o \
                       half.o \
                       aes.o \
                       avx.o \
                       vgather.o \
                       fma.o \
                       f16c.o \
                       bmi.o \
                       vsse.o \
                       vsse2.o \
                       vsse3.o \
                       vssse3.o \
                       vsse41.o \
                       vsse42.o

obj-m += $(MODULE_NAME).o

KBUILD_CFLAGS += -g -O2 -march=native -mtune=native -mmmx -msse -msse2

export KBUILD_CFLAGS

all:
	make -C $(KERNEL_PATH) M=$(PWD) modules

clean:
	make -C $(KERNEL_PATH) M=$(PWD) clean
