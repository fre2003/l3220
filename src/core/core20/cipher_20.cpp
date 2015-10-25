#include "StdAfx.h"
#include <IEncryption.h>
#include <WinCrypt.h>

#pragma comment (lib, "AdvAPI32.Lib")

// This Crypt routine uses an algorithm created by Rodney Thayer. 
// This algorithm can use keys of various sizes. With a 40-bit key (5 characters) 
// it can be freely exported from the U.S. 
// The cipher is considered robust with 128 bits of key material but can use up to 2048 bits. 
// It is compatible with RSA?s RC4 algorithm.

/*
The only moral here is that we are generating a 256 bit key given from the user. 
To encrypt we always pick a random byte from the key and encrypt the text with that key. 
If we have a large pattern of repeating text with this algorithm the key repeat interval
 (distance between n repetitions) will be approximately of 21700 (2 to the 1700th power). 
For very good security I recommend using 128 bits (16 characters) of key material. 
If you want optimal security you could use 2048 bits (256 characters) of key material (but this is usually considered an overkill).
This routine is constructed from a draft taken from Internet Engineering Task Force 
(IETF) at http://www.ietf.cnri.reston.va.us/home.htm
I have tested the routine on a 27 MB file to test that it produce valid decrypted output 
from input and get the valid results. Using this routine it is fairly simple.
*/

U32 CEncryptDecrypt_i::EncryptDecryptDataS(CONST PBYTE inp, u32 inplen, BYTE* outp, CONST BYTE* key, u8 ucKeyLen)
{
  //we will consider size of sbox 256 bytes
  //(extra byte are only to prevent any mishep just in case)
  BYTE Sbox[257], Sbox2[257];
  unsigned long i, j, t, x;
  
  // check the valid input
  if(inp == NULL || outp == NULL || inplen == 0)
  {
    return ERR_INVALID_PARAM_I;
  }
    
  //this unsecured key is to be used only when there is no input key from user
  static const TCHAR  OurUnSecuredKey[] = "123456" ;
  static const int OurKeyLen = _tcslen(OurUnSecuredKey);    
  BYTE temp , k;
  
  i = j = 0;
  k = 0;
  t = x = 0;
  temp = 0;
  
  //always initialize the arrays with zero
  ZeroMemory(Sbox, sizeof(Sbox));
  ZeroMemory(Sbox2, sizeof(Sbox2));
  
  //initialize sbox i
  for(i = 0; i < 256U; i++)
  {
    Sbox[i] = (BYTE)i;
  }
  
  j = 0;

  //whether user has sent any inpur key
  if(ucKeyLen && key)
  {
    //initialize the sbox2 with user key
    for(i = 0; i < 256U ; i++)
    {
      if(j == ucKeyLen)
      {
        j = 0;
      }
      Sbox2[i] = key[j++];
    }    
  }
  else
  {
    //initialize the sbox2 with our key
    for(i = 0; i < 256U ; i++)
    {
      if(j == (unsigned long)OurKeyLen)
      {
        j = 0;
      }
      Sbox2[i] = OurUnSecuredKey[j++];
    }
  }
  
  j = 0 ; //Initialize j
  
  //scramble sbox1 with sbox2
  for(i = 0; i < 256; i++)
  {
    j       = (j + (unsigned long) Sbox[i] + (unsigned long) Sbox2[i]) % 256U ;
    temp    =  Sbox[i];                    
    Sbox[i] = Sbox[j];
    Sbox[j] =  temp;
  }
  
  i = j = 0;
  for(x = 0; x < inplen; x++)
  {
    //increment i
    i = (i + 1U) % 256U;

    //increment j
    j = (j + (unsigned long) Sbox[i]) % 256U;
    
    //Scramble SBox #1 further so encryption routine will
    //will repeat itself at great interval
    temp    = Sbox[i];
    Sbox[i] = Sbox[j];
    Sbox[j] = temp;
    
    //Get ready to create pseudo random  byte for encryption key
    t = ((unsigned long) Sbox[i] + (unsigned long) Sbox[j]) %  256U ;
    
    //get the random byte
    k = Sbox[t];
    
    //xor with the data and done
    outp[x] = (inp[x] ^ k);
  }
  
  return ERR_OK_I;
}


#include "Base64Coder.h"

U32 CEncryptDecrypt_i::EncryptTextS(CONST PBYTE pData, u32 ulDataLen,
                                    CONST PBYTE pPassword, u8 ucKeylen, 
                                    OUT TCHAR*& pszOutputEncText)
{
  Base64Coder b64;
  
  if(pData == NULL || ulDataLen == 0)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  BYTE* pOutBuffer = new BYTE[ulDataLen]; 
  if(pOutBuffer == NULL)
  {
    return ERR_NO_MORE_MEMORY_I;
  }
  
  pOutBuffer[0] = 0;
  
  EncryptDecryptDataS(pData, ulDataLen, pOutBuffer, (CONST BYTE*)pPassword, ucKeylen);
  
  // convert the binary to a string
  b64.Encode(pOutBuffer, ulDataLen);
  
  delete [] pOutBuffer;
  
  LPCTSTR pEncodedDataBuffer = b64.EncodedMessage();
  u32 len = lstrlen(pEncodedDataBuffer);
  
  pszOutputEncText = new TCHAR[len+1+8];
  if(pOutBuffer == NULL)
  {
    return ERR_NO_MORE_MEMORY_I;
  }
  lstrcpy(pszOutputEncText, pEncodedDataBuffer);
  
  TCHAR szEncryptSize[8+1] = {0,};
  wsprintf(szEncryptSize, "%08d", ulDataLen);
  lstrcpy(&pszOutputEncText[len], szEncryptSize);
  
  return 0;
}


