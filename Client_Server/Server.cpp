#include "stdio.h"
#include "WinSock2.h"
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#define DEFAULT_PORT "27015"

void main(){

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult!=0){
		printf("WSA Startup is failed with error %d", iResult);
		return;
	}


	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return;
	}

	SOCKET MasterSocket = INVALID_SOCKET;
	MasterSocket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);

	if (MasterSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Setup the TCP listening socket
	iResult = bind(MasterSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(MasterSocket);
		WSACleanup();
		return;
	}

	if (listen(MasterSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(MasterSocket);
		WSACleanup();
		return;
	}

	SOCKET ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(MasterSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(MasterSocket);
		WSACleanup();
		return;
	}
	
#define DEFAULT_BUFLEN 512

	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}

	} while (iResult > 0);


}

