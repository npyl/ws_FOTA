/*
 * Flash.h
 *
 *  Created on: Nov 27, 2025
 *      Author: npyla
 */

#ifndef APP_FLASH_H_
#define APP_FLASH_H_


/* MCU Address Space, values for STM32F452 */
/* Total Flash memory */
#define FLASH_ADDRESS_ALL_MIN       0x08000000
#define FLASH_ADDRESS_ALL_MAX       0x0807FFFF

#define MEMSPACE_BL                 0x20000
#define MEMSPACE_APP                0x2F000
#define MEMSPACE_CFG                0x00800

#define FLASH_ADDRESS_BL_MIN        FLASH_ADDRESS_ALL_MIN
#define FLASH_ADDRESS_BL_MAX        FLASH_ADDRESS_ALL_MIN + MEMSPACE_BL - 1

/* Application space 1, 138KB */
#define FLASH_ADDRESS_APP1_MIN      FLASH_ADDRESS_BL_MAX + 1
#define FLASH_ADDRESS_APP1_MAX      FLASH_ADDRESS_APP1_MIN + MEMSPACE_APP - 1

/* Application space 2, 138KB */
#define FLASH_ADDRESS_APP2_MIN      FLASH_ADDRESS_APP1_MAX + 1
#define FLASH_ADDRESS_APP2_MAX      FLASH_ADDRESS_APP2_MIN + MEMSPACE_APP - 1

/* Configuration space, 2KBx2 */
#define FLASH_ADDRESS_CFG1_MIN       FLASH_ADDRESS_ALL_MAX - MEMSPACE_CFG*2 + 1
#define FLASH_ADDRESS_CFG1_MAX       FLASH_ADDRESS_ALL_MAX
#define FLASH_ADDRESS_CFG2_MIN       FLASH_ADDRESS_ALL_MAX - MEMSPACE_CFG + 1
#define FLASH_ADDRESS_CFG2_MAX       FLASH_ADDRESS_ALL_MAX

/* RAM block, 128KB */
#define RAM_ADDRESS_1_MIN        0x20000000
#define RAM_ADDRESS_1_MAX        0x2001FFFF

#define c_PAGE_SIZE_Bytes       2048        /*!< STM32F452 */

#define FLASH_PROTECTED_ADDRESS  0x08003FFF
#define FLASH_ADDR_OFFSET        0x44000000

#endif /* APP_FLASH_H_ */
