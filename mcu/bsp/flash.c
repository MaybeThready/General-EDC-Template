#include "flash.h"

bool flash_write_by_sector(uint32_t* data, uint32_t size, uint32_t address, bool erase_sector)
{
    if (address < FLASH_BASE_ADDR || address % FLASH_ALIGN_FACTOR != 0)
    {
        return false;
    }

    if (size == 0)
    {
        return true;
    }
    
    DL_FLASHCTL_COMMAND_STATUS status;

    // 擦除扇区
    if (erase_sector)
    {
        DL_FlashCTL_unprotectSector(FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
        status = DL_FlashCTL_eraseMemoryFromRAM(FLASHCTL, address, DL_FLASHCTL_COMMAND_SIZE_SECTOR);
        if (status != DL_FLASHCTL_COMMAND_STATUS_PASSED)
        {
            return false;
        }
    }

    // 写入数据
    uint32_t write_cnt = (size + 3) / 4;  // 计算需要写入的32位字数
    uint32_t full_blocks = write_cnt / 2;  // 每个块包含两个32位字
    uint32_t current_address = address;

    for (uint32_t i = 0; i < full_blocks; i++)
    {
        DL_FlashCTL_unprotectSector(FLASHCTL, current_address, DL_FLASHCTL_REGION_SELECT_MAIN);
        status = DL_FlashCTL_programMemoryFromRAM64WithECCGenerated(FLASHCTL, current_address, &data[2 * i]);
        if (status != DL_FLASHCTL_COMMAND_STATUS_PASSED)
        {
            return false;
        }
        current_address += 8;
    }

    if (write_cnt % 2 != 0) // 如果有剩余的一个32位字需要写入
    {
        uint32_t pending_word = data[write_cnt - 1];
        uint32_t fill_array[2] = {pending_word, 0xFFFFFFFF}; // 填充剩余部分为0xFFFFFFFF
        DL_FlashCTL_unprotectSector(FLASHCTL, current_address, DL_FLASHCTL_REGION_SELECT_MAIN);
        status = DL_FlashCTL_programMemoryFromRAM64WithECCGenerated(FLASHCTL, current_address, fill_array);
        if (status != DL_FLASHCTL_COMMAND_STATUS_PASSED)
        {
            return false;
        }
    }
    return true;
}

bool flash_erase_sector(uint32_t sector_idx)
{
    uint32_t address = FLASH_SECTOR_ADDR(sector_idx);
    DL_FlashCTL_unprotectSector(FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
    DL_FLASHCTL_COMMAND_STATUS status = DL_FlashCTL_eraseMemoryFromRAM(FLASHCTL, address, DL_FLASHCTL_COMMAND_SIZE_SECTOR);
    return status == DL_FLASHCTL_COMMAND_STATUS_PASSED;
}

bool flash_save(uint32_t* data, uint32_t size, uint32_t address, bool erase_sector)
{
    if (address < FLASH_BASE_ADDR || address % FLASH_ALIGN_FACTOR != 0)
    {
        return false;
    }

    uint32_t max_sector_idx = (address + size - 1) / FLASH_SECTOR_SIZE;
    uint32_t current_sector_idx = address / FLASH_SECTOR_SIZE;
    uint32_t current_address = address;
    uint32_t* current_data_ptr = data;

    while (current_sector_idx < max_sector_idx)
    {
        uint32_t write_size = FLASH_SECTOR_ADDR(current_sector_idx + 1) - current_address;
        if (!flash_write_by_sector(current_data_ptr, write_size, current_address, erase_sector))
        {
            return false;
        }
        current_address += write_size;
        current_data_ptr += (write_size + 3) / 4;
        current_sector_idx++;
    }

    uint32_t remaining_size = (address + size) - current_address;
    if (remaining_size > 0)
    {
        if (!flash_write_by_sector(current_data_ptr, remaining_size, current_address, erase_sector))
        {
            return false;
        }
    }

    return true;
}

bool flash_load(uint32_t* data, uint32_t size, uint32_t address)
{
    if (address < FLASH_BASE_ADDR || address % FLASH_ALIGN_FACTOR != 0)
    {
        return false;
    }

    uint32_t read_cnt = (size + 3) / 4;
    uint32_t current_address = address;

    // 再读取数据
    for (uint32_t i = 0; i < read_cnt; i += 2)
    {
        data[i] = *(volatile uint32_t*)(current_address);
        if (i + 1 < read_cnt)
        {
            data[i + 1] = *(volatile uint32_t*)(current_address + 4);
        }
        current_address += 8;
    }

    return true;
}