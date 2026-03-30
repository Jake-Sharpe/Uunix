# Toolchain
NASM = nasm
CXX = g++ -m64
LD = ld -m elf_x86_64 --no-warn-rwx-segments

# Flags
# Added include paths for ALL subdirectories in src
CXXFLAGS = -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -nostdlib -fno-stack-protector \
           -mno-sse -mno-sse2 -mno-mmx -mno-80387 \
           -Isrc -Isrc/kernel -Isrc/drivers -Isrc/fs -Isrc/mm -Isrc/lib -Isrc/arch/x86_64

LDFLAGS = -n -T src/arch/x86_64/linker.ld

# Directories
SRC_DIR = src
ISO_DIR = iso
BOOT_DIR = $(ISO_DIR)/boot

# Find all source files recursively
CPPSOURCES = $(shell find $(SRC_DIR) -name "*.cpp")
ASMSOURCES = $(shell find $(SRC_DIR) -name "*.asm")

# Map source files to object files in the local directory
OBJECTS = $(patsubst %.cpp, %.o, $(notdir $(CPPSOURCES))) \
          $(patsubst %.asm, %.o, $(notdir $(ASMSOURCES)))

# Files
KERNEL_BIN = $(BOOT_DIR)/kernel.bin
KERNEL_ISO = kernel.iso
DISK_IMG = disk.img

all: $(KERNEL_ISO)

$(KERNEL_ISO): $(KERNEL_BIN)
	grub-mkrescue -o $@ $(ISO_DIR)
	@rm -f *.o $(KERNEL_BIN)

$(KERNEL_BIN): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

# VPATH allows make to find sources in subdirectories
VPATH = $(shell find $(SRC_DIR) -type d)

%.o: %.asm
	$(NASM) -f elf64 $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(KERNEL_BIN) $(KERNEL_ISO)

$(DISK_IMG):
	qemu-img create -f raw $@ 64M

run: $(KERNEL_ISO) $(DISK_IMG)
	qemu-system-x86_64 -cdrom $(KERNEL_ISO) -drive file=$(DISK_IMG),format=raw -serial stdio
	
runS: $(KERNEL_ISO)
	qemu-system-x86_64 -cdrom $(KERNEL_ISO) -drive file=disk.img,format=raw -serial stdio

.PHONY: all clean run
