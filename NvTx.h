#pragma once

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
#define NvTxGet(var, addr) nv_tx_get(#var, &var, sizeof(var), addr)
#define NvTxPut(var, addr) nv_tx_put(#var, &var, sizeof(var), addr)

// Calculates the amount of storage is takes on EEPROM to save the value of the variable
#define NvTxSize(var) (2 * (4 + sizeof(var)))
