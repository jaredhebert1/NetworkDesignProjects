/*
	Jared Hebert and Andrew O'Brien
	EECE.4830
	Project Phase 2
	RDT 1.0 Server

	code from https://www.binarytides.com/udp-socket-programming-in-winsock/ used
*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include <iostream>
#include<winsock2.h>
#include <WS2tcpip.h>


using namespace::std;

char packetData[1024];
char buf[1024];
char outStr[] = "bmp_out.bmp";
FILE* fpOut;
 
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 1024
#define packetDebugMode 1

int Make_File(int newFile) {
	int fwriteReturn;
	int fileEnd = 0;
	
	if (newFile == 1) {
		fpOut = fopen(outStr, "wb"); 
		
		if (fpOut == NULL) {
			printf("Failed to Open File\n");
			return -1;
		}
	}
	
	fwriteReturn = fwrite(packetData, 1, 1024, fpOut);
	if (fwriteReturn == 0) {
		fileEnd = 1;
	}
	
	if (fileEnd == 1) {
		fclose(fpOut);
	}
	
	return fileEnd; 	
}

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET server;
	sockaddr_in server_addr, client;
	hostent* localHost;
	int slen, recv_len;
	char* localIP;

	slen = sizeof(client);

	//init winsock library
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//create socket
	if ((server = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	//define server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(8888);

	//bind socket to defined address
	if (bind(server, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {

		printf("Bind failed with error code : %d" , WSAGetLastError());
		exit(EXIT_FAILURE);

	}

	printf("Bind Done\n");
	
	if (packetDebugMode == 1) {
		
		int fileEnd = 0;
		int newFile = 1; 
		
		while (fileEnd == 0) {
			printf("Waiting for data...");
			fflush(stdout);
				
			//clear the buffer by filling null, it might have previously received data
			memset(buf, '\0', BUFLEN);

			//try to receive some data, this is a blocking call
			if ((recv_len = recvfrom(server, packetData, 1024, 0, (struct sockaddr*)&client, &slen)) == SOCKET_ERROR)
			{
				printf("recvfrom() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
				
			//print details of the client/peer and the data received
			printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
			printf("Data: %s\n", packetData);

			//now reply the client with the same data
			if (sendto(server, packetData, recv_len, 0, (struct sockaddr*)&client, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			
			fileEnd = Make_File(newFile); 
			if (newFile == 1) {
				newFile = 0;
			}
		}
	
	}
	//close socket
	closesocket(server);
	WSACleanup();

	return 0;
}