# This block runs only if the user has set CONFIG_OUR_BME680=y in prj.conf
# or if it defaulted to y because the hardware was found in the Device Tree.

if(CONFIG_OUR_BME680)
  # Create a Zephyr library slice for this driver
  zephyr_library()
  
  # Add the implementation source file to the build
  zephyr_library_sources(our_bme680.c  bme680_i2c.c bme680_spi.c)
endif()