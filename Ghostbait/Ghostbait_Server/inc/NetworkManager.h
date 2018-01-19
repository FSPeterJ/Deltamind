#pragma once
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")


class Network
{
	SOCKET entryTCP = socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP);
	SOCKET entryUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);


};