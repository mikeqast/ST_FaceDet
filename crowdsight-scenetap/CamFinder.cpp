#include "CamFinder.h"

void CamFinder::scanNetwork()
{
	std::stringstream tempIP;
	int lastDigit = subnet.S_un.S_un_b.s_b4;
	for(int i=0;i<255;++i)
	{
		if(scanIP((i+lastDigit)%255))
		{
			tempIP<<"rtsp://"<<(int)subnet.S_un.S_un_b.s_b1<<'.'<<(int)subnet.S_un.S_un_b.s_b2<<'.'<<(int)subnet.S_un.S_un_b.s_b3<<'.'<<(int)subnet.S_un.S_un_b.s_b4<<":554/live.sdp";
			IPstring = tempIP.str();
			return;
		}
	}
	std::cout<<"failed to locate device";
}
bool CamFinder::scanIP(int lastDigit)
{
	DWORD retVal;
	ULONG MACtoCheck[2];
	ULONG PhysicalLength = 6;
	BYTE* tempMAC;
	subnet.S_un.S_un_b.s_b4 = lastDigit;
	std::cout<<(int)subnet.S_un.S_un_b.s_b4<<"\n";
	memset(&MACtoCheck, 0xff, sizeof (MACtoCheck));
	retVal = SendARP(IPAddr(subnet.S_un.S_addr),0, &MACtoCheck, &PhysicalLength);

	tempMAC = (BYTE*) &MACtoCheck;
	bool equal = true;
	for(int x=0;x<6;++x)
	{
		if((int)tempMAC[x]!=(int)target[x])
			equal=false;
	}
	return equal;
}
void CamFinder::StringToIP(std::string address)
{
	 std::stringstream addr;
	 char temp[4];
	 addr << address;
	 addr.getline(temp,4,'.');
	 subnet.S_un.S_un_b.s_b1 = atoi(temp);


	 addr.getline(temp,4,'.');
	 subnet.S_un.S_un_b.s_b2 = atoi(temp);

	 addr.getline(temp,4,'.');
	 subnet.S_un.S_un_b.s_b3 = atoi(temp);

	 addr.getline(temp,4,'.');
	 subnet.S_un.S_un_b.s_b4 = atoi(temp);
}

void CamFinder::StringToMAC(std::string address)
{
	std::stringstream addr, bytePair;
	char temp[3];
	addr << address;
	bytePair.clear();
	for(int i=0;i<6;++i)
	{
		addr.getline(temp,3,':');
		
		bytePair << std::hex << temp;
		int x;
		bytePair>>x;
		
		target[i] = x;
		bytePair.clear();		
	}
}