
/*
*  s_Buffer.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef S_BUFFER_H_
#define S_BUFFER_H_
#define RX_BUFFER_SIZE 256
#include <stdint.h>

typedef struct
{
	char Buffer[RX_BUFFER_SIZE];
	uint16_t volatile Head;
	uint16_t volatile Tail;
	uint8_t OverFlow;
	uint8_t volatile EOL;

} s_Buffer;



#endif /* S_BUFFER_H_ */