#include "c_storage_win.h"
#include <string.h>

static char * _eeprom;
static char eeprom[256000];

void HardwareAbstractionLayer::Storage::initialize()
{

}
void HardwareAbstractionLayer::Storage::load(char* data, uint16_t size)
{
	memcpy(data, eeprom, size);
}
void HardwareAbstractionLayer::Storage::save(char* data, uint16_t size)
{
	memcpy(eeprom, data, size);
}