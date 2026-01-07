#ifndef OUR_DRIVERS_OUR_BME680_H_
#define OUR_DRIVERS_OUR_BME680_H_

#include <zephyr/types.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/i2c.h>

// This is the definition for compatible DTS nodes.
// It links the device tree compatible string to this driver implementation.
// It tells Zephyr: "Find every node in the .dts file that has compatible = "my,bme680" and create a device
// instance for it using this driver."
#define DT_DRV_COMPAT our_bme680

/* This block handles the C++ compatibility */
#ifdef __cplusplus
extern "C" {
#endif
#define BME680_BUS_SPI DT_ANY_INST_ON_BUS_STATUS_OKAY(spi)
#define BME680_BUS_I2C DT_ANY_INST_ON_BUS_STATUS_OKAY(i2c)

union bme680_bus {
#if BME680_BUS_SPI
	struct spi_dt_spec spi;
#endif
#if BME680_BUS_I2C
	struct i2c_dt_spec i2c;
#endif
};
/* Private Data Structures */
typedef int (*bme680_bus_check_fn)(const union bme680_bus *bus);
typedef int (*bme680_reg_read_fn)(const struct device *dev, uint8_t start, uint8_t *buf, int size);
typedef int (*bme680_reg_write_fn)(const struct device *dev, uint8_t reg, uint8_t val);

struct bme680_bus_io {
    bme680_bus_check_fn check;
    bme680_reg_read_fn read;
    bme680_reg_write_fn write;
};

#if BME680_BUS_SPI
#define BME680_SPI_OPERATION (SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL \
		| SPI_MODE_CPHA | SPI_OP_MODE_MASTER)
extern const struct bme680_bus_io bme680_bus_io_spi;
#endif

#if BME680_BUS_I2C
extern const struct bme680_bus_io bme680_bus_io_i2c;
#endif

struct our_bme680_config {
	union bme680_bus bus;
    const struct bme680_bus_io *bus_io;
};

struct our_bme680_data_regs {
    uint8_t pressure[3];
    uint8_t temperature[3];
    uint8_t humidity[2];
    uint8_t padding[3];
    uint8_t gas[2];
} __packed;

struct our_bme680_data {
    /* Compensation parameters. */
    uint16_t par_h1;
    uint16_t par_h2;
    int8_t par_h3;
    int8_t par_h4;
    int8_t par_h5;
    uint8_t par_h6;
    int8_t par_h7;
    int8_t par_gh1;
    int16_t par_gh2;
    int8_t par_gh3;
    uint16_t par_t1;
    int16_t par_t2;
    int8_t par_t3;
    uint16_t par_p1;
    int16_t par_p2;
    int8_t par_p3;
    int16_t par_p4;
    int16_t par_p5;
    int8_t par_p6;
    int8_t par_p7;
    int16_t par_p8;
    int16_t par_p9;
    uint8_t par_p10;
    uint8_t res_heat_range;
    int8_t res_heat_val;
    int8_t range_sw_err;
    bool has_read_compensation;

    /* Calculated sensor values. */
    int32_t calc_temp;
    uint32_t calc_press;
    uint32_t calc_humidity;
    uint32_t calc_gas_resistance;

    /* Additional information */
    uint8_t heatr_stab;

    /* Carryover between temperature and pressure/humidity compensation. */
    int32_t t_fine;

    uint8_t chip_id;
#if BME680_BUS_SPI
	uint8_t mem_page;
#endif
};

#define BME680_CHIP_ID           0x61

#define BME680_LEN_COEFF_ALL     42
#define BME680_LEN_COEFF1        23
#define BME680_LEN_COEFF2        14
#define BME680_LEN_COEFF3        5

#define BME680_REG_COEFF3        0x00
#define BME680_REG_MEAS_STATUS   0x1D
#define BME680_REG_FIELD0        0x1F
#define BME680_REG_IDAC_HEAT0    0x50
#define BME680_REG_RES_HEAT0     0x5A
#define BME680_REG_GAS_WAIT0     0x64
#define BME680_REG_SHD_HEATR_DUR 0x6E
#define BME680_REG_CTRL_GAS_0    0x70
#define BME680_REG_CTRL_GAS_1    0x71
#define BME680_REG_CTRL_HUM      0x72
#define BME680_REG_STATUS        0x73
#define BME680_REG_CTRL_MEAS     0x74
#define BME680_REG_CONFIG        0x75
#define BME680_REG_UNIQUE_ID     0x83
#define BME680_REG_COEFF1        0x8a
#define BME680_REG_COEFF2        0xe1
#define BME680_REG_CHIP_ID       0xd0
#define BME680_REG_SOFT_RESET    0xe0

#define BME680_MSK_NEW_DATA      0x80
#define BME680_MSK_GAS_RANGE     0x0f
#define BME680_MSK_RH_RANGE      0x30
#define BME680_MSK_RANGE_SW_ERR  0xf0
#define BME680_MSK_HEATR_STAB    0x10

