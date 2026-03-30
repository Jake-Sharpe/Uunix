#include "Disk.hpp"
#include "../drivers/Port.hpp"
bool DiskAvailable() { return inb(0x1F7) & 0x08; }

uint8_t DisksAvailable() {
  uint8_t disks = 0;

  // Helper: wait for BSY to clear, then drain 256 words if DRQ is set
  auto drain_identify = [](uint16_t status_port, uint16_t data_port) {
    // Wait for BSY to clear (with timeout)
    for (int timeout = 0; timeout < 100000; timeout++) {
      uint8_t s = inb(status_port);
      if (!(s & 0x80))
        break; // BSY cleared
    }
    uint8_t status = inb(status_port);
    if (status & 0x08) { // DRQ set - data waiting
      for (int i = 0; i < 256; i++) {
        inw(data_port); // drain 256 words
      }
    }
    return (uint8_t)(status != 0 ? 1 : 0);
  };

  if (inb(0x1F7) != 0xFF) {
    // Check Primary Master
    outb(0x1F6, 0xA0);
    outb(0x1F7, 0xEC);
    if (drain_identify(0x1F7, 0x1F0))
      disks |= 1;

    // Check Primary Slave
    outb(0x1F6, 0xB0);
    outb(0x1F7, 0xEC);
    if (drain_identify(0x1F7, 0x1F0))
      disks |= 2;
  }

  if (inb(0x177) != 0xFF) {
    // Check Secondary Master
    outb(0x176, 0xA0);
    outb(0x177, 0xEC);
    if (drain_identify(0x177, 0x170))
      disks |= 4;

    // Check Secondary Slave
    outb(0x176, 0xB0);
    outb(0x177, 0xEC);
    if (drain_identify(0x177, 0x170))
      disks |= 8;
  }

  return disks;
}

void SetDisk(uint8_t disk) {
  if (disk < 2) {
    // Primary Bus
    outb(0x1F6, (disk == 0) ? 0xA0 : 0xB0);
  } else {
    // Secondary Bus
    outb(0x176, (disk == 2) ? 0xA0 : 0xB0);
  }
  // Very important: The hardware needs a tiny bit of time to switch
  for (int i = 0; i < 4000; i++)
    inb(0x1F7);
}

void ReadSector(uint32_t lba, uint8_t *buffer) {
  outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
  outb(0x1F2, 1);
  outb(0x1F3, (uint8_t)lba);
  outb(0x1F4, (uint8_t)(lba >> 8));
  outb(0x1F5, (uint8_t)(lba >> 16));
  outb(0x1F7, 0x20);
  while (inb(0x1F7) & 0x80)
    ;
  while (!(inb(0x1F7) & 0x08))
    ;
  for (int i = 0; i < 256; i++) {
    ((uint16_t *)buffer)[i] = inw(0x1F0);
  }
}