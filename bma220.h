#ifndef BMA220_H
#define BMA220_H

#include <stdint.h>

/* Range values for bma220_set_range_mode() */
#define RANGE_2G 0
#define RANGE_4G 1
#define RANGE_8G 2
#define RANGE_16G 3

#define BMA220_CHIP_ID 0xDD

/**
 * Write function for the bma220_dev write field
 *
 * The given device address corresponds to the *raw* address of the device. In
 * some cases such as on the STM32, it is necessary for the write function to
 * shift the adresse before calling the low level write function.
 *
 * For example, HAL_I2C_Master_Transmit(i2c_handle, dev_addr << 1, data, len);
 */
typedef int (*bma220_write_f)(uint8_t dev_addr, const uint8_t *data,
                              uint32_t len);

/**
 * Read function for the bma220_dev read field
 *
 * The given device address corresponds to the *raw* address of the device. In
 * some cases such as on the STM32, it is necessary for the write function to
 * shift the adresse before calling the low level write function.
 *
 * For example, HAL_I2C_Master_Receive(i2c_handle, dev_addr << 1, data, len);
 */
typedef int (*bma220_read_f)(uint8_t dev_addr, uint8_t *data, uint32_t len);

/**
 * BMA220 handler structure
 */
struct bma220_dev {
  /* Function to use to read from the device */
  bma220_read_f read;
  /* Function to use to write to the device */
  bma220_write_f write;
};

/**
 * Possible return values from BMA220 functions including error codes
 */
enum bma220_retval {
  BMA220_OK = 0,
  BMA220_NULL_FUNC,
  BMA220_COMMUNICATION,
  BMA220_INVALID_MODE,
};

/**
 * Initialize a given device
 *
 * @warn The device should already have write and read functions defined
 *
 * @dev Pointer to the device to initialize
 *
 * @return 0 on success, an error code otherwise
 */
int bma220_init(struct bma220_dev *dev);

/**
 * Set the range mode to use before fetching acceleration values.
 * By default, the device is in 2g range mode.
 *
 * @dev Pointer to the device to set
 * @mode Value to set. The value *must* be one of RANGE_2G, RANGE_4G, RANGE_8G
 * or RANGE_16G
 *
 * @return 0 on success, an error code otherwise
 */
int bma220_set_range_mode(struct bma220_dev *dev, uint8_t mode);

/**
 * Get the range mode currently set in the device
 *
 * @dev Pointer to the device to read from
 * @mode Pointer to the mode to set
 *
 * @return 0 on success, an error code otherwise
 */
int bma220_get_range_mode(struct bma220_dev *dev, uint8_t *mode);

/**
 * Acceleration data returned by the bma220_acceleration() function.
 *
 * This structure wraps the three axes that the sensor can handle
 */
struct bma220_acceleration_data {
  float x;
  float y;
  float z;
};

/**
 * Fetch the acceleration value from a device
 *
 * @dev Pointer of the device to read
 * @data Pointer in which to store the acceleration result
 *
 * @return 0 on success, an error code otherwise
 */
int bma220_get_acceleration(struct bma220_dev *dev,
                            struct bma220_acceleration_data *data);

/**
 * Fetch the chip ID of a device
 *
 * @dev Pointer of the device to read
 * @chip_id Pointer in which to store the result
 *
 * @return 0 on success, an error code otherwise
 */
int bma220_chip_id(struct bma220_dev *dev, uint8_t *chip_id);

#endif /* !BMA220_H */
