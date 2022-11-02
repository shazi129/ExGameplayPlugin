#pragma once

#include "CoreMinimal.h"

// 短整型大小端互换
#define BigLittleSwap16(A)  ((((uint16)(A) & 0xff00) >> 8) | (((uint16)(A) & 0x00ff) << 8))

// 长整型大小端互换
#define BigLittleSwap32(A)  ((((uint32)(A) & 0xff000000) >> 24) | (((uint32)(A) & 0x00ff0000) >> 8) | (((uint32)(A) & 0x0000ff00) << 8) | (((uint32)(A) & 0x000000ff) << 24))


//UCLASS()
class FClientNetUtil
{
public:
	// 本机大端返回true，小端返回false
	static bool CheckCPUendian(); 

	// 若本机为大端，与网络字节序同，直接返回
	// 若本机为小端，网络数据转换成小端再返回
	static unsigned long int t_ntohl(unsigned long int  n);

	static unsigned long int t_htonl(unsigned long int  h);

	static unsigned short t_ntohs(unsigned short n);

	static unsigned short t_htons(unsigned short h);

	static int EncodeInt32(char** pstrEncode, unsigned int uiSrc);

	static int DecodeInt32(char** pstrDecode, unsigned int* puiDest);
};