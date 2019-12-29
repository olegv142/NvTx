#include "crc16.h"

uint16_t crc16_up_(uint16_t crc, uint8_t up)
{
	for (uint8_t i = 8; i; --i) {
		uint8_t mix = ((uint8_t)crc ^ up) & 1;
		crc >>= 1;
		if (mix) crc ^= 0xa001;
		up >>= 1;
	}
	return crc;
}

uint16_t crc16_up(uint16_t crc, const void* data, unsigned len)
{
	const uint8_t* ptr = data;
	for (; len; --len, ++ptr)
		crc = crc16_up_(crc, *ptr);
	return crc;
}

uint16_t crc16_up_str(uint16_t crc, const char* str)
{
	for (; *str; ++str)
		crc = crc16_up_(crc, *str);
	return crc;
}
