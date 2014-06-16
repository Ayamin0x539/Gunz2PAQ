// dllmain.cpp : Defines the entry point for the DLL application.
// PAQ by Ayamin
// For Gunz 2 Questing
//

#include "stdafx.h"
#include "GlobalConstants.h"
#include <string>

using namespace std;

// Pointers for dereferencing coordinates
DWORD ptr1, ptr2, ptr3, ptr4, ptr5, ptrX, ptrY, ptrZ;
// Address of the function we will be detouring for the vacuum
DWORD address = 0x00946CE8; // 0x00946CE8
// Return address - we need to jump back here after detouring
DWORD rtnAddress = 0x00946CF1; // 9 after "address"
// Return address of the z-bar edit. (Don't know how to implement this another way; see inline asm below)
DWORD zBarRtnAddress = 0x0087187D; // used to be 0x008701AD

int teleport = 0; // to keep track of the teleport location (no longer needed with the new map)
bool teleOn = false;
bool ammoOn = false;
bool zbarOn = false;
bool vacOn = false;

//char buffer[252]; // testing purposes


// Need this for the code cave
DWORD base = (DWORD)0x01C356E4; // address of the static pointer that we will trace to eventually get to our coordinates
DWORD x = 0, y = 0, z = 0; // variables to store coordinates of monsters, so we can bind them to the player
// Shift variables
float deltax = 700; // 0x43160000; // (float)150
float deltaz = 150; // 0x41200000; // (float)10
float zUp = 300; 
float zDown = 100;
float zStore = 0;
float zAddMe = 1400;
// End Code cave constants

// Useful functions: Float to dword conversion (bitwise)
DWORD floatToHex(float const num) {
	return *reinterpret_cast<DWORD const*>(&num);
}
float hexToFloat(DWORD const num) {
	return *reinterpret_cast<float const*>(&num);
}

// Sounds! :D
void playMajor() {
	Beep(523, 200);
	Beep(659, 200);
	Beep(784, 200);
	Beep(659, 200);
	Beep(523, 200);
}
void playMinor() {
	Beep(523, 200);
	Beep(622, 200);
	Beep(784, 200);
	Beep(622, 200);
	Beep(523, 200);
}
void playMario() {
	// Intro Mario
	Beep(659, 100); Sleep(100);
	Beep(659, 100); Sleep(300);
	Beep(659, 100); Sleep(300);
	Beep(523, 100); Sleep(100);
	Beep(659, 100); Sleep(300);
	Beep(784, 100); Sleep(700);
	Beep(392, 100); Sleep(700);
}

// Code Caves
// We will use these to detour functions in the game stored at certain addresses.
// These code caves will be referenced in the "Functions" section below.
//
__declspec(naked) void vacAll(void) {
	__asm {
		fld dword ptr[eax + 0x08]
			fstp dword ptr[edi + 0x000000A0]
			push ecx
			mov ecx, [base] // dereference multi-level pointers for player coordinates, and ...
			mov ecx, [ecx]
			mov ecx, [ecx + 0x120]
			mov ecx, [ecx + 0x10]
			mov ecx, [ecx + 0x98]
			mov[x], ecx // ... store the result into x, and do the same for y and z coordinates
			fld [x]
			fadd [deltax] // don't spawn the monster ON the player; shift it by deltax
			fstp[x]
			mov ecx, [base]
			mov ecx, [ecx]
			mov ecx, [ecx + 0x120]
			mov ecx, [ecx + 0x10]
			mov ecx, [ecx + 0x9C]
			mov[y], ecx
			mov ecx, [base]
			mov ecx, [ecx]
			mov ecx, [ecx + 0x120]
			mov ecx, [ecx + 0x10]
			mov ecx, [ecx + 0xA0]
			mov[z], ecx
			fld [z]
			fadd [deltaz]
			fstp [z]
			cmp[x], 0 // if we failed to store the coordinates, jump to exit
			je getout
			cmp dword ptr[edi + 0x50], 0x656C6973 // check to see if our player is being moved; if so, jump to exit and don't bind the coordinates! [edi = 0x50] holds a string that tells the class of the entity
			je getout
			cmp dword ptr[edi + 0x50], 0x736E7567
			je getout
			cmp dword ptr[edi + 0x50], 0x64616873
			je getout
			cmp dword ptr[edi + 0x50], 0x65696873
			je getout
			cmp dword ptr[edi + 0x50], 0x69727473
			je getout
			mov ecx, [x]
			mov[edi + 0x98], ecx // store the player's coordinates in the monster's coordinates, effectively "vacuuming" them
			mov ecx, [y]
			mov[edi + 0x9C], ecx
			mov ecx, [z]
			mov[edi + 0xA0], ecx
			pop ecx
			jmp[rtnAddress]

		getout:
			pop ecx
			jmp[rtnAddress]
	}
}

