#ifndef __C_EEPROM_SAM_D21_H__
#define __C_EEPROM_SAM_D21_H__


namespace HardwareAbstractionLayer
{
	class Storage
	{
		public:
		static void initialize();
		static void load(char* data, uint16_t size);
		static void save(char* data, uint16_t size);
		
	};
};
#endif