# Readme

Firmware Over The Air update Bootloader for TMS750LC43xx
(Testing on Launchxl2-tms570lc43xx)

## Implementation Details

- See Doc for initial implementation specifications (V4 STM32 version)

- Using [F021 API](https://www.ti.com/tool/F021FLASHAPI)
- Parts that use Fapi build & link successfully BUT don't work! Code was based on: [ws_UART_Boot](https://github.com/npyl/ws_UART_BOOT)
- Minor porting of Ilia's code!

  Plan is to replace lower level methods with code from ws_UART_Boot example (for flashing), timers, and uart to be converted to this MCU etc.
