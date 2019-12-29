#pragma once

#include <Arduino.h>

//
// CRC16-ANSI implementation
//

#define CRC16_INIT 0

uint16_t crc16_up_(uint16_t crc, uint8_t up);
uint16_t crc16_up(uint16_t crc, const void* data, unsigned len);
uint16_t crc16_up_str(uint16_t crc, const char* str);

static inline uint16_t crc16(const void* data, unsigned len)
{
	return crc16_up(CRC16_INIT, data, len);
}

static inline uint16_t crc16_str(const char* str)
{
	return crc16_up_str(CRC16_INIT, str);
}

