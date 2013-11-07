#ifndef ST_CAM_FINDER
#define ST_CAM_FINDER


#if WIN32
#include <Winsock2.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

#include <string>
#include <sstream>
#include <iostream>

class CamFinder
{
public:
	unsigned char target[6];
	in_addr subnet;
	in_addr properIP;
	bool scanIP(int lastVal);
	void scanNetwork();
	void StringToIP(std::string address);
	void StringToMAC(std::string address);
	std::string IPstring;
private:
	
	
};

#endif