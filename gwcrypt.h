#pragma once
#include <Windows.h>

void gwcrypt(BYTE* packet, int packet_size);
void user_encrypt(BYTE* username, int username_size);
void user_decrypt(BYTE* username, int username_size);

