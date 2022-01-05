#include "bma220.h"

#define BMA220_ADDR 0x0A

#define CHIPID_REG 0x00
#define SOFTRESET_REG 0x32

static int read_reg(struct bma220_dev *dev, uint8_t reg, uint8_t *value)
{
	if (dev->write(BMA220_ADDR, &reg, 1))
		return -1;

	if (dev->read(BMA220_ADDR, value, 1))
		return -1;

	return 0;
}

int bma220_init(struct bma220_dev *dev)
{
	if (!dev->write || !dev->read)
		return BMA220_NULL_FUNC;

	uint8_t reset_value;
	if (read_reg(dev, SOFTRESET_REG, &reset_value))
		return BMA220_COMMUNICATION;

	if (reset_value == 0xFF)
		if (read_reg(dev, SOFTRESET_REG, &reset_value))
			return BMA220_COMMUNICATION;

	if (reset_value != 0x00)
		return BMA220_INVALID_MODE;

	if (read_reg(dev, SOFTRESET_REG, &reset_value))
		return BMA220_COMMUNICATION;

	if (reset_value != 0xFF)
		return BMA220_INVALID_MODE;

	return 0;
}

int bma220_chip_id(struct bma220_dev *dev, uint8_t *chip_id)
{
	return read_reg(dev, CHIPID_REG, chip_id);
}
