// Zephyr modules
#include <inttypes.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/sys/util.h>
// #include <modem/lte_lc.h>
// App modules
#include "services/settings_storage.h"
#include "services/system_manager.h"

#define DEBUGGER_ATTACH 0

LOG_MODULE_REGISTER(main_app, LOG_LEVEL_INF);

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS_OKAY(SW0_NODE)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
volatile int buttonPushed            = 0;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
    printk("Button pushed");
    buttonPushed++;
}

int main(void) {
#if DEBUGGER_ATTACH
    volatile int attach_debugger = 1;
    while (attach_debugger) {
        // Spin here forever until YOU change 'attach_debugger' to 0
        k_sleep(K_MSEC(100));
    }
#endif

    LOG_INF("BabbiesTracker application started. Like a charm!\n");

    if (!gpio_is_ready_dt(&led)) {
        LOG_ERR("Error: LED device %s is not ready\n", led.port->name);
        return 0;
    }
    int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Error %d: failed to configure %s pin %d\n", ret, led.port->name, led.pin);
        return 0;
    }

    if (!gpio_is_ready_dt(&button)) {
        LOG_ERR("Error: button device %s is not ready\n", button.port->name);
        return 0;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0) {
        LOG_ERR("Error %d: failed to configure %s pin %d\n", ret, button.port->name, button.pin);
        return 0;
    }

    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        LOG_ERR("Error %d: failed to configure interrupt on %s pin %d\n", ret, button.port->name, button.pin);
        return 0;
    }

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
    LOG_INF("Set up button at %s pin %d\n", button.port->name, button.pin);

    // Fetch the BME680 Sensor
    // The device name "BME680" must match your devicetree label/node
    // const struct device *dev = DEVICE_DT_GET_ANY(bosch_bme680);

    // if (!device_is_ready(dev)) {
    //     LOG_ERR("Sensor BME680 not ready!\n");
    //     return 0;
    // }

    // int err;
    // Initialize the LTE Modem (Required for nRF9160 system stability)
    // This boots the modem core, even if we don't connect to a tower yet.
    // LOG_INF("Initializing LTE modem...\n");
    // err = lte_lc_init();
    // if (err) {
    //     LOG_ERR("Failed to init LTE modem: %d\n", err);
    // }
    // LOG_INF("LTE modem initialized.\n");

    Services::SystemManager &system = Services::SystemManager::getInstance();
    system.init();

    Services::SettingsStorage &settings = Services::SettingsStorage::getInstance();
    
    if (ret != 0) {
        LOG_ERR("Failed to initialize Settings Storage: %d", ret);
        return ret;
    }

    while (1) {
        ret = gpio_pin_toggle_dt(&led);
        // Fetch fresh data
        // sensor_sample_fetch(dev);

        // struct sensor_value temp, press, humidity, gas;
        // sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
        // sensor_channel_get(dev, SENSOR_CHAN_PRESS, &press);
        // sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &humidity);
        // sensor_channel_get(dev, SENSOR_CHAN_GAS_RES, &gas);

        // LOG_INF("BME680 readings\n\tT: %d.%06d; P: %d.%06d; H: %d.%06d; G: %d.%06d",
        //         temp.val1, temp.val2, press.val1, press.val2,
        //         humidity.val1, humidity.val2, gas.val1, gas.val2);
        LOG_INF("LED toggled.");

        switch (buttonPushed) {
        case 0:
            // No button push
            break;
        case 1:
            LOG_INF("Button pushed once.");
            if (!settings.isInitialized())
                settings.init();
            break;
        case 2:
            LOG_INF("Button pushed twice.");
            settings.SetKey(Services::SettingsStorage::KEY_CELL_APN, (void *)"my_apn_mew",
                            strlen("my_apn_mew") + 1);
            settings.SetKey(Services::SettingsStorage::KEY_CELL_PASS, (void *)"my_pass_mold",
                            strlen("my_pass_mold") + 1);
            LOG_INF("System rebooting now...");
            k_sleep(K_SECONDS(3));
            sys_reboot(SYS_REBOOT_COLD);
            break;
        default:
            LOG_INF("Button pushed %d times.", buttonPushed);
            break;
        }

        k_sleep(K_SECONDS(2));
    }
    return 0;
}