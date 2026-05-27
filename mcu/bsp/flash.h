/**
 * @file flash.h
 * @author Thready
 * @brief FLASH读写模块
 * 读写时需要注意对扇区的处理。写入数据前必须要先擦除，可以以选择在写入时自动擦除扇区，也可以先调用flash_erase_sector函数擦除扇区后再写入。擦除和写入都是以扇区为单位的，不能跨扇区操作。
 * flash_write_by_sector函数要求数据必须在同一个扇区内，flash_save函数可以跨扇区写入数据，内部会自动处理跨扇区的情况。flash_load函数也可以跨扇区读取数据。
 * @version 0.1
 * @date 2026-05-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include "../ti_msp_dl_config.h"

// Flash存储起始地址。越大可存储空间越小，代码区和数据区的越大。必须是8字节对齐的地址。
#define FLASH_BASE_ADDR (0x00004000)

// Flash写入对齐要求，必须是8字节对齐
#define FLASH_ALIGN_FACTOR (8)

// Flash扇区大小，单位字节
#define FLASH_SECTOR_SIZE (1024)

// 根据地址获取扇区号
#define FLASH_SECTOR_IDX(addr) (addr / FLASH_SECTOR_SIZE)

// 根据扇区号获取首地址
#define FLASH_SECTOR_ADDR(idx) (idx * FLASH_SECTOR_SIZE)

/**
 *@brief 将数据写入Flash，要求数据在同一个扇区
 * 
 * @param data RAM数据首地址指针
 * @param size RAM数据大小，单位字节
 * @param address FLASH写入首地址，必须是8字节对齐的地址
 * @param erase_sector 是否擦除扇区
 * @return true 写入成功
 * @return false 写入失败
 */
bool flash_write_by_sector(uint32_t* data, uint32_t size, uint32_t address, bool erase_sector);

/**
 *@brief 擦除Flash扇区
 * 
 * @param sector_idx 扇区号
 * @return true 擦除成功
 * @return false 擦除失败
 */
bool flash_erase_sector(uint32_t sector_idx);

/**
 *@brief 将数据保存到Flash，可以跨扇区
 * 
 * @param data RAM数据首地址指针
 * @param size RAM数据大小，单位字节
 * @param address FLASH保存首地址，必须是8字节对齐的地址
 * @param erase_sector 是否擦除扇区
 * @return true 保存成功
 * @return false 保存失败
 */
bool flash_save(uint32_t* data, uint32_t size, uint32_t address, bool erase_sector);

/**
 *@brief 从Flash加载数据
 * 
 * @param data RAM数据首地址指针
 * @param size RAM数据大小，单位字节
 * @param address FLASH加载首地址，必须是8字节对齐的地址
 * @return true 加载成功
 * @return false 加载失败
 */
bool flash_load(uint32_t* data, uint32_t size, uint32_t address);
