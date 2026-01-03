#ifndef MY_DRIVERS_MY_BME680_H_
#define MY_DRIVERS_MY_BME680_H_

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

/* * 1. Standard Zephyr Sensor API Usage (For context):
 * * struct sensor_value temp;
 * sensor_sample_fetch(dev);
 * sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
 */

/* * 2. Custom Extension API
 * These functions expose hardware-specific features that don't fit 
 * into the generic 'sensor_channel_get' model.
 */

/**
 * @brief Force the sensor to run a specific gas heater profile immediately.
 * * @param dev Pointer to the BME680 device structure.
 * @param temp_c Target temperature in Celsius.
 * @param duration_ms Duration in milliseconds.
 * @return 0 on success, negative errno on failure.
 */
int my_bme680_run_gas_heater(const struct device *dev, uint16_t temp_c, uint16_t duration_ms);

/**
 * @brief Get the raw chip ID (useful for diagnostics).
 * * @param dev Pointer to the BME680 device structure.
 * @param chip_id Output pointer for the ID.
 * @return 0 on success.
 */
int my_bme680_get_chip_id(const struct device *dev, uint8_t *chip_id);

#endif /* MY_DRIVERS_MY_BME680_H_ */