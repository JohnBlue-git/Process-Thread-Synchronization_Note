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