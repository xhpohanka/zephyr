/*
  * Copyright (c) 2020 Jan Pohanka
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <device.h>
#include <string.h>
#include <drivers/flash.h>
#include <init.h>
#include <soc.h>

#include "flash_stm32.h"

#define STM32H7X_SECTOR_MASK		((uint32_t) ~(1 << 8))

bool flash_stm32_valid_range(const struct device *dev, off_t offset, uint32_t len,
			     bool write)
{
	ARG_UNUSED(write);

	return flash_stm32_range_exists(dev, offset, len);
}

static int write_byte(const struct device *dev, off_t offset, uint8_t val)
{
	FLASH_TypeDef *regs = FLASH_STM32_REGS(dev);
	int rc;

	/* if the control register is locked, do not fail silently */
	if (regs->CR1 & FLASH_CR_LOCK) {
		return -EIO;
	}

	rc = flash_stm32_wait_flash_idle(dev);
	if (rc < 0) {
		return rc;
	}

	/* prepare to write a single byte */
	regs->CR1 = (regs->CR1 & FLASH_CR_PSIZE_Msk) |
		   FLASH_PSIZE_BYTE | FLASH_CR_PG;
	/* flush the register write */
	__DSB();

	/* write the data */
	*((uint8_t *) offset + FLASH_BANK1_BASE) = val;
	/* flush the register write */
	__DSB();

	rc = flash_stm32_wait_flash_idle(dev);
	regs->CR1 &= (~FLASH_CR_PG);

	return rc;
}

static int erase_sector(const struct device *dev, uint32_t sector)
{
	FLASH_TypeDef *regs = FLASH_STM32_REGS(dev);
	int rc;

	/* if the control register is locked, do not fail silently */
	if (regs->CR1 & FLASH_CR_LOCK) {
		return -EIO;
	}

	rc = flash_stm32_wait_flash_idle(dev);
	if (rc < 0) {
		return rc;
	}

	/* Dual bank mode, SNB MSB selects the bank2,
	 * others select sector, so we remap sector number.
	 */
#if defined(FLASH_OPTCR_nDBANK) && FLASH_SECTOR_TOTAL == 24
#if CONFIG_FLASH_SIZE == 2048
	if (sector > 11) {
		sector += 4U;
	}
#elif CONFIG_FLASH_SIZE == 1024
	if (sector > 7) {
		sector += 8U;
	}
#endif /* CONFIG_FLASH_SIZE */
#endif /* defined(FLASH_OPTCR_nDBANK) && FLASH_SECTOR_TOTAL == 24 */

	regs->CR1 = (regs->CR1 & (FLASH_CR_PSIZE_Msk | STM32H7X_SECTOR_MASK)) |
		     FLASH_PSIZE_BYTE |
		     FLASH_CR_SER |
		     (sector << FLASH_CR_SNB_Pos) |
		     FLASH_CR_START;
	/* flush the register write */
	__DSB();

	rc = flash_stm32_wait_flash_idle(dev);
	regs->CR1 &= ~(FLASH_CR_SER | FLASH_CR_SNB);

	return rc;
}

int flash_stm32_block_erase_loop(const struct device *dev, unsigned int offset,
				 unsigned int len)
{
	struct flash_pages_info info;
	uint32_t start_sector, end_sector;
	uint32_t i;
	int rc = 0;

	rc = flash_get_page_info_by_offs(dev, offset, &info);
	if (rc) {
		return rc;
	}
	start_sector = info.index;
	rc = flash_get_page_info_by_offs(dev, offset + len - 1, &info);
	if (rc) {
		return rc;
	}
	end_sector = info.index;

	for (i = start_sector; i <= end_sector; i++) {
		rc = erase_sector(dev, i);
		if (rc < 0) {
			break;
		}
	}

	return rc;
}

int flash_stm32_write_range(const struct device *dev, unsigned int offset,
			    const void *data, unsigned int len)
{
	int i, rc = 0;

	for (i = 0; i < len; i++, offset++) {
		rc = write_byte(dev, offset, ((const uint8_t *) data)[i]);
		if (rc < 0) {
			return rc;
		}
	}

	return rc;
}


/* Some SoC can run in single or dual bank mode, others can't.
 * Different SoC flash layouts are specified in various reference
 * manuals, but the flash layout for a given number of sectors is
 * consistent across these manuals. The number of sectors is given
 * by the HAL as FLASH_SECTOR_TOTAL. And some SoC that with same
 * FLASH_SECTOR_TOTAL have different flash size.
 *
 * In case of 8 sectors and 24 sectors we need to differentiate
 * between two cases by using the memory size.
 * In case of 24 sectors we need to check if the SoC is running
 * in single or dual bank mode.
 */
#ifndef FLASH_SECTOR_TOTAL
#error "Unknown flash layout"
#elif FLASH_SECTOR_TOTAL == 1
static const struct flash_pages_layout stm32h7_flash_layout[] = {
	/* RM0433, table 11: STM32H750xx */
	{.pages_count = 1, .pages_size = KB(128)},
};
#else
#error "Unknown flash layout"
#endif/* !defined(FLASH_SECTOR_TOTAL) */

void flash_stm32_page_layout(const struct device *dev,
			     const struct flash_pages_layout **layout,
			     size_t *layout_size)
{
#if FLASH_OPTCR_nDBANK // TODO! H7 way
	if (FLASH_STM32_REGS(dev)->OPTCR & FLASH_OPTCR_nDBANK) {
		*layout = stm32h7_flash_layout_single_bank;
		*layout_size = ARRAY_SIZE(stm32h7_flash_layout_single_bank);
	} else {
		*layout = stm32h7_flash_layout_dual_bank;
		*layout_size = ARRAY_SIZE(stm32h7_flash_layout_dual_bank);
	}
#else
	ARG_UNUSED(dev);
	*layout = stm32h7_flash_layout;
	*layout_size = ARRAY_SIZE(stm32h7_flash_layout);
#endif
}
