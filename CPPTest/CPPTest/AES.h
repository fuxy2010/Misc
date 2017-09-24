// **********************************************************************
// 作者: 傅佑铭
// 版本: 1.0
// 日期: 2011-01 ~ 2011-11
// 修改历史记录: 
// 日期, 作者, 变更内容
// **********************************************************************
#ifndef _AES_H_       
#define _AES_H_

#ifndef USE_AES
//#define USE_AES
#endif

#ifndef AES_KEY_LENGTH
#define AES_KEY_LENGTH	128
#endif

//#ifndef AES_KEY
//#define AES_KEY	"whu_nercms_2015@"
//#endif
extern unsigned char _aes_key[];

struct AESWORD
{
	unsigned char column[4];
};

class CAES
{
public:
	CAES(unsigned char* inputkey);// , int key_bit_length);
	virtual ~CAES();

public:
	void encrypt(unsigned char* data, int length);
	void decrypt(unsigned char* data, int length);

private:
	int Nr;
	int Nk;
	int Nb;
	AESWORD state[4];//AESWORD* state;
	unsigned char key[AES_KEY_LENGTH / 8];//unsigned char* key;
	AESWORD roundkey[4 * (AES_KEY_LENGTH / 32 + 6 + 1)];//AESWORD* roundkey;

private:
	void InitialState(unsigned char* input);
	void InvCipher();
	void Cipher();

protected:
	void AddRoundKey( int round );
	void InvMixColumns();
	void MixColumns();
	void InvShiftRows();
	void ShiftRows();
	void KeyExpansion();
	void SubBytes();
	void InvSubBytes();
};

#endif//_AES_H_
