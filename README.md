# Snake for ESP32 and ILI9341

This is a modified version of @HailTheBDFL ( Tyler Edwards ) snake game.

 _Patched bugs, added music (beeps), remade the input buttons, made food not spawn on the snake._ 

- 1x ESP32
- 1x buzzer
- 5x buttons ( up/down/left/right and select )
- ILI9341 TFT LCD

Pinout for this LCD 2.8 inch 240*320 
- GND (power ground)
- VCC (power 3.3V)
- CLK (clock line)
- MOSI (data input)
- RES (reset line)
- DC (data line)
- BLK (backlight)
- MISO (data output)
- CS1 (LCD chip select)
- CS2 (touch select)
- PEN (touch interrupt signal)

[Original source] (https://github.com/HailTheBDFL/esp32-snake)

Modified for a project.