#define BME680_SPI_MEM_PAGE_MSK  0x10
#define BME680_SPI_MEM_PAGE_POS  4
#define BME680_SPI_READ_BIT      0x80
#define BME680_SPI_WRITE_MSK     0x7f

#if defined CONFIG_OUR_BME680_TEMP_OVER_1X
#define BME680_TEMP_OVER (1 << 5)
#elif defined CONFIG_OUR_BME680_TEMP_OVER_2X
#define BME680_TEMP_OVER (2 << 5)
#elif defined CONFIG_OUR_BME680_TEMP_OVER_4X
#define BME680_TEMP_OVER (3 << 5)
#elif defined CONFIG_OUR_BME680_TEMP_OVER_8X
#define BME680_TEMP_OVER (4 << 5)
#elif defined CONFIG_OUR_BME680_TEMP_OVER_16X
#define BME680_TEMP_OVER (5 << 5)
#endif

#if defined CONFIG_OUR_BME680_PRESS_OVER_1X
#define BME680_PRESS_OVER (1 << 2)
#elif defined CONFIG_OUR_BME680_PRESS_OVER_2X
#define BME680_PRESS_OVER (2 << 2)
#elif defined CONFIG_OUR_BME680_PRESS_OVER_4X
#define BME680_PRESS_OVER (3 << 2)
#elif defined CONFIG_OUR_BME680_PRESS_OVER_8X
#define BME680_PRESS_OVER (4 << 2)
#elif defined CONFIG_OUR_BME680_PRESS_OVER_16X
#define BME680_PRESS_OVER (5 << 2)
#endif

#if defined CONFIG_OUR_BME680_HUMIDITY_OVER_1X
#define BME680_HUMIDITY_OVER 1
#elif defined CONFIG_OUR_BME680_HUMIDITY_OVER_2X
#define BME680_HUMIDITY_OVER 2
#elif defined CONFIG_OUR_BME680_HUMIDITY_OVER_4X
#define BME680_HUMIDITY_OVER 3
#elif defined CONFIG_OUR_BME680_HUMIDITY_OVER_8X
#define BME680_HUMIDITY_OVER 4
#elif defined CONFIG_OUR_BME680_HUMIDITY_OVER_16X
#define BME680_HUMIDITY_OVER 5
#endif

#if defined CONFIG_OUR_BME680_HEATR_TEMP_LP
#define BME680_HEATR_TEMP 320
#elif defined CONFIG_OUR_BME680_HEATR_TEMP_ULP
#define BME680_HEATR_TEMP 400
#endif

#if defined CONFIG_OUR_BME680_HEATR_DUR_LP
#define BME680_HEATR_DUR_MS 197
#elif defined CONFIG_OUR_BME680_HEATR_DUR_ULP
#define BME680_HEATR_DUR_MS 1943
#endif

#if defined CONFIG_OUR_BME680_FILTER_OFF
#define BME680_FILTER 0
#elif defined CONFIG_OUR_BME680_FILTER_2
#define BME680_FILTER (1 << 2)
#elif defined CONFIG_OUR_BME680_FILTER_4
#define BME680_FILTER (2 << 2)
#elif defined CONFIG_OUR_BME680_FILTER_8
#define BME680_FILTER (3 << 2)
#elif defined CONFIG_OUR_BME680_FILTER_16
#define BME680_FILTER (4 << 2)
#elif defined CONFIG_OUR_BME680_FILTER_32
#define BME680_FILTER (5 << 2)
#elif defined CONFIG_OUR_BME680_FILTER_64
#define BME680_FILTER (6 << 2)
#elif defined CONFIG_OUR_BME680_FILTER_128
#define BME680_FILTER (7 << 2)
#endif

#define BME680_MODE_SLEEP             0
#define BME680_MODE_FORCED            1

#define BME680_CTRL_MEAS_VAL          (BME680_PRESS_OVER | BME680_TEMP_OVER | BME680_MODE_FORCED)
#define BME680_CONFIG_VAL             BME680_FILTER
#define BME680_CTRL_GAS_1_VAL         0x10

#define BME680_CONCAT_BYTES(msb, lsb) (((uint16_t)msb << 8) | (uint16_t)lsb)

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
int our_bme680_run_gas_heater(const struct device *dev, uint16_t temp_c, uint16_t duration_ms);

/**
 * @brief Get the raw chip ID (useful for diagnostics).
 * * @param dev Pointer to the BME680 device structure.
 * @param chip_id Output pointer for the ID.
 * @return 0 on success.
 */
int our_bme680_get_chip_id(const struct device *dev, uint8_t *chip_id);

#ifdef __cplusplus
}
#endif

#endif /* OUR_DRIVERS_OUR_BME680_H_ */