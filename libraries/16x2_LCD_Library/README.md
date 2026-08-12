# I2C LCD driver board Arduino library

[![Make docs and publish to GitHub Pages](https://github.com/SolderedElectronics/Soldered-16x2-LCD-Arduino-Library/actions/workflows/make_docs.yml/badge.svg?branch=dev)](https://github.com/SolderedElectronics/Soldered-16x2-LCD-Arduino-Library/actions/workflows/make_docs.yml)

| ![I2C LCD driver board](https://github.com/SolderedElectronics/I2C-LCD-driver-board-hardware-design/blob/main/OUTPUTS/V1.2.2/333003.jpg) |
| :--------------------------------------------------------------------------------------------------------------------------------------: |

| [I2C LCD driver board](https://www.solde.red/333003)

This adapter makes connection of LCD to Dasduino (or any other Arduino-compatible board) a breeze! It uses I2C communication which needs only two pins. It can be used and work with all of ours LCDs (16x2, 20x4, 16x4). Use our library and get started in few minutes of work!

It's even easier with easyC - no soldering is needed!

- I2C address: 0x20 (HEX)
- Voltage: 3V
- Potenciometer for contrast at the back of the board
  (https://www.soldered.com/en/easyC)

| ![LCD display 16x2 I2C white characters on blue background](https://upload.wikimedia.org/wikipedia/commons/8/8f/Example_image.svg) |
| :--------------------------------------------------------------------------------------------------------------------------------: |

| [LCD display 16x2 I2C white characters on blue background](https://www.solde.red/333171)

LCD display with HD44780 controller enables showing text, symbols, value or anything else that comes from Croduino/Arduino. It is capable of showing 16 symbols in two lines and therefore can be used in many projects that need to send clearly visible and legible output information. An I2C LCD adapter is soldered to this LCD screen, which enables characters to be displayed on the LCD screen via the I2C bus. It also has an easyC connector for a really simple connection.

- Color of symbols: White
- Backlight: Blue
- Number of chars: 16 chars in 2 rows
- Dimensions: 80 x 36 x 12 mm
- Display dimensions: 64.5 x 16 mm
- Voltage: 3V3
- Communication protocol: I2C
- With soldered I2C adapter

| ![LCD display 20x4 I2C white characters on blue background](https://upload.wikimedia.org/wikipedia/commons/8/8f/Example_image.svg) |
| :--------------------------------------------------------------------------------------------------------------------------------: |

| [LCD display 20x4 I2C white characters on blue background](https://www.solde.red/333172)

LCD display with HD44780 controller enables showing text, symbols, value or anything else that comes from Croduino/Arduino. It is capable of showing 16 symbols in two lines and therefore can be used in many projects that need to send clearly visible and legible output information. An I2C LCD adapter is soldered to this LCD screen, which enables characters to be displayed on the LCD screen via the I2C bus. It also has an easyC connector for a really simple connection.

- Color of symbols: White
- Backlight: Blue
- Number of chars: 20 chars in 4 rows
- Dimensions: 98 x 60 x 12 mm
- Display dimensions: 77 x 25,2 mm
- Voltage: 3V3
- Communication protocol: I2C
- With soldered I2C adapter

### Repository Contents

- **/src** - source files for the library (.h & .cpp)
- **/examples** - examples for using the library
- **_other_** - _keywords_ file highlights function words in your IDE, _library.properties_ enables implementation with Arduino Library Manager.

### Hardware design

You can find hardware design for this board in [_I2C LCD driver board_](https://github.com/SolderedElectronics/I2C-LCD-driver-board-hardware-design) hardware repository.

You can find hardware design for this board in [_LCD display 16x2 I2C white characters on blue background_](https://github.com/SolderedElectronics/NAZIVPROIZVODA-hardware-design) hardware repository.

You can find hardware design for this board in [_LCD display 20x4 I2C white characters on blue background_](https://github.com/SolderedElectronics/NAZIVPROIZVODA-hardware-design) hardware repository.

### Documentation

Access Arduino library documentation [here](https://SolderedElectronics.github.io/Soldered-16x2-LCD-Arduino-Library/).

- Tutorial for using the NAZIV PROIZVODA board
- Installing an Arduino library

### Board compatibility

The library is compatible with board & microcontroller families shown in green below:

[![Compile Sketches](http://github-actions.40ants.com/e-radionicacom/Soldered-16x2-LCD-Arduino-Library/matrix.svg?branch=dev&only=Compile%20Sketches)](https://github.com/SolderedElectronics/Soldered-16x2-LCD-Arduino-Library/actions/workflows/compile_test.yml)

### About Soldered

<img src="https://raw.githubusercontent.com/e-radionicacom/Soldered-16x2-LCD-Arduino-Library/dev/extras/Soldered-logo-color.png" alt="soldered-logo" width="500"/>

At Soldered, we design and manufacture a wide selection of electronic products to help you turn your ideas into acts and bring you one step closer to your final project. Our products are intented for makers and crafted in-house by our experienced team in Osijek, Croatia. We believe that sharing is a crucial element for improvement and innovation, and we work hard to stay connected with all our makers regardless of their skill or experience level. Therefore, all our products are open-source. Finally, we always have your back. If you face any problem concerning either your shopping experience or your electronics project, our team will help you deal with it, offering efficient customer service and cost-free technical support anytime. Some of those might be useful for you:

- [Web Store](https://www.soldered.com/shop)
- [Tutorials & Projects](https://soldered.com/learn)
- [Community & Technical support](https://soldered.com/community)

### Original source

​
This library is possible thanks to original [arduino-mcp23017](https://github.com/blemasle/arduino-mcp23017) library. Thank you, blemasle.

### Open-source license

Soldered invests vast amounts of time into hardware & software for these products, which are all open-source. Please support future development by buying one of our products.

Check license details in the LICENSE file. Long story short, use these open-source files for any purpose you want to, as long as you apply the same open-source licence to it and disclose the original source. No warranty - all designs in this repository are distributed in the hope that they will be useful, but without any warranty. They are provided "AS IS", therefore without warranty of any kind, either expressed or implied. The entire quality and performance of what you do with the contents of this repository are your responsibility. In no event, Soldered (TAVU) will be liable for your damages, losses, including any general, special, incidental or consequential damage arising out of the use or inability to use the contents of this repository.

## Have fun!

And thank you from your fellow makers at Soldered Electronics.
