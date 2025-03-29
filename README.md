# stm32f103c8t6_modbus_read_sync_async

In STM32CubeMX:
```
Start My project from MCU
STM32F103C8T6
System Core
  SYS -> Debug -> Serial Wire
  Connectivity -> USART2 -> Mode Asynchronous -> Baud Rate 9600
  Connectivity -> USART3 -> Mode Asynchronous -> Baud Rate 9600
    NVIC Settings -> Enable global interrupt
PC13 -> GPIO Output
Project Manager -> Toolchain -> Makefile
Code Generator -> Add necessary library files...
```