__declspec(naked) void zBar(void) {
	__asm {
		fld dword ptr[ecx]
			fadd[zAddMe] // add the value zAddMe to the zbar every time this function is called
			fstp dword ptr[eax]
			pop esi
			jmp[zBarRtnAddress]
	}
}

// Functions

// Ammo
void writeInfAmmo() {
	//MessageBoxA(NULL, "Infinite Ammo Enabled!", "", MB_OK);
	DWORD store;
	if (VirtualProtect((LPVOID)0x0095AB69, 3, PAGE_EXECUTE_READWRITE, &store)) {
		*(BYTE*)0x0095AB69 = 0x90; // nop the ammo decrease
		*(BYTE*)0x0095AB6A = 0x90;
		*(BYTE*)0x0095AB6B = 0x90;
		playMajor();
	}
}

void disableAmmo() {
	//MessageBoxA(NULL, "Infinite Ammo disabled.", "", MB_OK);
	DWORD store;
	if (VirtualProtect((LPVOID)0x0095AB69, 3, PAGE_EXECUTE_READWRITE, &store)) {
		*(BYTE*)0x0095AB69 = 0xFF; // restore the ammo decrease
		*(BYTE*)0x0095AB6A = 0x4F;
		*(BYTE*)0x0095AB6B = 0x08;
		playMinor();
	}
}

// Cooldown
void writeNoDelay() {
	//MessageBoxA(NULL, "Cooldown enabled.", "", MB_OK);
	DWORD store;
	if (VirtualProtect((LPVOID)0x009A3CE4, 2, PAGE_EXECUTE_READWRITE, &store)) {
		//		MessageBoxA(NULL, "No Delay Activated!", "", MB_OK);
		*(BYTE*)0x009A3CE4 = 0x90; // nop the delay function
		*(BYTE*)0x009A3CE5 = 0x90;
		playMajor();
	}
}

void disableNoDelay() {
	//MessageBoxA(NULL, "Cooldown disabled.", "", MB_OK);
	DWORD store;
	if (VirtualProtect((LPVOID)0x009A3CE4, 2, PAGE_EXECUTE_READWRITE, &store)) { // 009A2C54
		*(BYTE*)0x009A3CE4 = 0x74; // restore the delay function
		*(BYTE*)0x009A3CE5 = 0x04;
		playMinor();
	}
}

// Z bar
void writeZBar() {
	//MessageBoxA(NULL, "ZBar enabled.", "", MB_OK);
	DWORD storage;
	if (VirtualProtect((LPVOID)0x00871878, 5, PAGE_EXECUTE_READWRITE, &storage)) {
		*(BYTE*)0x00871878 = 0xE9; // jump to...
		*(DWORD*)0x00871879 = ((DWORD)zBar - 0x00871878 - 5); // the code cave function zBar()
		playMajor();
	}
}

