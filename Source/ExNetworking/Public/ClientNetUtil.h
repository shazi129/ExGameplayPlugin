#pragma once

#include "CoreMinimal.h"

// �����ʹ�С�˻���
#define BigLittleSwap16(A)  ((((uint16)(A) & 0xff00) >> 8) | (((uint16)(A) & 0x00ff) << 8))

// �����ʹ�С�˻���
#define BigLittleSwap32(A)  ((((uint32)(A) & 0xff000000) >> 24) | (((uint32)(A) & 0x00ff0000) >> 8) | (((uint32)(A) & 0x0000ff00) << 8) | (((uint32)(A) & 0x000000ff) << 24))


//UCLASS()
class FClientNetUtil
{
public:
	// ������˷���true��С�˷���false
	static bool CheckCPUendian(); 

	// ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���
	// ������ΪС�ˣ���������ת����С���ٷ���
	static unsigned long int t_ntohl(unsigned long int  n);

	static unsigned long int t_htonl(unsigned long int  h);

	static unsigned short t_ntohs(unsigned short n);

	static unsigned short t_htons(unsigned short h);

	static int EncodeInt32(char** pstrEncode, unsigned int uiSrc);

	static int DecodeInt32(char** pstrDecode, unsigned int* puiDest);
};