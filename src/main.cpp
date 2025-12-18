// App's modules
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <modem/lte_lc.h>

LOG_MODULE_REGISTER(main_app, LOG_LEVEL_INF);


int main(void) {
    LOG_INF("BabbiesTracker application started.");
    int err;
    // 1. Initialize the LTE Modem (Required for nRF9160 system stability)
    // This boots the modem core, even if we don't connect to a tower yet.
    LOG_INF("Initializing LTE modem...\n");
    err = lte_lc_init();
    if (err) {
        LOG_ERR("Failed to init LTE modem: %d\n", err);
    }
    LOG_INF("LTE modem initialized.\n");

    // 2. Fetch the BME680 Sensor
    // The device name "BME680" must match your devicetree label/node
    const struct device *dev = DEVICE_DT_GET_ANY(bosch_bme680);

    if (!device_is_ready(dev)) {
        LOG_ERR("Sensor BME680 not ready!\n");
        return;
    }

    while (1) {
        // Fetch fresh data
        sensor_sample_fetch(dev);

        struct sensor_value temp, press, humidity, gas;
        sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
        sensor_channel_get(dev, SENSOR_CHAN_PRESS, &press);
        sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &humidity);
        sensor_channel_get(dev, SENSOR_CHAN_GAS_RES, &gas);

        LOG_INF("T: %d.%06d; P: %d.%06d; H: %d.%06d; G: %d.%06d",
                temp.val1, temp.val2, press.val1, press.val2,
                humidity.val1, humidity.val2, gas.val1, gas.val2);

        k_sleep(K_SECONDS(3));
    }
    return 0;    
}