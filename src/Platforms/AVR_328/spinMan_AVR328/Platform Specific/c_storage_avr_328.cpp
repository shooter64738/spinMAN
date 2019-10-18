#include "c_storage_avr_328.h"

#include <avr/eeprom.h>

void HardwareAbstractionLayer::Storage::initialize()
{
	
}
void HardwareAbstractionLayer::Storage::load(char* data, uint16_t size)
{
	eeprom_read_block(data, 0, size);
}
void HardwareAbstractionLayer::Storage::save(char* data, uint16_t size)
{
	eeprom_write_block(data, (void*)0, size);
}

