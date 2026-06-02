# STM32G431KBT-SH1106-OLED-DISPLAY-U8G2-LIBRARY
STM32G431KBT-SH1106-OLED-DISPLAY-U8G2-LIBRARY

OLED Pin -- STM32 Pin:

GND -- GND
VCC -- +3V3

CUBEMX ВЫСТАВЛЯЕТ САМ АВТОМАТОМ
ПРИ ВКЛЮЧЕНИИ SPI:
SCK -- PA5 (SPI1_SCK)
SDA -- PA7 (SPI1_MOSI)
RES -- PA6 (SPI1_MISO)

ВЫСТАВИТЬ САМОСТОЯТЕЛЬНО ВРУЧНУЮ:
DC --  PA4 ( GPIO_Output )
CS --  PB0 ( GPIO_Output )

ДОПОЛНИТЕЛЬНО МОЖНО ВКЛЮЧИТЬ БОРТОВОЙ LED НА PB8 (LED_PB8)

ПАРАМЕТРЫ SPI1:
Data Size: 8Bits
Prescaler (for Baud Rate): 256
Всё остальное по умолчанию.

ЧТОБЫ CUBE IDE "УВИДЕЛА" БИБЛИОТЕКУ U8G2 НАДО ЕЁ ДОБАВИТЬ В СВОЙСТВАХ ПРОЕКТА
(СМ.СКРИНШОТ)
