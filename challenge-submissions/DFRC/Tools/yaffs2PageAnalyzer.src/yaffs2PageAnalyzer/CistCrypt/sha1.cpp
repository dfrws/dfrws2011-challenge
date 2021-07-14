/*************** Header files *********************************************/
#include "stdafx.h"
#include "sha1.h"

/*************** Assertions ***********************************************/

/*************** Definitions / Macros  ************************************/

/*************** New Data Types *******************************************/

/*************** Global Variables *****************************************/

/*************** Prototypes ***********************************************/

/*************** Macros ***************************************************/
//	define boolean operations
#define F(x, y, z)	( (z)^( (x) & ((y) ^ (z)) ) )
#define G(x, y, z)	( (x) ^ (y) ^ (z) )
#define H(x, y, z)	( ((x)&(y)) | ( (z)&((x) | (y)) ) )

//	define step operations
#define FF(a,b,c,d,e,X) {								\
	(e) += ROTL_DWORD((a), 5) + F((b),(c),(d)) + X + K;	\
	(b) = ROTL_DWORD((b), 30);							\
}
#define GG(a,b,c,d,e,X) {								\
	(e) += ROTL_DWORD((a), 5) + G((b),(c),(d)) + X + K;	\
	(b) = ROTL_DWORD((b), 30);							\
}
#define HH(a,b,c,d,e,X) {								\
	(e) += ROTL_DWORD((a), 5) + H((b),(c),(d)) + X + K;	\
	(b) = ROTL_DWORD((b), 30);							\
}
#define II(a,b,c,d,e,X) {								\
	(e) += ROTL_DWORD((a), 5) + G((b),(c),(d)) + X + K;	\
	(b) = ROTL_DWORD((b), 30);							\
}