void disableZBar() {
	//MessageBoxA(NULL, "ZBar disabled.", "", MB_OK);
	DWORD storage;
	if (VirtualProtect((LPVOID)0x00871878, 5, PAGE_EXECUTE_READWRITE, &storage)) {
		*(BYTE*)0x00871878 = 0xD9; // restore original byte code
		*(BYTE*)0x00871879 = 0x01;
		*(BYTE*)0x0087187A = 0xD9;
		*(BYTE*)0x0087187B = 0x18;
		*(BYTE*)0x0087187C = 0x5E;
		playMinor();
	}
}

// Vacuums
void writeVac() {
	//MessageBoxA(NULL, "Vac enabled for ALL.", "", MB_OK);
	*(BYTE*)address = 0xE9; // jump to...
	*(DWORD*)0x00946CE9 = ((DWORD)vacAll - 0x00946CE8 - 5); // the vacAll() code cave
	*(DWORD*)(address + 5) = 0x90909090; // four nop's
	playMario();
}

void restoreMemory() {
	//MessageBoxA(NULL, "Vac disabled.", "", MB_OK);
	*(BYTE*)0x00946CE8 = 0xD9;
	*(BYTE*)0x00946CE9 = 0x40;
	*(BYTE*)0x00946CEA = 0x08;
	*(BYTE*)0x00946CEB = 0xD9;
	*(BYTE*)0x00946CEC = 0x9F;
	*(BYTE*)0x00946CED = 0xA0;
	*(BYTE*)0x00946CEE = 0x00;
	*(BYTE*)0x00946CEF = 0x00;
	*(BYTE*)0x00946CF0 = 0x00;
	playMinor();
}

// Commented code in loop() is from old maps in old revisions. May implement them later in another version.

