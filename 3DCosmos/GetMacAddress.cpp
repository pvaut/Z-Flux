#include "stdafx.h"

#include "qstring.h"

#include <iphlpapi.h>

#pragma comment(lib, "IPHLPAPI.lib")



// Fetches the MAC address and prints it
void GetMACaddress(QString &addressname)
{
	QString encoding=_qstr("0123456789ABCDE.........");
	addressname.clear();

	IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
										 // for up to 16 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
				AdapterInfo,                 // [out] buffer to receive data
				&dwBufLen);                  // [in] size of receive data buffer
	if (dwStatus!=ERROR_SUCCESS) return;

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to current adapter info

	if (pAdapterInfo!=NULL)
	{
		for (int i=0; i<pAdapterInfo->AddressLength; i++)
		{
			BYTE bt=pAdapterInfo->Address[i];
			int bt1=bt/16;
			int bt2=bt%16;
			if (i>0) addressname+=_qstr("-");
			addressname+=encoding[bt1];
			addressname+=encoding[bt2];
		}
	}

/*	do {
	//    PrintMACaddress(pAdapterInfo->Address); // Print MAC address
	pAdapterInfo = pAdapterInfo->Next;    // Progress through
										  // linked list
	}
	while(pAdapterInfo);                    // Terminate if last adapter*/
}
