#include "ClientNetUtil.h"


bool FClientNetUtil::CheckCPUendian()

{
	union {
		unsigned long int i;
		unsigned char s[4];
	}c;

	c.i = 0x12345678;

	return (0x12 == c.s[0]);
}

unsigned long int FClientNetUtil::t_ntohl(unsigned long int n)
{
	// 若本机为大端，与网络字节序同，直接返回
	// 若本机为小端，网络数据转换成小端再返回
	return CheckCPUendian() ? n : BigLittleSwap32(n);
}

unsigned long int FClientNetUtil::t_htonl(unsigned long int h)
{
	// 若本机为大端，与网络字节序同，直接返回
	// 若本机为小端，转换成大端再返回
	return CheckCPUendian() ? h : BigLittleSwap32(h);
}

unsigned short FClientNetUtil::t_ntohs(unsigned short n)
{
	// 若本机为大端，与网络字节序同，直接返回
	// 若本机为小端，网络数据转换成小端再返回
	return CheckCPUendian() ? n : BigLittleSwap16(n);
}

unsigned short FClientNetUtil::t_htons(unsigned short h)
{
	// 若本机为大端，与网络字节序同，直接返回
	// 若本机为小端，转换成大端再返回
	return CheckCPUendian() ? h : BigLittleSwap16(h);
}

int FClientNetUtil::EncodeInt32(char** pstrEncode, unsigned int uiSrc)
{
	if (pstrEncode == NULL || *pstrEncode == NULL)
	{
		return 0;
	}

	**pstrEncode = (char)((uiSrc & 0xFF000000) >> 24);
	(*pstrEncode)++;
	**pstrEncode = (char)((uiSrc & 0xFF0000) >> 16);
	(*pstrEncode)++;
	**pstrEncode = (char)((uiSrc & 0xFF00) >> 8);
	(*pstrEncode)++;
	**pstrEncode = (char)(uiSrc & 0xFF);
	(*pstrEncode)++;

	return 4;
}

int FClientNetUtil::DecodeInt32(char** pstrDecode, unsigned int* puiDest)
{
	if (pstrDecode == NULL || *pstrDecode == NULL || puiDest == NULL)
	{
		return 0;
	}

	*puiDest = (unsigned char)**pstrDecode;
	*puiDest <<= 8;
	(*pstrDecode)++;

	*puiDest += (unsigned char)**pstrDecode;
	*puiDest <<= 8;
	(*pstrDecode)++;

	*puiDest += (unsigned char)**pstrDecode;
	*puiDest <<= 8;
	(*pstrDecode)++;

	*puiDest += (unsigned char)**pstrDecode;
	(*pstrDecode)++;

	return 4;
}