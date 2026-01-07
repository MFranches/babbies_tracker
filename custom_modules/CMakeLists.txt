# Export the include directory to anyone using this module
zephyr_include_directories(include)

# Add the sub-directory containing the driver code
add_subdirectory(drivers/sensor/our_bme680)