/*************** Function *************************************************
*	SHA1 core fuction
*/
static void SHA1_Transform(DWORD *Y, DWORD *DigestValue)
{
	DWORD a, b, c, d, e, X[80], K;

#if defined(BIG_ENDIAN)
	#define GetData(x)	x
#else
	#define GetData(x)	ENDIAN_REVERSE_DWORD(x)
#endif

	//	unroll loops
	X[ 0] = GetData(Y[ 0]);
	X[ 1] = GetData(Y[ 1]);
	X[ 2] = GetData(Y[ 2]);
	X[ 3] = GetData(Y[ 3]);
	X[ 4] = GetData(Y[ 4]);
	X[ 5] = GetData(Y[ 5]);
	X[ 6] = GetData(Y[ 6]);
	X[ 7] = GetData(Y[ 7]);
	X[ 8] = GetData(Y[ 8]);
	X[ 9] = GetData(Y[ 9]);
	X[10] = GetData(Y[10]);
	X[11] = GetData(Y[11]);
	X[12] = GetData(Y[12]);
	X[13] = GetData(Y[13]);
	X[14] = GetData(Y[14]);
	X[15] = GetData(Y[15]);

	#define TT(t)	X[t] = ROTL_DWORD(X[t-16]^X[t-14]^X[t-8]^X[t-3], 1)
	TT(16);	TT(17);	TT(18);	TT(19);	TT(20);	TT(21);	TT(22);	TT(23);
	TT(24);	TT(25);	TT(26);	TT(27);	TT(28);	TT(29);	TT(30);	TT(31);
	TT(32);	TT(33);	TT(34);	TT(35);	TT(36);	TT(37);	TT(38);	TT(39);
	TT(40);	TT(41);	TT(42);	TT(43);	TT(44);	TT(45);	TT(46);	TT(47);
	TT(48);	TT(49);	TT(50);	TT(51);	TT(52);	TT(53);	TT(54);	TT(55);
	TT(56);	TT(57);	TT(58);	TT(59);	TT(60);	TT(61);	TT(62);	TT(63);
	TT(64);	TT(65);	TT(66);	TT(67);	TT(68);	TT(69);	TT(70);	TT(71);
	TT(72);	TT(73);	TT(74);	TT(75);	TT(76);	TT(77);	TT(78);	TT(79);

	a=DigestValue[0];
	b=DigestValue[1];
	c=DigestValue[2];
	d=DigestValue[3];
	e=DigestValue[4];

	//	1st round
	K = 0x5A827999;
	FF(a,b,c,d,e,X[ 0]);	FF(e,a,b,c,d,X[ 1]);
	FF(d,e,a,b,c,X[ 2]);	FF(c,d,e,a,b,X[ 3]);
	FF(b,c,d,e,a,X[ 4]);	FF(a,b,c,d,e,X[ 5]);
	FF(e,a,b,c,d,X[ 6]);	FF(d,e,a,b,c,X[ 7]);
	FF(c,d,e,a,b,X[ 8]);	FF(b,c,d,e,a,X[ 9]);
	FF(a,b,c,d,e,X[10]);	FF(e,a,b,c,d,X[11]);
	FF(d,e,a,b,c,X[12]);	FF(c,d,e,a,b,X[13]);
	FF(b,c,d,e,a,X[14]);	FF(a,b,c,d,e,X[15]);
	FF(e,a,b,c,d,X[16]);	FF(d,e,a,b,c,X[17]);
	FF(c,d,e,a,b,X[18]);	FF(b,c,d,e,a,X[19]);

	//	2nd round
	K = 0x6ED9EBA1;
	GG(a,b,c,d,e,X[20]);	GG(e,a,b,c,d,X[21]);
	GG(d,e,a,b,c,X[22]);	GG(c,d,e,a,b,X[23]);
	GG(b,c,d,e,a,X[24]);	GG(a,b,c,d,e,X[25]);
	GG(e,a,b,c,d,X[26]);	GG(d,e,a,b,c,X[27]);
	GG(c,d,e,a,b,X[28]);	GG(b,c,d,e,a,X[29]);
	GG(a,b,c,d,e,X[30]);	GG(e,a,b,c,d,X[31]);
	GG(d,e,a,b,c,X[32]);	GG(c,d,e,a,b,X[33]);
	GG(b,c,d,e,a,X[34]);	GG(a,b,c,d,e,X[35]);
	GG(e,a,b,c,d,X[36]);	GG(d,e,a,b,c,X[37]);
	GG(c,d,e,a,b,X[38]);	GG(b,c,d,e,a,X[39]);

	//	3rd round
	K = 0x8F1BBCDC;
	HH(a,b,c,d,e,X[40]);	HH(e,a,b,c,d,X[41]);
	HH(d,e,a,b,c,X[42]);	HH(c,d,e,a,b,X[43]);
	HH(b,c,d,e,a,X[44]);	HH(a,b,c,d,e,X[45]);
	HH(e,a,b,c,d,X[46]);	HH(d,e,a,b,c,X[47]);
	HH(c,d,e,a,b,X[48]);	HH(b,c,d,e,a,X[49]);
	HH(a,b,c,d,e,X[50]);	HH(e,a,b,c,d,X[51]);
	HH(d,e,a,b,c,X[52]);	HH(c,d,e,a,b,X[53]);
	HH(b,c,d,e,a,X[54]);	HH(a,b,c,d,e,X[55]);
	HH(e,a,b,c,d,X[56]);	HH(d,e,a,b,c,X[57]);
	HH(c,d,e,a,b,X[58]);	HH(b,c,d,e,a,X[59]);

	//	4th round
	K = 0xCA62C1D6;
	II(a,b,c,d,e,X[60]);	II(e,a,b,c,d,X[61]);
	II(d,e,a,b,c,X[62]);	II(c,d,e,a,b,X[63]);
	II(b,c,d,e,a,X[64]);	II(a,b,c,d,e,X[65]);
	II(e,a,b,c,d,X[66]);	II(d,e,a,b,c,X[67]);
	II(c,d,e,a,b,X[68]);	II(b,c,d,e,a,X[69]);
	II(a,b,c,d,e,X[70]);	II(e,a,b,c,d,X[71]);
	II(d,e,a,b,c,X[72]);	II(c,d,e,a,b,X[73]);
	II(b,c,d,e,a,X[74]);	II(a,b,c,d,e,X[75]);
	II(e,a,b,c,d,X[76]);	II(d,e,a,b,c,X[77]);
	II(c,d,e,a,b,X[78]);	II(b,c,d,e,a,X[79]);

	//	chaining variables update
	DigestValue[0] += a;
	DigestValue[1] += b;
	DigestValue[2] += c;
	DigestValue[3] += d;
	DigestValue[4] += e;
}

