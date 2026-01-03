#ifndef __BL_CONFIG_H__
#define __BL_CONFIG_H__

#define   TMS570LC43
#define CRYSTAL_FREQ            16             //MHz
#define SYS_CLK_FREQ            150            //MHz

#define APP_START_ADDRESS       0x00200100
#define APP_STATUS_ADDRESS       0x00200000

#define UART              sciREG3   /* Use UART port 1 for UART boot */
#define BUFFER_SIZE             64       /*words in the data buffer used for receiving packets*/

#endif // __BL_CONFIG_H__
