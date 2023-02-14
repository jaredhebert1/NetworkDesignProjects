/*
	Jared Hebert and Andrew O'Brien
	EECE.4830
	Project Phase 2
	RDT 1.0 Client

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
char inStr[] = "bmp_example.bmp";
//char outStr[] = "bmp_out.bmp";
FILE* fpIn;
//FILE* fpOut;

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 1024
#define packetDebugMode 1

int Make_Packet(int newFile) {
	int freadReturn;
	int fileEnd;
	
	if (newFile == 1) {
		fpIn = fopen(inStr, "rb"); 
		
		if (fpIn == NULL) {
			printf("Failed to Open file\n"); 
			return -1;			
		}
	}
	
	freadReturn = fread(packetData, 1, 1024, fpIn); 
	if (freadReturn == 0) {
		fileEnd = 1;
	}
	
	if (fileEnd == 1) {
		fclose(fpIn); 
	}
	
	return fileEnd; 
	
}

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET client;
	sockaddr_in client_addr, si_other;
	hostent* localHost;

	char buf[BUFLEN];
	//char message[BUFLEN] = "hello";	//message to send

	int slen;

	slen = sizeof(si_other);

	char* localIP;


	//Init winsock library
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//create socket
	if ((client = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	//cout << "Local IP: " << inet_addr("127.0.0.1") << endl;

	//define address of server to cennect to
	si_other.sin_family = AF_INET;
	si_other.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	si_other.sin_port = htons(8888);

	if (packetDebugMode == 1) {
		
		int fileEnd = 0;
		int newFile = 1;
		
			
		// Start Comunication 
		while (fileEnd == 0) {
			

			fileEnd = Make_Packet(newFile);
			if (newFile == 1) {
				newFile = 0;
			}
			
			if (fileEnd == -1) {
				printf("File did not open\n"); 
				return 1;
			}
			
			if (sendto(client, packetData, strlen(packetData), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR){
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			//receive a reply and print it
			//clear the buffer by filling null, it might have previously received data
			memset(buf, '\0', BUFLEN);
			//try to receive some data, this is a blocking call
			if (recvfrom(client, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen) == SOCKET_ERROR){
				printf("recvfrom() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			puts(buf);
			Sleep(500);
				
		}	
	}

	//close socket
	closesocket(client);
	WSACleanup();

	return 0;
}