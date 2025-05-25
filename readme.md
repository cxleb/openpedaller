# openpedaller

A bike computer.

Goal is to use off the shelf hardware and write some custom firmware to create a cheap, powerful and extensible bike computer.

### Hardware 
- Waveshare esp32-s3-touch-amoled-2.41
- u-blox neo 7m gps module (to be changed to probably 9 or 10)

Connect GPS module UART out to UART 0 on the esp32-s3 (on the waveshare module this is the exposed UART connector)

### Firmware

Make sure you git clone --recursive to get submodules.

after enabling the idf environment

`idf.py set-target esp32s3`
`idf.py build flash monitor`