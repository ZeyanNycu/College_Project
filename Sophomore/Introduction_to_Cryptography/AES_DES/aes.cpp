#pragma comment(lib,"cryptlib.lib")
#include <iostream>
#include <fstream>
#include <iomanip>
#include <Cryptopp/aes.h>
#include <Cryptopp/hex.h>
#include <Cryptopp/modes.h>
#include <Cryptopp/filters.h>
using namespace CryptoPP;
using namespace std;
std::string enc_ecb(std::string plaintext, CryptoPP::byte key[], std::string padding) {

    std::string ciphertext;

    CryptoPP::ECB_Mode< CryptoPP::AES >::Encryption eecb;
    eecb.SetKey(key, CryptoPP::AES::DEFAULT_KEYLENGTH);

    if (padding == "zero") {
        CryptoPP::StringSource en(plaintext, true, new CryptoPP::StreamTransformationFilter(eecb, new CryptoPP::StringSink(ciphertext), CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING));
        return ciphertext;
    }
    else {
        CryptoPP::StringSource en(plaintext, true, new CryptoPP::StreamTransformationFilter(eecb, new CryptoPP::StringSink(ciphertext), CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING));
        return ciphertext;
    }

}

std::string enc_cbc(std::string plaintext, CryptoPP::byte key[], CryptoPP::byte iv[], std::string padding) {

    std::string ciphertext;

    CryptoPP::CBC_Mode< CryptoPP::AES >::Encryption ecbc;
    ecbc.SetKeyWithIV(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);

    if (padding == "zero") {
        CryptoPP::StringSource en(plaintext, true, new CryptoPP::StreamTransformationFilter(ecbc, new CryptoPP::StringSink(ciphertext), CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING));
        return ciphertext;
    }
    else {
        CryptoPP::StringSource en(plaintext, true, new CryptoPP::StreamTransformationFilter(ecbc, new CryptoPP::StringSink(ciphertext), CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING));
        return ciphertext;
    }
}
std::string enc_cfb(std::string plaintext, CryptoPP::byte key[], CryptoPP::byte iv[])
{
    std::string ciphertext;
   
    CryptoPP::CFB_Mode< CryptoPP::AES >::Encryption ecbc;
    ecbc.SetKeyWithIV(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);
    //cout << ecbc.OptimalBlockSize() << endl;
    CryptoPP::StringSource en(plaintext, true, new CryptoPP::StreamTransformationFilter(ecbc, new CryptoPP::StringSink(ciphertext)));
    return ciphertext;
}

void printHex(std::string ciphertext) {
    
    for (int i = 0; i < ciphertext.size(); i++) {
        cout <<std::setfill('0') << std::setw(2) << std::hex << (0xFF & static_cast<CryptoPP::byte>(ciphertext[i]));
    }
    cout<< std::endl;
}
string convert_to_hex(string ciphertext)
{
    int a=0x00;
    //string str_a;
    fstream output;
    output.open("aes-out.txt",ios::app);
    string ans;
    for (int i = 0; i < ciphertext.size(); i++)
    {
        a= (0xFF & static_cast<CryptoPP::byte>(ciphertext[i])) ;
        output <<hex<< a<<" ";
       
    }
    output << endl;
    output.close();
    return ans;
}


int main(int argc, char* argv[]) {

    // Homework
    CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE],iv_1[CryptoPP::AES::BLOCKSIZE],test[CryptoPP::AES::BLOCKSIZE], temp[CryptoPP::AES::BLOCKSIZE];
    memset(key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
    memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);
    std::string s = "keyis84932731830";
    std::string plaintext = "AES is the US block cipher standard.";//"Hello World!";
    string sub_plaintext;
    string ans;
    for (int i = 0; i < CryptoPP::AES::BLOCKSIZE; i++)
    {
        iv[i] = '0'; 
        test[i] = '0';
    }
    for (int i = 0; i < CryptoPP::AES::DEFAULT_KEYLENGTH; i++)
    {
        key[i] = s[i]; 
    }
    for (int i = 0; i < CryptoPP::AES::DEFAULT_KEYLENGTH; i++)
    {
        iv_1[i] = '9';
    }
    sub_plaintext = plaintext.substr(0,4);
    string a;
    int size=0;
    a = enc_cfb(sub_plaintext,key,test);
    while ((size+4)<plaintext.size())
    {
        size = 4 + size;
        sub_plaintext = plaintext.substr(size,4);
        for (int i = 0; i < 16; i++)
        {
            temp[i] = test[i];
        }
        for (int i = 0; i < 12; i++)
        {
            test[i] = temp[i + 4];
        }
        for (int i = 12; i < 16; i++)
        {
            test[i] = a[i - 12+size-4];
        }
        a += enc_cfb(sub_plaintext,key,test);
    }
    if (size != plaintext.size())
    {
        sub_plaintext = plaintext.substr(size, plaintext.size() - size);
        for (int i = 0; i < 16; i++)
        {
            temp[i] = test[i];
        }
        for (int i = 0; i < 12; i++)
        {
            test[i] = temp[i + 4];
        }
        for (int i = 12; i < 16; i++)
        {
            test[i] = a[i - 12 + size - 4];
        }
        a += enc_cfb(sub_plaintext, key, test);
    }
    string want_ans = a.substr(0,plaintext.size());
    
    printHex(want_ans);
    convert_to_hex(want_ans) ;
   printHex(enc_cbc(plaintext, key, iv, "zero"));
   //cout << convert_to_hex(enc_cbc(plaintext, key, iv, "zero")) << endl;
   convert_to_hex(enc_cbc(plaintext, key, iv, "zero"));
    printHex(enc_cbc(plaintext, key, iv_1,"PKCS"));
     convert_to_hex(enc_cbc(plaintext, key, iv_1, "PKCS"));
    printHex(enc_ecb(plaintext, key,"PKCS"));
    convert_to_hex(enc_ecb(plaintext, key, "PKCS"));
    return 0;
}