void loop() {
	while (1) {
		/*if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000 && !vacOn) {
			writeVac();
			vacOn = true;
		}
		if (GetAsyncKeyState(VK_ADD) & 0x8000 && vacOn) {
			vacOn = false;
			restoreMemory();
		}*/
		if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000) {
			if (!ammoOn) {
				ammoOn = true;
				writeInfAmmo();
			}
			else {
				ammoOn = false;
				disableAmmo();
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000) {
			if (!zbarOn) {
				zbarOn = true;
				writeZBar();
				writeNoDelay();
			}
			else {
				zbarOn = false;
				disableZBar();
				disableNoDelay();
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000) {
			teleOn = true;
			//MessageBoxA(NULL, "Teleports enabled!", "", MB_OK);
			playMajor();
		}
		while (teleOn) {
			ptr1 = *(DWORD*)(base); // 13A05B88
			ptr2 = (DWORD)(ptr1 + 0x120);
			ptr3 = *(DWORD*)(ptr2); // 30532998
			ptr4 = (DWORD)(ptr3 + 0x10);
			ptr5 = *(DWORD*)(ptr4); // 3A4D3028
			ptrX = (DWORD)(ptr5 + 0x98); // address of x-coord
			ptrY = (DWORD)(ptr5 + 0x9C); // address of y-coord
			ptrZ = (DWORD)(ptr5 + 0xA0); // address of z-coord
			if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000) {
				teleOn = false;
				teleport = 0;
				//MessageBoxA(NULL, "Teleports disabled!", "", MB_OK);
				playMinor();
			}
			//if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000) {
			//	teleport = 0;
			//	//MessageBoxA(NULL, "Teleport reset.", "", MB_OK);
			//	Beep(659, 200);
			//	Beep(784, 200);
			//	Beep(1046, 200);
			//}
			// Fallen Angel
			// Teleport 1
			if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
				*(DWORD*)ptrX = GlobalConstants::faRP1x;
				*(DWORD*)ptrY = GlobalConstants::faRP1y;
				*(DWORD*)ptrZ = GlobalConstants::faRP1z;
				Sleep(100);
			}
			// Teleport 2
			if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
				*(DWORD*)ptrX = GlobalConstants::faRP2x;
				*(DWORD*)ptrY = GlobalConstants::faRP2y;
				*(DWORD*)ptrZ = GlobalConstants::faRP2z;
				Sleep(100);
			}
			//// First teleport
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 0)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 1... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele1x;
			//	*(DWORD*)ptrY = GlobalConstants::tele1y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele1z;
			//	Sleep(100);
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 2)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 1... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele1x;
			//	*(DWORD*)ptrY = GlobalConstants::tele1y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele1z;
			//}
			//// Teleport 2
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 1)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 2... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele2x;
			//	*(DWORD*)ptrY = GlobalConstants::tele2y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele2z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 3)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 2... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele2x;
			//	*(DWORD*)ptrY = GlobalConstants::tele2y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele2z;
			//}
			//// Teleport 3
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 2)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 3... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele3x;
			//	*(DWORD*)ptrY = GlobalConstants::tele3y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele3z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 4)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 3... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele3x;
			//	*(DWORD*)ptrY = GlobalConstants::tele3y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele3z;
			//}
			//// Teleport 4
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 3)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 4... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele4x;
			//	*(DWORD*)ptrY = GlobalConstants::tele4y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele4z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 5)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 4... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele4x;
			//	*(DWORD*)ptrY = GlobalConstants::tele4y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele4z;
			//}
			//// Teleport 5
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 4)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 5... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele5x;
			//	*(DWORD*)ptrY = GlobalConstants::tele5y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele5z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 6)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 5... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele5x;
			//	*(DWORD*)ptrY = GlobalConstants::tele5y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele5z;
			//}
			//// Teleport 6
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 5)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 6... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele6x;
			//	*(DWORD*)ptrY = GlobalConstants::tele6y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele6z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 7)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 6... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele6x;
			//	*(DWORD*)ptrY = GlobalConstants::tele6y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele6z;
			//}
			//// Teleport 7
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 6)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 7... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele7x;
			//	*(DWORD*)ptrY = GlobalConstants::tele7y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele7z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 8)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 7... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele7x;
			//	*(DWORD*)ptrY = GlobalConstants::tele7y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele7z;
			//}
			//// Teleport 8
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 7)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 8... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele8x;
			//	*(DWORD*)ptrY = GlobalConstants::tele8y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele8z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 9)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 8... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele8x;
			//	*(DWORD*)ptrY = GlobalConstants::tele8y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele8z;
			//}
			//// Teleport 9
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 8)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 9... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele9x;
			//	*(DWORD*)ptrY = GlobalConstants::tele9y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele9z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 10)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 9... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele9x;
			//	*(DWORD*)ptrY = GlobalConstants::tele9y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele9z;
			//}
			//// Teleport 10
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 9)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 10... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele0x;
			//	*(DWORD*)ptrY = GlobalConstants::tele0y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele0z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 11)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 10... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele0x;
			//	*(DWORD*)ptrY = GlobalConstants::tele0y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele0z;
			//}
			//// Teleport 11
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 10)) { // i
			//	//MessageBoxA(NULL, "RIGHT: Spot 11... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::teledx;
			//	*(DWORD*)ptrY = GlobalConstants::teledy;
			//	*(DWORD*)ptrZ = GlobalConstants::teledz;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 12)) { // i
			//	//MessageBoxA(NULL, "LEFT: Spot 11... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::teledx;
			//	*(DWORD*)ptrY = GlobalConstants::teledy;
			//	*(DWORD*)ptrZ = GlobalConstants::teledz;
			//}
			//// Teleport 12
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 11)) { // o
			//	//MessageBoxA(NULL, "RIGHT: Spot 12... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele11x;
			//	*(DWORD*)ptrY = GlobalConstants::tele11y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele11z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 13)) { // o
			//	//MessageBoxA(NULL, "LEFT: Spot 12... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele11x;
			//	*(DWORD*)ptrY = GlobalConstants::tele11y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele11z;
			//}
			//// Teleport 13
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 12)) { // p
			//	//MessageBoxA(NULL, "RIGHT: Spot 13... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele12x;
			//	*(DWORD*)ptrY = GlobalConstants::tele12y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele12z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 14)) { // p
			//	//MessageBoxA(NULL, "LEFT: Spot 13... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele12x;
			//	*(DWORD*)ptrY = GlobalConstants::tele12y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele12z;
			//}
			//// Teleport 14
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 13)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 14... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele13x;
			//	*(DWORD*)ptrY = GlobalConstants::tele13y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele13z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 15)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 14... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele13x;
			//	*(DWORD*)ptrY = GlobalConstants::tele13y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele13z;
			//}
			//// Teleport 15
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 14)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 15... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele14x;
			//	*(DWORD*)ptrY = GlobalConstants::tele14y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele14z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 16)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 15... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele14x;
			//	*(DWORD*)ptrY = GlobalConstants::tele14y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele14z;
			//}
			//// Teleport 16
			//if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && (teleport == 15)) {
			//	//MessageBoxA(NULL, "RIGHT: Spot 16... Get ready!", "", MB_OK);
			//	teleport++;
			//	*(DWORD*)ptrX = GlobalConstants::tele15x;
			//	*(DWORD*)ptrY = GlobalConstants::tele15y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele15z;
			//}
			//if (GetAsyncKeyState(VK_LEFT) & 0x8000 && (teleport == 17)) {
			//	//MessageBoxA(NULL, "LEFT: Spot 16... Get ready!", "", MB_OK);
			//	teleport--;
			//	*(DWORD*)ptrX = GlobalConstants::tele15x;
			//	*(DWORD*)ptrY = GlobalConstants::tele15y;
			//	*(DWORD*)ptrZ = GlobalConstants::tele15z;
			//}
			if (GetAsyncKeyState(VK_UP) & 0x8000) {
				//DWORD herepls = (*(DWORD*)ptrZ + 0x437a0000);
				//*(DWORD*)ptrZ = herepls;
				DWORD zValue = *(DWORD*)ptrZ;
				float fNum = hexToFloat(zValue);
				float newValue = fNum + (float)1000.0;
				DWORD dValue = floatToHex(newValue);
				*(DWORD*)ptrZ = dValue;
				Sleep(100);
			}
			if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
				//DWORD downpls = (*(DWORD*)ptrZ - 0x42c80000);
				//*(DWORD*)ptrZ = downpls;
				DWORD zValue = *(DWORD*)ptrZ;
				float fNum = hexToFloat(zValue);
				float newValue = fNum - (float)150.0;
				DWORD dValue = floatToHex(newValue);
				*(DWORD*)ptrZ = dValue;
				Sleep(100);
			}
			// Off
			/*if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000 && !vacOn) {
				writeVac();
				vacOn = true;
			}
			if (GetAsyncKeyState(VK_ADD) & 0x8000 && vacOn) {
				vacOn = false;
				restoreMemory();
			}*/
			if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000) {
				if (!ammoOn) {
					ammoOn = true;
					writeInfAmmo();
				}
				else {
					ammoOn = false;
					disableAmmo();
				}
			}
			if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000) {
				if (!zbarOn) {
					zbarOn = true;
					writeZBar();
					writeNoDelay();
				}
				else {
					zbarOn = false;
					disableZBar();
					disableNoDelay();
				}
			}
			Sleep(100);
		}
		Sleep(100);
	}
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		MessageBoxA(NULL, "Vacuum disabled.", "PAQ V2.07", MB_OK);
		DWORD old;
		if (VirtualProtect((LPVOID)address, 9, PAGE_EXECUTE_READWRITE, &old))
			MessageBoxA(NULL, "Made by Ayamin.", "", MB_OK);
		Sleep(500);

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&loop, 0, 0, 0);
	}
	return TRUE;
}