obj-y := mvmdio_uio.o

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order .*.cmd .tmp*
