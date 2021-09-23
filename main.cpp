#include "mbed.h"
#include "ei_run_classifier.h"
#include "numpy.hpp"
#include "LSM6DSO.h"

/**
 * @brief Structure containing acceleration value of each axis.
 */
/*
typedef struct {
	int32_t AXIS_X;
	int32_t AXIS_Y;
	int32_t AXIS_Z;
} AxesRaw_t;
*/

static axis3bit16_t data_raw_acceleration;
//static axis3bit16_t data_raw_angular_rate;
//static float acceleration_mg[3];
//static float angular_rate_mdps[3];
//AxesRaw_t acc_data, gyro_data, mag_data, mag_offset;

// Allocate a buffer here for the values we'll read from the IMU
int16_t buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };

#define CONVERT_G_TO_MS2    9.80665f

int get_accelerometer_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t ix = 0; ix < length; ix++) {
        int16_t v = buffer[offset + ix];
        float v2 = (float) LSM6DSO_FROM_FS_2g_TO_mg(v);
        out_ptr[ix] = v2;
    }
    return 0;
}

void my_wait_us(int us)
{
    const ticker_data_t *const ticker = get_us_ticker_data();
    uint32_t start = ticker_read(ticker);
    while ((ticker_read(ticker) - start) < (uint32_t)us);
}

void Init_Accelerometer_Gyroscope(void) {
	uint8_t rst;

	lsm6dso_i3c_disable_set(0, LSM6DSO_I3C_DISABLE);

	rst = lsm6dso_reset_set(0, PROPERTY_ENABLE);
	do {
		lsm6dso_reset_get(0, &rst);
	} while (rst);

	lsm6dso_pin_mode_set(0, LSM6DSO_PUSH_PULL);
	lsm6dso_pin_polarity_set(0, LSM6DSO_ACTIVE_LOW);
	lsm6dso_all_on_int1_set(0, PROPERTY_ENABLE);
	lsm6dso_int_notification_set(0, LSM6DSO_ALL_INT_LATCHED);

	lsm6dso_block_data_update_set(0, PROPERTY_ENABLE);
	lsm6dso_xl_power_mode_set(0, LSM6DSO_LOW_NORMAL_POWER_MD);
	lsm6dso_gy_power_mode_set(0, LSM6DSO_GY_NORMAL);
	lsm6dso_xl_data_rate_set(0, LSM6DSO_XL_ODR_52Hz);
	lsm6dso_gy_data_rate_set(0, LSM6DSO_GY_ODR_52Hz);
	lsm6dso_xl_full_scale_set(0, LSM6DSO_2g);
	lsm6dso_gy_full_scale_set(0, LSM6DSO_2000dps);

	lsm6dso_auto_increment_set(0, PROPERTY_ENABLE);
}

void getAcceleration() {
    memset(data_raw_acceleration.u8bit, 0x00, 3 * sizeof(int16_t));
    for (int i = 0; i < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; i += 3) {
        lsm6dso_acceleration_raw_get(0, data_raw_acceleration.u8bit);
        // Add accelerometer data to buffer
        buffer[i + 0] = (data_raw_acceleration.i16bit[0]); // LSM6DSO_FROM_FS_2g_TO_mg(...
        buffer[i + 1] = (data_raw_acceleration.i16bit[1]);
        buffer[i + 2] = (data_raw_acceleration.i16bit[2]);

        //printf("%f, %f, %f\n", buffer[i + 0], buffer[i + 1], buffer[i + 2], "\n");

        // Gyroscope data
        /*memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));
        lsm6dso_angular_rate_raw_get(0, data_raw_angular_rate.u8bit);
        angular_rate_mdps[0] = LSM6DSO_FROM_FS_2000dps_TO_mdps(data_raw_angular_rate.i16bit[0]);
        angular_rate_mdps[1] = LSM6DSO_FROM_FS_2000dps_TO_mdps(data_raw_angular_rate.i16bit[1]);
        angular_rate_mdps[2] = LSM6DSO_FROM_FS_2000dps_TO_mdps(data_raw_angular_rate.i16bit[2]);
        gyro_data.AXIS_X = (int32_t) angular_rate_mdps[0];
        gyro_data.AXIS_Y = (int32_t) angular_rate_mdps[1];
        gyro_data.AXIS_Z = (int32_t) angular_rate_mdps[2];*/

        my_wait_us(EI_CLASSIFIER_INTERVAL_MS * 1000);
    }
}

int main() {
    printf("Edge Impulse motion inferencing (Mbed)\n");

    printf("[INIT] I2C...\n");
    SdkEvalI2CInit(400000);

    printf("[INIT] Accelerometer/gyroscope...\n");
    Init_Accelerometer_Gyroscope();

    printf("[INIT] Low-power mode...\n");
    lsm6dso_xl_data_rate_set(0, LSM6DSO_XL_ODR_OFF);
    lsm6dso_gy_data_rate_set(0, LSM6DSO_GY_ODR_OFF);

    ei_impulse_result_t result = { 0 };

    printf("[INIT] Success.\n");

    while (1) {
        printf("Sampling...\n");

        getAcceleration();

        // the features are stored into flash, and we don't want to load everything into RAM
        signal_t signal;
        signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
        signal.get_data = &get_accelerometer_data;

        // invoke the impulse
        EI_IMPULSE_ERROR res = run_classifier(&signal, &result, true);
        printf("run_classifier returned: %d\n", res);

        if (res != 0) return 1;

        printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
            result.timing.dsp, result.timing.classification, result.timing.anomaly);

        // print the predictions
        printf("[");
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            printf("%.5f", result.classification[ix].value);
#if EI_CLASSIFIER_HAS_ANOMALY == 1
            printf(", ");
#else
            if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
                printf(", ");
            }
#endif
        }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        printf("%.3f", result.anomaly);
#endif
        printf("]\n");

        wait_ms(2000);
    }
}
