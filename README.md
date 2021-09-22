# BlueNRG build for Mbed

1. Get GCC ARM 7
1. Configure:

    ```
    $ mbed config GCC_ARM_PATH /Users/janjongboom/toolchains/gcc-arm-none-eabi-7-2018-q2-update/bin/
    $ mbed target STEVAL_IDB008V2
    ```

1. Build:

    ```
    $ mbed compile --profile=develop.json
    ```