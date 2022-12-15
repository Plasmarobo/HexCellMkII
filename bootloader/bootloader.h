#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#define BOOTLOADER_ADDRESS (0xFFFFFFFF)

void bootloader_init(void);
void bootloader_update(void);

#endif