/*************** Function *************************************************
*	
*/
void	SHA1_Init(
		SHA1_ALG_INFO	*AlgInfo)
{
	//
	AlgInfo->ChainVar[0] = 0x67452301;
	AlgInfo->ChainVar[1] = 0xefcdab89;
	AlgInfo->ChainVar[2] = 0x98badcfe;
	AlgInfo->ChainVar[3] = 0x10325476;
	AlgInfo->ChainVar[4] = 0xC3D2E1F0;

	//
	AlgInfo->Count[0] = AlgInfo->Count[1] = 0;
}

/*************** Function *************************************************
*	
*/
void	SHA1_Update(
		SHA1_ALG_INFO	*AlgInfo,
		BYTE			*Message,		//	input Message
		DWORD			MessageLen)		//	in BYTEs
{
	DWORD		RemainedLen, PartLen;

	//	Compute the number of hashed bytes mod SHA1_DIGEST_BLOCKLEN
	RemainedLen = (AlgInfo->Count[0] >> 3) % SHA1_DIGEST_BLOCKLEN;
	//	compute the number of bytes that can be filled up
	PartLen = SHA1_DIGEST_BLOCKLEN - RemainedLen;

	//	Update Count (number of toatl data bits)
	if( (AlgInfo->Count[0] += (MessageLen << 3)) < AlgInfo->Count[0] )
		AlgInfo->Count[1]++;
	AlgInfo->Count[1] += (MessageLen >> 29);

	//	핵심 updtae 부분
	if( MessageLen>=PartLen ) {
		memcpy(AlgInfo->Buffer+RemainedLen, Message, (int)PartLen);
		SHA1_Transform((DWORD *)AlgInfo->Buffer, AlgInfo->ChainVar);

		Message += PartLen;
		MessageLen -= PartLen;
		RemainedLen = 0;

		while( MessageLen>=SHA1_DIGEST_BLOCKLEN ) {
			if( (((int)Message)%4)==0 ) {	//	Speed up technique
				SHA1_Transform((DWORD *)Message, AlgInfo->ChainVar);
			}
			else {
				memcpy((BYTE *)AlgInfo->Buffer, Message, (int)SHA1_DIGEST_BLOCKLEN);
				SHA1_Transform((DWORD *)AlgInfo->Buffer, AlgInfo->ChainVar);
			}
			Message += SHA1_DIGEST_BLOCKLEN;
			MessageLen -= SHA1_DIGEST_BLOCKLEN;
		}
	}

	//	Buffer remaining input
	memcpy((BYTE *)AlgInfo->Buffer+RemainedLen, Message, (int)MessageLen);
}

/*************** Function *************************************************
*	
*/
void	SHA1_Final(
		SHA1_ALG_INFO	*AlgInfo,
		BYTE			*Digest)	//	output Hash Value
{
	DWORD		i, dwIndex, CountL, CountH;

	//	마지박 블록 처리
	CountL = AlgInfo->Count[0];
	CountH = AlgInfo->Count[1];
	dwIndex = (CountL >> 3) % SHA1_DIGEST_BLOCKLEN;
	AlgInfo->Buffer[dwIndex++] = 0x80;

	if(dwIndex>SHA1_DIGEST_BLOCKLEN-8) {
		memset((BYTE *)AlgInfo->Buffer + dwIndex, 0, (int)(SHA1_DIGEST_BLOCKLEN-dwIndex));

		SHA1_Transform((DWORD *)AlgInfo->Buffer, AlgInfo->ChainVar);

		memset((BYTE *)AlgInfo->Buffer, 0, (int)SHA1_DIGEST_BLOCKLEN-8);
	}
	else
		memset((BYTE *)AlgInfo->Buffer+dwIndex, 0, (int)(SHA1_DIGEST_BLOCKLEN-dwIndex-8));

	//	출력
#if defined(LITTLE_ENDIAN)
	CountL = ENDIAN_REVERSE_DWORD(CountL);
	CountH = ENDIAN_REVERSE_DWORD(CountH);
#endif
	((DWORD *)AlgInfo->Buffer)[SHA1_DIGEST_BLOCKLEN/4-2] = CountH;
	((DWORD *)AlgInfo->Buffer)[SHA1_DIGEST_BLOCKLEN/4-1] = CountL;

	SHA1_Transform((DWORD *)AlgInfo->Buffer, AlgInfo->ChainVar);

	for( i=0; i<SHA1_DIGEST_VALUELEN; i+=4)
		BIG_D2B((AlgInfo->ChainVar)[i/4], &(Digest[i]));
}

/*************** END OF FILE **********************************************/