U32 CEncryptDecrypt_i::DecryptTextS(CONST TCHAR* pszData, u32 ulszDataLen,
                                    CONST PBYTE pPassword, u8 ucKeylen,  
                                    OUT TCHAR*& pszDecBuffer)
{
  Base64Coder b64;
  
  if(ulszDataLen <=8)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  if(pszData == NULL)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  U32 ulEncryptSize = 0;
  TCHAR szEncryptSize[8+1] = {0,};
  
  lstrcpy(szEncryptSize, &pszData[ulszDataLen-8]);   
  
  ulEncryptSize = _ttol(szEncryptSize);
  
  // convert the input string back to binary
  b64.Decode((PBYTE)pszData, ulszDataLen-8);
  
  DWORD nReqBufLen = 0;
  PBYTE pDecodedDataBuffer = b64.DecodedMessage(nReqBufLen);
  
  pszDecBuffer = (TCHAR*)(new BYTE[ulEncryptSize+1]);
  
  EncryptDecryptDataS( pDecodedDataBuffer, ulEncryptSize, (PBYTE)pszDecBuffer, pPassword, ucKeylen);
  pszDecBuffer[ulEncryptSize] = 0;
  
  return 0;
}

BOOL CEncryptDecrypt_i::FreeBufferS(IN TCHAR*& pData)
{
  if(pData)
  {
    delete pData;
    pData = NULL;
  }
  
  return TRUE;
}


CEncryptDecrypt_i::CEncryptDecrypt_i()
: m_pEncryptor(NULL) 
{
	
}

CEncryptDecrypt_i::~CEncryptDecrypt_i()
{
	ReleaseEncryptionInterface(m_pEncryptor);
}

BOOL CEncryptDecrypt_i::RsaFreeBuffer(IN CHAR*& pData)
{
	m_pEncryptor->FreeBuffer((CHAR*)pData);
  
  return TRUE;
}


BOOL CEncryptDecrypt_i::InitEncryptor()
{
	if (m_pEncryptor)
		return TRUE;
	
  CHAR szEncryptionFile[MAX_PATH];
  
#ifdef UNICODE
  CHAR mbszfile[MAX_PATH];
  CStringUtl::wcs2mbsz(mbszfile, sizeof(mbszfile), CFilePath_i::GetAppPath());
  wsprintfA(szEncryptionFile, "%s\\EncryptDecrypt.dll", mbszfile);
#else
  wsprintfA(szEncryptionFile, "%s\\EncryptDecrypt.dll", CFilePath_i::GetAppPath());
#endif
  
	m_pEncryptor = CreateEncryptionInterface(szEncryptionFile);
	
	return (m_pEncryptor != NULL);
}


BOOL CEncryptDecrypt_i::RsaGenerate(DWORD dwKeyLength, // Key length in bits
                              IN CHAR szSeed[512],  // Random Seed 
                              IN CHAR* pszPrivFilename, // to store priv key
                              IN CHAR* pszPubFilename)  // to store pub key 
{
  if (!InitEncryptor())
    return FALSE;
   
  return m_pEncryptor->RsaGenerate(dwKeyLength, pszPrivFilename, pszPubFilename, szSeed);
}


BOOL CEncryptDecrypt_i::RsaSignatureFile(CHAR* pszPrivFilename, // to store priv key
                                   CHAR* pszMessageFilename, // message to encrypt in file
                                   CHAR* pszSignatureFilename // to store signatureFilename
                                   )
{
  if (!InitEncryptor())
    return FALSE;
  
  return m_pEncryptor->RsaSignatureFile(pszPrivFilename, pszMessageFilename, pszSignatureFilename);
}


// verify the signature file is valid? 
BOOL CEncryptDecrypt_i::RsaSignatureVerify(CHAR* pszPubFilename, // pub key file 
                                           CHAR* pszMessageFilename, // message to encrypt in file
                                           CHAR* pszSignatureFilename // to store signatureFilename
                                           )
{
  if (!InitEncryptor())
    return FALSE;
  
  return m_pEncryptor->RsaSignatureVerify(pszPubFilename, pszMessageFilename, pszSignatureFilename);
}

BOOL CEncryptDecrypt_i::RsaEncryptString(CHAR* pszPubFilename, 
                                         CHAR* pszSeed, 
                                         CHAR* pszMessage, 
                                         CHAR*&  rCipherText)
{
  if (!InitEncryptor())
    return FALSE;

  return m_pEncryptor->RsaEncryptString(pszPubFilename, pszSeed, pszMessage, rCipherText);
}


BOOL CEncryptDecrypt_i::RsaDecryptString(CHAR* pszPrivFilename, 
                                         CHAR* pszCiphertext, 
                                         CHAR*&  rDecryptedText)
{
  if (!InitEncryptor())
    return FALSE;
  
  return m_pEncryptor->RsaDecryptString(pszPrivFilename,
    pszCiphertext, 
    rDecryptedText);
}
