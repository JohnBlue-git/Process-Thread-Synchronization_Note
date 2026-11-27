MODULES := basic_kthread mutex semaphore spinlock rwsem seqlock atomic
MODULE ?= all

ifeq ($(MODULE),all)
obj-m := $(addsuffix .o,$(MODULES))
else ifneq ($(filter $(MODULE),$(MODULES)),$(MODULE))
$(error Unknown MODULE '$(MODULE)'; choose one of: $(MODULES))
else
obj-m := $(MODULE).o
endif

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

.PHONY: all clean
