#!/usr/local/bin/bash
avr-objdump -S -l -C -h -t ./.pio/build/nanoatmega328/firmware.elf > firmware.lst

