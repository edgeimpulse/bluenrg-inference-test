# BlueNRG-Tile Build for Mbed

Board: https://www.st.com/en/evaluation-tools/steval-bcn002v1b.html

Edge Impulse Project: https://studio.edgeimpulse.com/public/48988/latest

1. Get GCC ARM 7 & Mbed CLI
1. Configure:
    ```
    $ mbed config -G GCC_ARM_PATH /Users/janjongboom/toolchains/gcc-arm-none-eabi-7-2018-q2-update/bin/
    ```
1. Build:
    ```
    $ mbed compile -t GCC_ARM -m STEVAL_IDB008V2 --profile=develop.json
    ```
1. Flash/verify built binary with [STSW-BNRGFLASHER](https://www.st.com/en/embedded-software/stsw-bnrgflasher.html) on Windows from address `0x10040000`
    - Note: verify will fail, but it doesn't seem to matter
1. See printf output on `9600` baud rate from serial terminal