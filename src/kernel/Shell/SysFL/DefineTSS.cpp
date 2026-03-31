#include "DefineTSS.hpp"
#include "../../MemoryManager/Kmalloc.hpp"

extern "C" uint64_t gdt64_ptr;
TSS kernel_tss; // Create a global instance

void install_tss(uint64_t stack_top) {
  // 1. Setup the TSS structure
  kernel_tss.rsp0 =
      stack_top; // The stack the CPU should switch to on interrupt
  kernel_tss.iopb_offset = sizeof(TSS);

  // 2. Get the address of the TSS slot in your boot.asm GDT
  // Based on your order: 0x00, 0x08, 0x10, 0x18, 0x20 -> TSS is at 0x28
  uint64_t tss_base = (uint64_t)&kernel_tss;
  uint32_t tss_limit = sizeof(TSS) - 1;

  // We need to point to the .tss_low and .tss_high you added in boot.asm
  // You can export a label from asm or calculate it from the gdt64_ptr.base
  uint8_t *gdt_base = (uint8_t *)&gdt64_ptr;

  struct TSSDescriptor {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
    uint32_t base_upper32;
    uint32_t reserved;
  } __attribute__((packed)) *desc = (TSSDescriptor *)&gdt_base[0x28];

  desc->limit_low = tss_limit & 0xFFFF;
  desc->base_low = tss_base & 0xFFFF;
  desc->base_mid = (tss_base >> 16) & 0xFF;
  desc->access = 0x89; // Present, Executable, Accessed (TSS type)
  desc->granularity = ((tss_limit >> 16) & 0x0F);
  desc->base_high = (tss_base >> 24) & 0xFF;
  desc->base_upper32 = (tss_base >> 32) & 0xFFFFFFFF;

  // 3. Load the TSS into the CPU
  asm volatile("ltr %%ax" : : "a"(0x28));
}