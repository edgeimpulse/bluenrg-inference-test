# Running an impulse on your BlueTile

This example runs an impulse on the [STEVAL-BCN002V1](https://www.st.com/en/evaluation-tools/steval-bcn002v1b.html) multi-sensor board, classifying data in real-time from the LSM6DSO IMU. An example Edge Impulse project (the one included in this example) can be found here: https://studio.edgeimpulse.com/public/48988/latest - you can clone this project and use the [data forwarder](https://docs.edgeimpulse.com/docs/cli-data-forwarder) to add new data, or train new gestures.

## Building and flashing this application

1. Install the [Mbed CLI](https://github.com/ARMmbed/mbed-cli).
1. Install the [GNU Arm Embedded Toolchain 7](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads#panel8a).
1. Configure the Mbed CLI, and point it to the toolchain:

    ```
    $ echo 'ROOT=.' > .mbed
    $ mbed config GCC_ARM_PATH ~/toolchains/gcc-arm-none-eabi-7-2018-q2-update/bin/
    ```

1. Build the project:

    ```
    $ mbed compile -t GCC_ARM -m STEVAL_IDB008V2 --profile=develop.json
    ```

1. Install the [STSW-BNRGFLASHER](https://www.st.com/en/embedded-software/stsw-bnrgflasher.html) (Windows only).
1. Open the flash application, and flash `./BUILD/STEVAL_IDB008V2/GCC_ARM-DEVELOP/bluenrg-inference-test.bin` to address `0x10040000`.

    > **Note:** Verify seems to always fail, but the application is flashed anyway.

1. Open a serial terminal to the device on baud rate `9600` to see the inference results.
