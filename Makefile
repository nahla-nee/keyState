obj-m += keyState.o

all: create clean

create:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	rm -rf *.o *mod.c .*.cmd .tmp_versions .cache.mk Module.symvers modules.order