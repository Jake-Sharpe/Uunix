#ifndef DISK_H
#define DISK_H

#include "../drivers/Port.hpp"

bool DiskAvailable();
uint8_t DisksAvailable();
void ReadSector(uint32_t lba, uint8_t *buffer);
void SetDisk(uint8_t disk);
#endif