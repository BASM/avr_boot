avr_boot
========

Bootloader for AVR devices.

Bootloader based on Atmel documents:
* 109 -- baseline
* 230 -- DES bootloader
* 231 -- AES bootloader
* 911 -- OSP
* 947 -- single vare

Directories:
* firmware -- Boot loader for AVR devices
* hello    -- Hello world application for AVR bootloader testing
* write    -- PC application, send firmware (example hello.hex) to AVR bootloader.

Building:
* FIXME

Stage: begin.

I wrote this loader, because I needed a way of programming with one-way bond (only from PC to MK).
When the device can not confirm the success of obtaining data, stop/pause the flow.

I have plan to programming it to MK what have only IR transiver. IR transiver is one of the
easiest and cheapest ways to wireless. Only one device, example TSOP348, only one pin in MK.

About IR channel on AVR see project: https://github.com/BASM/avr_libinfrared


Goals:
* Simple firmware -- OK
* Simple PC programmator -- OK
* Simple hello world program for testring non boot application -- OK
* Support UART -- OK
* Adding CRC and FEC -- NO
* Protocol for switching to boot mode from application mode -- NO
* Support IR channel -- NO

