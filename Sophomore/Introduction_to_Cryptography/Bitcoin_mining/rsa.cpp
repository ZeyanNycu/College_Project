#pragma comment(lib,"cryptlib.lib")
#include <iostream>
#include <fstream>
#include <iomanip>
#include <Cryptopp/aes.h>
#include <Cryptopp/hex.h>
#include <Cryptopp/modes.h>
#include <Cryptopp/filters.h>
#include<CryptoPP/rsa.h>
#include <CryptoPP/osrng.h>

using namespace CryptoPP;
using namespace std;

const Integer enc(const long e, const string nStr, const string mStr);
const string dec(const long e, const Integer n, const Integer d, const Integer c);
const pair<const string, const int> dec(const string dStr, const string nStr, const string cStr);

int main() {
	cin.tie(0);
	cin.sync_with_stdio(0);
	ofstream out;
	out.open("out.txt");

	const Integer c1 = enc(0x11, "0x04823f9fe38141d93f1244be161b20f", "Hello World!");
	const Integer c2 = enc(0x10001, "0x9711ea5183d50d6a91114f1d7574cd52621b35499b4d3563ec95406a994099c9", "RSA is public key.");
	out << "Ciphertext 1 = " << hex << c1 << '\n';
	out << "Ciphertext 2 = " << hex << c2 << '\n';

	//Integer n("0xc45350fa19fa8d93"), e("0x11");

	//string s = "0x454a950c5bc";
	//string temp = "baa41";
	//s = s + temp;
	//auto p = dec(s,
	//	"0xc45350fa19fa8d93",
	//	"0xa4a59490b843eea0");
	////dnc
	//string m = p.first; int e = p.second;
	//out << "Message = " << m << "\nPublic key = 0x" << hex << e << '\n';

	int isbreak = 0;
	int time = 0;
	for (int i0 = 0; i0 < 16; i0++) {
		for (int i1 = 0; i1 < 16; i1++) {
			for (int i2 = 0; i2 < 16; i2++) {
				for (int i3 = 0; i3 < 16; i3++) {
					for (int i4 = 0; i4 < 16; i4++) {
						string s = "0x53a0a95b089cf23adb5cc73f07";
						string temp = "00000";
						if (i4 < 10) temp[0] = char(i4 + 48);
						else temp[0] = char(i4 + 87);
						if (i3 < 10) temp[1] = char(i3 + 48);
						else temp[1] = char(i3 + 87);
						if (i2 < 10) temp[2] = char(i2 + 48);
						else temp[2] = char(i2 + 87);
						if (i1 < 10) temp[3] = char(i1 + 48);
						else temp[3] = char(i1 + 87);
						if (i0 < 10) temp[4] = char(i0 + 48);
						else temp[4] = char(i0 + 87);
						s = s + temp;
						//if(time%10000==0) cout << s << endl;
						time++;
						auto p = dec(s,
							"0xc4b361851de35f080d3ca7352cbf372d",
							"0xa02d51d0e87efe1defc19f3ee899c31d");
						string m = p.first; int e = p.second;
						if (m != "") {
							//cout<< "Message = " << m << "\nPublic key = 0x" << hex << e << '\n';
							out << "Message = " << m << "\nPrivate key = " << s << '\n';
							isbreak = 1;
							break;
						}
						//RSA::PrivateKey rsaPrivate(s);
					}
					if (isbreak == 1) break;
				}
				if (isbreak == 1) break;
			}
			if (isbreak == 1) break;
		}
		if (isbreak == 1) break;
	}


	//Integer d("0x454a950c5bcbaa41");
	//
	////RSA::PrivateKey rsaPrivate;
	////RSA::PublicKey rsaPublic();
	//auto p = dec("0x12e6a85100b889c9905a939b274a91bc57ca85d52e6c464fb455c86a29d63c89",
	//	"0xd6361e40b2d619970ead338912a273adb75a4ce21356304834753fe94e6de24b",
	//	"0xa1676afd68a2fc67dac32c633600b76fa90aca9f9cca5201490a20c8b01a061a");
	//string m = p.first; int e = p.second;
	////assert(m != "" && "Decryption failed");

	//out << "Message = " << m << "\nPublic key = 0x" << hex << e << '\n';

	return 0;
}

const Integer enc(const long e, const string nStr, const string mStr) {
	const Integer n(nStr.c_str());
	const Integer m((const byte*)mStr.data(), mStr.size());

	RSA::PublicKey pubKey;
	pubKey.Initialize(n, e);

	const Integer c = pubKey.ApplyFunction(m);

	return c;
}

const string dec(const long e, const Integer d, const Integer n, const Integer c) {
	AutoSeededRandomPool prng;
	string msg;

	RSA::PrivateKey priKey;
	try {
		priKey.Initialize(n, e, d);
	}
	catch (InvalidArgument e) {
		return "";
	}

	const Integer m = priKey.CalculateInverse(prng, c);
	msg.resize(m.MinEncodedSize());
	m.Encode((byte*)msg.data(), msg.size());

	return msg;
}

const pair<const string, const int> dec(const string dStr, const string nStr, const string cStr) {
	const Integer d(dStr.c_str());
	const Integer n(nStr.c_str());
	const Integer c(cStr.c_str());

	//for (int e = 3; e < 1 << 30; e += e - 1) {
	int e = 7477;
	const string m = dec(e, d, n, c);
	if (m != "") return { m, e };
	//}

	return { "", 0 };
}
