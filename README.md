# Running an impulse on your BlueTile

This example runs an impulse on the [STEVAL-BCN002V1](https://www.st.com/en/evaluation-tools/steval-bcn002v1b.html) multi-sensor board, classifying data in real-time from the LSM6DSO IMU. An example Edge Impulse project can be found here: https://studio.edgeimpulse.com/public/48988/latest - you can clone this project and use the [data forwarder](https://docs.edgeimpulse.com/docs/cli-data-forwarder) to add new data, or train new gestures.

* Continuous Motion Recognition - Gestures - ST BlueTile Low Memory Edge Impulse project: https://studio.edgeimpulse.com/public/48988/latest
* Fan Monitoring - Advanced Anomaly Detection - ST BlueTile Low Memory Edge Impulse project: https://studio.edgeimpulse.com/public/111797/latest

> **Note: This is an example, and not a fully supported development board.**

## Update/replace the existing Edge Impulse library

**Note:** This inference test repo only utilizes the accelerometer (LSM6DSO) sensor onboard the STEVAL-BCN002V1, to create an inference application using one of the other sensors, you will need to add the driver code from the STSW-BLUETILE-DK SDK into the mbed-os target folder and update the sensor init code in main.cpp.

To update the existing Edge Impulse continuous motion recognition library in this repository with a new accelerometer Edge Impulse C++ library deployment, follow these instructions:

1. Delete the following directories: `edge-impulse-sdk`, `model-parameters`, `tflite-model`
1. Extract the .zip file of your new Edge Impulse C++ library and copy/paste the following directories into the root of this project: `edge-impulse-sdk`, `model-parameters`, `tflite-model`
1. Change the following code in `edge-impulse-sdk/porting/mbed/ei_classifier_porting.cpp` from:
    ```cpp
    /**
     * Cancelable sleep, can be triggered with signal from other thread
     */
    EI_WEAK_FN EI_IMPULSE_ERROR ei_sleep(int32_t time_ms) {
    #if MBED_VERSION >= MBED_ENCODE_VERSION((5), (11), (0))
        rtos::ThisThread::sleep_for(time_ms);
    ```
    to...
    ```cpp
    /**
     * Cancelable sleep, can be triggered with signal from other thread
     */
    EI_WEAK_FN EI_IMPULSE_ERROR ei_sleep(int32_t time_ms) {
    #if MBED_VERSION >= MBED_ENCODE_VERSION((5), (11), (0))
        wait_ms(time_ms);
    ```

Now you are ready to build and flash your new accelerometer Edge Impulse machine learning application to the STEVAL-BCN002V1!

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
1. Open the flash application, and flash `./BUILD/STEVAL_IDB008V2/GCC_ARM-DEVELOP/bluenrg-inference-test.bin` to address `0x10040000` and check "Entire Memory".
1. Open a serial terminal to the device on baud rate `9600` to see the inference results.

## Attribution

This work is based on an Mbed OS port for the BlueTile by [ntoni92](https://github.com/ntoni92/mbed-os-BlueNRG2).
