// This is the definition for compatible DTS nodes.
// It links the device tree compatible string to this driver implementation.
// It tells Zephyr: "Find every node in the .dts file that has compatible = "my,bme680" and create a device
// instance for it using this driver."
#define DT_DRV_COMPAT my_bme680

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/* This include path comes from the module's 'include' directory */
#include "my_drivers/my_bme680.h"

LOG_MODULE_REGISTER(my_bme680, LOG_LEVEL_INF);

/* BME680 Registers (Simplified subset for demo) */
#define BME680_REG_CHIP_ID   0xD0
#define BME680_CHIP_ID_VAL   0x61
#define BME680_REG_RESET     0xE0
#define BME680_REG_CTRL_GAS  0x71
#define BME680_REG_CTRL_HUM  0x72
#define BME680_REG_CTRL_MEAS 0x74
#define BME680_REG_CONFIG    0x75

/* Private Data Structures */
struct my_bme680_config {
    struct i2c_dt_spec i2c;
};

struct my_bme680_data {
    uint8_t chip_id;
    int32_t temp_centi_c;
    /* Add other measurement storage here */
};

/* --- Internal Helpers --- */

static int bme680_reg_read(const struct device *dev, uint8_t reg, uint8_t *val) {
    const struct my_bme680_config *cfg = dev->config;
    return i2c_write_read_dt(&cfg->i2c, &reg, 1, val, 1);
}

static int bme680_reg_write(const struct device *dev, uint8_t reg, uint8_t val) {
    const struct my_bme680_config *cfg = dev->config;
    uint8_t tx_buf[2]                  = {reg, val};
    return i2c_write_dt(&cfg->i2c, tx_buf, sizeof(tx_buf));
}

/* --- Standard Zephyr Sensor API Implementation --- */

static int my_bme680_sample_fetch(const struct device *dev, enum sensor_channel chan) {
    struct my_bme680_data *data = dev->data;

    /* * In a real driver, this would read the temperature registers
     * (0x22-0x24) and compensate using the Bosch BME680 math library.
     * For this structural demo, we simulate a reading or read raw registers.
     */

    // Example: Reading raw temp registers (simplified)
    // uint8_t raw_temp[3];
    // i2c_burst_read_dt(&cfg->i2c, 0x22, raw_temp, 3);

    // Mocking data for architecture demonstration
    data->temp_centi_c = 2550; // 25.50 C

    return 0;
}

static int my_bme680_channel_get(const struct device *dev, enum sensor_channel chan,
                                 struct sensor_value *val) {
    struct my_bme680_data *data = dev->data;

    if (chan == SENSOR_CHAN_AMBIENT_TEMP) {
        val->val1 = data->temp_centi_c / 100;
        val->val2 = (data->temp_centi_c % 100) * 10000;
        return 0;
    }

    /* Can add cases for SENSOR_CHAN_PRESS, SENSOR_CHAN_HUMIDITY, etc. */

    return -ENOTSUP;
}

/* This struct tells Zephyr which functions to call for the standard API */
static const struct sensor_driver_api my_bme680_api = {
    .sample_fetch = my_bme680_sample_fetch,
    .channel_get  = my_bme680_channel_get,
};

/* --- Custom Extension API Implementation --- */

int my_bme680_run_gas_heater(const struct device *dev, uint16_t temp_c, uint16_t duration_ms) {
    const struct my_bme680_config *cfg = dev->config;

    if (!device_is_ready(dev)) {
        return -ENODEV;
    }

    LOG_INF("Custom API: Running Heater at %dC for %dms", temp_c, duration_ms);

    /* * Here you would implement the complex logic to set:
     * 1. Gas Wait 0 register (duration)
     * 2. Res Heat 0 register (target temp calculation)
     * 3. Set CTRL_GAS_1 to select profile 0 and enable gas measurement
     * 4. Trigger Forced Mode
     */

    // Simple write example to demonstrate I2C usage:
    // This isn't the full Bosch algorithm, just the I2C mechanism
    // bme680_reg_write(dev, BME680_REG_CTRL_GAS, 0x10); // Enable Gas

    return 0;
}

int my_bme680_get_chip_id(const struct device *dev, uint8_t *chip_id) {
    struct my_bme680_data *data = dev->data;
    *chip_id                    = data->chip_id;
    return 0;
}

/* --- Initialization --- */

static int my_bme680_init(const struct device *dev) {
    const struct my_bme680_config *cfg = dev->config;
    struct my_bme680_data *data        = dev->data;

    if (!i2c_is_ready_dt(&cfg->i2c)) {
        LOG_ERR("I2C bus not ready");
        return -ENODEV;
    }

    /* 1. Reset the sensor */
    bme680_reg_write(dev, BME680_REG_RESET, BME680_CMD_RESET);
    k_sleep(K_MSEC(10)); // Wait for reset

    /* 2. Read Chip ID to verify connection */
    int ret = bme680_reg_read(dev, BME680_REG_CHIP_ID, &data->chip_id);
    if (ret != 0) {
        LOG_ERR("Failed to read Chip ID");
        return ret;
    }

    if (data->chip_id != BME680_CHIP_ID_VAL) {
        LOG_ERR("Bad Chip ID: 0x%02x (Expected 0x61)", data->chip_id);
        return -EINVAL;
    }

    LOG_INF("Init My Custom BME680. Chip ID: 0x%02x", data->chip_id);

    return 0;
}

/* --- Driver Instantiation Macro --- */
#define MY_BME680_DEFINE(inst)                                                                               \
    static struct my_bme680_data data_##inst;                                                                \
    static const struct my_bme680_config config_##inst = {                                                   \
        .i2c = I2C_DT_SPEC_INST_GET(inst),                                                                   \
    };                                                                                                       \
                                                                                                             \
    DEVICE_DT_INST_DEFINE(inst, my_bme680_init, NULL, &data_##inst, &config_##inst, POST_KERNEL,             \
                          CONFIG_SENSOR_INIT_PRIORITY, &my_bme680_api);

DT_INST_FOREACH_STATUS_OKAY(MY_BME680_DEFINE)