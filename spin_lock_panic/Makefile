#DEBUG=Y



ifneq ($(KERNELRELEASE),)
#scull-objs := main.o
obj-m :=spin_lock_panic.o
else
KERNELDIR ?=/lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	rm -rf *.o *.ko *.mod.c *.cmd *.order *symvers *unsigned *.out ./.c*

