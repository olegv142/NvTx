#pragma once

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
*/

// Read the stored value. Returns true if the value was successfully read. This function should always be called
// before nv_tx_put to format storage even in case you are not interested in the current value.
bool nv_tx_get(
		const char* tag, // the value tag is the string name of the value that serves to identify its semantic
		void* val,       // pointer to the memory location where the value will be copied should the read succeed
		unsigned size,   // the value size in bytes
		unsigned addr    // the EEPROM base address where the value is stored
	);

// Write new value. The caller is expected to call nv_tx_get first.
void nv_tx_put(
		const char* tag, // the value tag is the string name of the value that serves to identify its semantic
		void const* val, // pointer to the memory location from where the value will be copied to the EEPROM
		unsigned size,   // the value size in bytes
		unsigned addr    // the EEPROM base address where the value will be stored
	);

// The following helper macro get variable name as the parameter. The only other parameter required is EEPROM base address.
#define NvTxGetAt(var, addr) nv_tx_get(#var, &var, sizeof(var), addr)
#define NvTxPutAt(var, addr) nv_tx_put(#var, &var, sizeof(var), addr)

// Calculates the amount of storage is takes on EEPROM to save the value of the variable
#define NvTxSize(var) (2 * (4 + sizeof(var)))

//
// The following set of macro provide the means for automatically assigning addresses to storage locations
//

// Declare variable's storage address explicitly.
// Typically used for declaring first address of the sequence of storage locations.
#define NvPlace(var, addr) enum {var##_addr = addr}
// Assign the address to 'var's storage next to the 'prev's storage location
#define NvAfter(var, prev) enum {var##_addr = prev##_addr + NvTxSize(prev)}
// Get the address assigned to the variable's storage location
#define NvAddr(var) var##_addr

// Get/put variable using the previously declared address
#define NvTxGet(var) NvTxGetAt(var, NvAddr(var))
#define NvTxPut(var) NvTxPutAt(var, NvAddr(var))
