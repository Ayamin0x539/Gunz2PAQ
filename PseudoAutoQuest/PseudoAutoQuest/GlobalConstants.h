#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <Windows.h>

class GlobalConstants{
public:

	//DWORD tele1x = 0x00809f43; // 319
	//DWORD tele1y = 0x006892c5; // -4685
	////DWORD tele1z = 0x0080e744;
	//DWORD tele1z = 0x9822e744; // 1839.081055

	//unreversedreverse:
	static const DWORD 
		tele1x = 0x439f8000, // 319
		tele1y = 0xc5926800, // -4685
		tele1z = 0x44e78000, // 1852 THEY DONT NEED TO BE BACKWARDS HERE! :D
		
		tele2x = 0x449f8000,
		tele2y = 0xc56e6000,
		tele2z = 0x44550000,
		
		tele3x = 0x4534c000,
		tele3y = 0xc4e18000,
		tele3z = 0x44550000,
	
		tele4x = 0x4576a000,
		tele4y = 0xc3670000,
		tele4z = 0x44550000,
	//
		tele5x = 0x45192000,
		tele5y = 0x450fc000,
		tele5z = 0x44e78000,
	//
		tele6x = 0x43d78000,
		tele6y = 0x451ed000,
		tele6z = 0x44e78000,
	//
	//
		tele7x = 0xc44c0000,
		tele7y = 0x4408c000,
		tele7z = 0x44e78000,
	//
		tele8x = 0xc48e2000,
		tele8y = 0xc35d0000,
		tele8z = 0x44e78000,
	//
		tele9x = 0xc5079000,
		tele9y = 0x43fc0000,
		tele9z = 0x44e78000,
	//
		tele0x = 0xc4208000,
		tele0y = 0xc538b000,
		tele0z = 0x44e78000,
	// - -> i
		teledx = 0x45362000,
		teledy = 0xc57ee000,
		teledz = 0x44e78000,
	// y (equals) -> o
		tele11x = 0x450a6000,
		tele11y = 0x4495a000,
		tele11z = 0x45351000,
	 // u (backspace) -> p
		tele12x = 0xc474c000,
		tele12y = 0x44c9c000,
		tele12z = 0x45324000,
	// i (numpad7)
		tele13x = 0x43480000,
		tele13y = 0xc4b00000,
		tele13z = 0x44af4000,
	// o (numpad8)
		tele14x = 0x428e0000,
		tele14y = 0x451bf000,
		tele14z = 0x43520000, // needs to be higher
	// p (numpad9)
		tele15x = 0xc3bf8000,
		tele15y = 0xc559d000,
		tele15z = 0x40000000,
	//
	// Fallen Angel
		
		faRP1x = 0xc4612800,
		faRP1y = 0xc3b5b78d,
		faRP1z = 0x41200000,

		//faRP1x = 0xc431d333,
		//faRP1y = 0xc2ff47ae,
		//faRP1z = 0x41200000,
		//
		faRP2x = 0x45884229,
		faRP2y = 0x45712c7b,
		faRP2z = 0x0;
};

#endif