#include "NvTx.h"
#include "crc16.h"
#include <EEPROM.h>

/*
 Transactional non-volatile storage uses EEPROM for reliably storing
 client's data. It provides the following useful properties:
 - client will not read the data it have never written
 - client will not read the data corrupted for any reason
 - the data update will succeed either entirely or not at all, in the latter
   case old data will be read
 - the data is bound to the semantic tag (the string) provided by the client so
   the data originally bound to the different tag will not be written to avoid
   erroneous interpretation of the semantically different data
 
 To provide the above guarantees the data is saved in 2 copies in 2 adjacent
 storage 'cells'. Each copy is protected by 2 checksums calculated based on the
 semantic tag and the content of the data. The format of the storage is illustrated below
 where the E is epoch bit, V is the valid bit, addr is the base address, size is the data size:
 
 |E| tag chksum |V| tag chksum cont'd | data      | tag + data chksum || 2'nd cell ..
 ^                ^                   ^           ^                    ^
 | addr           | addr + 1          | addr + 2  | addr + 2 + size    | addr + 4 + size

*/

#define EPOCH 1
#define VALID 8

#define CELL_SIZE(val_size) (val_size + 4)

// Validate the particular storage cell.
// Clear VALID header bit if the cell content is considered as invalid.
// Returns either the value of the epoch bit (0 or 1) or -1 if the cell
// is considered as invalid.
static int8_t nv_tx_validate_cell(
		uint16_t crc,  // tag checksum
		unsigned size, // data size
		unsigned base_addr // base address of the cell
	)
{
	uint16_t head, tail;
	unsigned addr = base_addr;
	EEPROM.get(addr, head);
	if ((crc | VALID | EPOCH) != (head | EPOCH))
		goto invalid;
	for (addr += 2; size; --size, ++addr) {
		uint8_t b = EEPROM.read(addr);
		crc = crc16_up_(crc, b);
	}
	EEPROM.get(addr, tail);
	if (tail != crc)
		goto invalid;
	return head & EPOCH;
invalid:
	// Clear valid bit should head or tail checksum mismatch
	// So we can skip checksum verification in subsequent nv_tx_put call
	if (head & VALID)
		EEPROM.write(base_addr, 0);
	return -1;
}

// Read data from the cell given its base address
static inline void nv_tx_read_cell(
		void* val,
		unsigned size,
		unsigned addr
	)
{
	uint8_t* ptr = val;
	for (addr += 2; size; --size, ++addr, ++ptr)
		*ptr = EEPROM.read(addr);
}

// Write data to the cell given its base address
static inline void nv_tx_write_cell(
		const char* tag,
		void* val,
		unsigned size,
		unsigned addr,
		uint8_t epoch
	)
{
	uint16_t crc = crc16_str(tag);
	// Make header byte with valid bit set and proper epoch bit
	// Other header bits are equal to tag checksum bits
	uint16_t head = (crc & ~(uint16_t)EPOCH) | epoch | VALID;
	EEPROM.put(addr, head);
	uint8_t const* ptr = val;
	for (addr += 2; size; --size, ++addr, ++ptr) {
		uint8_t b = *ptr;
		EEPROM.update(addr, b);
		crc = crc16_up_(crc, b);
	}
	// Write final checksum as the tail
	EEPROM.put(addr, crc);
}

// Read the stored value. Returns true if the value was successfully read. This function should always be called
// before nv_tx_put to format storage even in case you are not interested in the current value.
bool nv_tx_get(
		const char* tag, // the value tag is the string name of the value that serves to identify its semantic
		void* val,       // pointer to the memory location where the value will be copied should the read succeed
		unsigned size,   // the value size in bytes
		unsigned addr    // the EEPROM base address where the value is stored
	)
{
	uint16_t tag_crc = crc16_str(tag);
	unsigned addr2 = addr + CELL_SIZE(size);
	int8_t valid[2] = {nv_tx_validate_cell(tag_crc, size, addr), nv_tx_validate_cell(tag_crc, size, addr2)};
	if (valid[0] >= 0 && valid[1] >= 0) {
		// Both cells have valid data so choose latest based on the epoch bit
		nv_tx_read_cell(val, size, valid[0] != valid[1] ? addr : addr2);
		return true;
	}
	// Read value from the valid cell
	if (valid[0] >= 0) {
		nv_tx_read_cell(val, size, addr);
		return true;
	}
	if (valid[1] >= 0) {
		nv_tx_read_cell(val, size, addr2);
		return true;
	}
	return false;
}

// Write new value. The caller is expected to call nv_tx_get first.
void nv_tx_put(
		const char* tag, // the value tag is the string name of the value that serves to identify its semantic
		void const* val, // pointer to the memory location from where the value will be copied to the EEPROM
		unsigned size,   // the value size in bytes
		unsigned addr    // the EEPROM base address where the value will be stored
	)
{
	unsigned addr2 = addr + CELL_SIZE(size);
	// Read header bytes from the cells
	uint8_t hdr[2] = {EEPROM.read(addr), EEPROM.read(addr2)};
	// Write to the first not valid cell using epoch calculated based on the other cell's epoch
	if (!(hdr[0] & VALID))
		nv_tx_write_cell(tag, val, size, addr, ~hdr[1] & EPOCH);
	else if (!(hdr[1] & VALID))
		nv_tx_write_cell(tag, val, size, addr2, hdr[0] & EPOCH);
	// If both cells have valid data choose the next one based on the epoch bits
	else if ((hdr[0] & EPOCH) == (hdr[1] & EPOCH))
		nv_tx_write_cell(tag, val, size, addr, ~hdr[1] & EPOCH);
	else
		nv_tx_write_cell(tag, val, size, addr2, hdr[0] & EPOCH);
}
