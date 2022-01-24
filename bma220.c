#include "bma220.h"

#define BMA220_ADDR 0x0A

#define REG_CHIPID 0x00
#define REG_SOFTRESET 0x32
#define REG_AXIS_X 0x04
#define REG_AXIS_Y 0x06
#define REG_AXIS_Z 0x08
#define REG_RANGE 0x22

static int read_reg(struct bma220_dev *dev, uint8_t reg, uint8_t *value) {
  if (dev->write(BMA220_ADDR, &reg, 1))
    return -1;

  if (dev->read(BMA220_ADDR, value, 1))
    return -1;

  return 0;
}

static int write_reg(struct bma220_dev *dev, uint8_t reg, uint8_t value) {
  uint8_t data[2] = {reg, value};

  if (dev->write(BMA220_ADDR, data, 2))
    return -1;

  return 0;
}

int bma220_init(struct bma220_dev *dev) {
  if (!dev->write || !dev->read)
    return BMA220_NULL_FUNC;

  uint8_t reset_value;
  if (read_reg(dev, REG_SOFTRESET, &reset_value))
    return BMA220_COMMUNICATION;

  if (reset_value == 0xFF)
    if (read_reg(dev, REG_SOFTRESET, &reset_value))
      return BMA220_COMMUNICATION;

  if (reset_value != 0x00)
    return BMA220_INVALID_MODE;

  if (read_reg(dev, REG_SOFTRESET, &reset_value))
    return BMA220_COMMUNICATION;

  if (reset_value != 0xFF)
    return BMA220_INVALID_MODE;

  return 0;
}

int bma220_chip_id(struct bma220_dev *dev, uint8_t *chip_id) {
  return read_reg(dev, REG_CHIPID, chip_id);
}

int bma220_set_range_mode(struct bma220_dev *dev, uint8_t mode) {
  return write_reg(dev, REG_RANGE, mode);
}

int bma220_get_range_mode(struct bma220_dev *dev, uint8_t *mode) {
  int res = read_reg(dev, REG_RANGE, mode);

  return res;
}

static float low_level_conversion(int8_t value, uint8_t range_mode) {
  // We now need to scale the byte value to its actual floating point value.
  // The scale is equal to the following: 32 / (2 ^ (range_mode + 1)).
  // For example, RANGE_2G is equal to 0, so range_mode + 1 equals 1.
  // 32 / 2 ^ 1 => 32 / 2 => 16 which is the proper scale to get a range of
  // 64 values between +2 and -2g
  // Likewise, RANGE_2G = 3, so range_mode + 1 equals 4
  // 32 / 2 ^ 4 => 32 / 16 => 2 which corresponds to value between +16 and -16g

  uint32_t scale = (1 << (range_mode + 1));
  scale = 32.0 / scale;

  return ((float)value) / ((float)scale);
}

static int convert_value(struct bma220_dev *dev, int8_t value, float *result) {
  uint8_t range_mode;
  int ret = bma220_get_range_mode(dev, &range_mode);
  if (ret)
    return ret;

  *result = low_level_conversion(value, range_mode);

  return 0;
}

static int8_t convert_acc_register(uint8_t value) { return (int8_t)value / 4; }

int bma220_get_acceleration(struct bma220_dev *dev,
                            struct bma220_acceleration_data *data) {
  uint8_t raw_x, raw_y, raw_z;
  int8_t x, y, z;

  if (read_reg(dev, REG_AXIS_X, &raw_x))
    return BMA220_COMMUNICATION;

  if (read_reg(dev, REG_AXIS_Y, &raw_y))
    return BMA220_COMMUNICATION;

  if (read_reg(dev, REG_AXIS_Z, &raw_z))
    return BMA220_COMMUNICATION;

  x = convert_acc_register(raw_x);
  y = convert_acc_register(raw_y);
  z = convert_acc_register(raw_z);

  convert_value(dev, x, &data->x);
  convert_value(dev, y, &data->y);
  convert_value(dev, z, &data->z);

  return 0;
}
