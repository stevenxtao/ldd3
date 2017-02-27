#DEBUG=Y



ifneq ($(KERNELRELEASE),)
#scull-objs := main.o
obj-m :=chardummy.o
else
KERNELDIR ?=/lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

test:
	gcc main.c
	mv a.out test.out

clean:
	rm -rf *.o *.ko *.mod.c *.cmd *.order *symvers *unsigned *.out

