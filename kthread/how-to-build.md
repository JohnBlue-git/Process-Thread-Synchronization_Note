
## Build

install necesarries
```console
# apt update
sudo apt-get update

# build tool
sudo apt-get install build-essential

# linux header
sudo apt-get install linux-headers-$(uname -r)
```

Makefile:   This will create a file named $(TARGET).ko (the kernel object file)
```makefile
# This line tells make to build $(TARGET).o as a module
obj-m +=$(TARGET).o

# Print variables (useful for debugging)
print:
	@echo "Target executable: $(TARGET)"

# The 'all' target builds the module
all:
    # Use make to build the module in the kernel's build directory
	@echo "make $(TARGET).ko ..."
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# The 'clean' target removes the module build artifacts
clean:
    # Use make to clean the module in the kernel's build directory
	@echo "clear $(TARGET) ..."
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

make command
```console
make print TARGET=<file name>

make TARGET=<file name>

make clean
```

## Load and Unload

```console
# Load the Kernel Module
sudo insmod ${TARGET}.ko

# To check if the module is loaded
lsmod | grep ${TARGET}

# Check Kernel Messages
dmesg | tail

# Unload the Kernel Module
sudo rmmod ${TARGET}
```

## Hints:

Makefile Variables with Defaults
```makefile
# Default values
CC ?= gcc
CFLAGS ?= -Wall
TARGET ?= my_program

# Build target
all:
    $(CC) $(CFLAGS) -o $(TARGET) main.c

# Clean target
clean:
    rm -f $(TARGET)
```

Running make with parameters
```console
make CC=clang CFLAGS="-O2 -g" TARGET=optimized_program
```
