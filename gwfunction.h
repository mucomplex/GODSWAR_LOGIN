#pragma once
#include "gwcrypt.h"
#include <Windows.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

struct gwheader {
	WORD header;
};

struct PlayerLogin {
	WORD header;
	WORD unknown1;
	BYTE username[32];
	BYTE password[32];
	DWORD unknown2;
	BYTE unknown3[18];
	BYTE padding[42];
	DWORD unknown4;
};

struct serverlist {
	WORD header = 0x06; // header?
	BYTE unknown2 = 0x03; // packet opcode?
	BYTE unknown3 = 0x00;
	BYTE unknown4 = 0x01;
	BYTE unknown5 = 0x01;
	BYTE unknown6[2] = { 0x30,0x00 };
	BYTE unknown7[2] = { 0x18,0x27 };
	BYTE server_name[9] = { 0x6d, 0x75, 0x63, 0x6f, 0x6d, 0x70, 0x6c, 0x65, 0x00 };
	BYTE unknown8[2] = { 0xC4, 0xB6 }; 
	BYTE unknown9[2] = { 0xDD, 0x41 };
	BYTE unknown10[6] = { 0x00, 0x00, 0x00, 0x00, 0x28, 0xB6 };
	BYTE unknown11[2] = { 0xDD, 0x41 };
	BYTE unknown12[2] = { 0x44, 0xB6 }; 
	BYTE unknown13[4] = { 0xDD, 0x41, 0xC1, 0x5A };
	BYTE unknown14[4] = { 0x41, 0x00, 0x44, 0xB6 };
	BYTE unknown15[4] = { 0xDD, 0x41, 0x15, 0x05 };
	BYTE unknown16 = 0x00;
	BYTE unknown17 = 0x00;
	BYTE id = 0x01;
	BYTE recommend = 0x01; 
	BYTE unknown18[3] = { 0x23, 0x00, 0x0F };
	BYTE unknown19 = 0x01;
	BYTE endlist = 0x01;
	BYTE unknown20 = 0x00;
};

class GWMain
{
private:


public:
	int accept_packet(SOCKET& current_user,BYTE* buffer, int buffer_size) {
		// create temporary space
		BYTE* new_buffer = new __nothrow BYTE[buffer_size];
		SecureZeroMemory(new_buffer, buffer_size);
		memcpy(new_buffer, buffer, buffer_size);
		gwcrypt(new_buffer, buffer_size);
		
		// write recv buffer
		//fwrite((char *)new_buffer, sizeof(char), buffer_size, stdout);
		//std::cout << std::endl;

		//define header
		gwheader* current_header = reinterpret_cast<gwheader *>(new_buffer);

		//authenticate header
		if (current_header->header == 0x008c) {
			PlayerLogin* current_player = reinterpret_cast<PlayerLogin *>(new_buffer);
			std::string username = reinterpret_cast<char const*>(current_player->username);
			std::cout << "Size of packet :" << username.size() << std::endl;
			user_decrypt((BYTE*)username.c_str(), username.size());
			std::cout << username + " is login!" << std::endl;
			
		// create serverlist.
			serverlist myserver;
			serverlist* server_list = &myserver;
			std::cout << sizeof(myserver) << std::endl;
			gwcrypt((BYTE *)server_list,sizeof(myserver));
			send(current_user,(const char *)server_list,sizeof(myserver),NULL);
		}

		// function closing
		current_header->header = NULL; // reset the header
		delete[] new_buffer;
		return 0;
	}
};