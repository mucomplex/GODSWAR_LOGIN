#include "gwsocket.h"
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <thread>
#include "gwfunction.h"
#define PORT "5999"

#pragma comment(lib,"Ws2_32.lib")

void user_connection() {
	WSADATA GWINIT;
	if (WSAStartup(MAKEWORD(2, 2), &GWINIT)) {
		printf("WSAStartup failed with error :%d]n", WSAGetLastError());
	}

	struct CLIENT {
		SOCKET sock;
	};

	std::vector<CLIENT> login_user;

	std::thread gwlogin([&login_user]() {
		int iResult;
		SOCKET ListenSocket = INVALID_SOCKET;
		SOCKET ClientSocket = INVALID_SOCKET;

		struct addrinfo* result = NULL;
		struct addrinfo hints;

		SecureZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		iResult = getaddrinfo(NULL, PORT, &hints, &result);
		if (iResult != 0) {
			printf("getaddrinfo failed with error :%d\n", iResult);
			WSACleanup();
		}

		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			printf("Socket failed with error : %d\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
		}

		iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
		}

		//freeaddrinfo(result);

		iResult = listen(ListenSocket, SOMAXCONN);
		while (true) {
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			closesocket(ListenSocket);
			WSACleanup();
		}
		u_long non_blocking = true;
		ioctlsocket(ClientSocket, FIONBIO, &non_blocking);

		CLIENT new_client;
		new_client.sock = ClientSocket;
		login_user.push_back(new_client);
		std::cout << "new client connected!\n" << std::endl;
		}});

	std::thread gwsuccess_login([&login_user]() {
		BYTE buffer[1024];
		int buffersize;
		while (true) {
			for (int i = 0; i < login_user.size(); i++) {
				SecureZeroMemory(buffer, sizeof(buffer));
				//godswar start here.
				if ((buffersize = recv(login_user[i].sock,(char *)buffer,sizeof(buffer),NULL)) > 0) {
					GWMain Start_Main;
					Start_Main.accept_packet(login_user[i].sock,buffer, buffersize);
				}
			}
		}
		});

	// Should be other threading to remove socket when user are offline.

	//terminate threading
	gwlogin.join();
	gwsuccess_login.join();
}