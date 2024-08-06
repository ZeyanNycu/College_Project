#include<iostream>
#include<string>
#include<bitset>
#include<cmath>
#include<fstream>
using namespace std;
bool once=true;
int Ip[64]={58,50,42,34,26,18,10,2,
60,52,44, 36, 28, 20, 12, 4,
62, 54, 46, 38, 30, 22, 14, 6,
64, 56, 48, 40, 32, 24, 16, 8,
57, 49, 41, 33, 25, 17, 9, 1,
59, 51, 43, 35, 27, 19, 11, 3,
61, 53, 45, 37, 29, 21, 13, 5,
63, 55, 47, 39, 31, 23, 15, 7};
int S_box[8][64]={{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
                0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
                4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
                15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}, 
                {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
                3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
                0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
                13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9
				},
				{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
                13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
                13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
                1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12
				 } 
                ,
                {
                	7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
                13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
                10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
                3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14
				}
                ,
                {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
                14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
                4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
                11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3
				}
                ,
                {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
                10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
                9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
                4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13
				}
                ,
                {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
                13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
                1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
                6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12
				}
                ,
                {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
                1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
                7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
                2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
				}
				};
int step[16]={1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};
int PC_1_l[]={57, 49, 41, 33, 25, 17, 9, 
            1, 58, 50, 42, 34, 26, 18, 
            10, 2, 59, 51, 43, 35, 27, 
            19, 11, 3, 60, 52, 44, 36};
int PC_1_r[]={63, 55, 47, 39, 31, 23, 15, 
            7, 62, 54, 46, 38, 30, 22, 
            14, 6, 61, 53, 45, 37, 29, 
            21, 13, 5, 28, 20, 12, 4};
int PC_2[]={14, 17, 11, 24, 1, 5, 
            3, 28, 15, 6, 21, 10,
            23, 19, 12, 4, 26, 8,
            16, 7, 27, 20, 13, 2,
            41, 52, 31, 37, 47, 55,
            30, 40, 51, 45, 33, 48,
            44, 49, 39, 56, 34, 53,
            46, 42, 50, 36, 29, 32};
int Fp[]={40, 8, 48, 16, 56, 24, 64, 32,
            39, 7, 47, 15, 55, 23, 63, 31,
            38, 6, 46, 14, 54, 22, 62, 30,
            37, 5, 45, 13, 53, 21, 61, 29,
            36, 4, 44, 12, 52, 20, 60, 28,
            35, 3, 43, 11, 51, 19, 59, 27,
            34, 2, 42, 10, 50, 18, 58, 26,
            33, 1, 41, 9, 49, 17, 57, 25};
int P[]={16 ,7, 20, 21, 29, 12, 28, 17,
1, 15, 23, 26, 5, 18, 31, 10,
2, 8, 24, 14, 32, 27, 3, 9,
19, 13, 30, 6, 22, 11, 4, 25};
string r_plain;
string toBinary(int n,bool sign)
{
    string r;
    string r_tr;
    while (n != 0){
        r = ( n % 2 == 0 ? "0" : "1" )+r;
        n /= 2;
    }
    // let the length be 8
    if(sign)
    {
    	while(r.size()<8)
		{
			r_tr="0"+r;
			r=r_tr;
		}
	}
	// let the length be 4
    else
    {
    	while(r.size()<4)
    	{
    		r_tr="0"+r;
    		r=r_tr;
		}
	}
    return r;
}
void Initper(string& plaintext)
{
	// do the  initial permutation
	string copy=plaintext;
	for(int i=0;i<64;i++)
	{
		plaintext[i]=copy[Ip[i]-1];
	}
	return;
}
void E(string& pl_sub,int times)
{
	// expand the string
	int front=4*times-1;
	if(front<0)
	{
		front=31;
	}
	int back=4*times+3+1;
	if(back>31)
	{
		back=0;
	}
	pl_sub=r_plain[front]+pl_sub;
	pl_sub=pl_sub+r_plain[back];
}
void S(string& pl_sub,int times)
{
	//let not linear result
	string copy=pl_sub;
	int row=0;
	int col=0;
	float a=0;
	if(pl_sub[0]=='1')
	{
		row+=2;
	}
	if(pl_sub[5]=='1')
	{
		row+=1;
	}
	for(int i=4;i>=1;i--)
	{
		if(pl_sub[i]=='1')
		{
			col+=pow(2,a);
		}
		a++;
	}
	pl_sub=toBinary(S_box[times][row*16+col],0);
}
string generate_subkey(string& key,int turn)
{
	// get the sub_key
	string l_sub;
	string r_sub;
	string copy_l;
	string copy_r;
	if(once)
	{
		copy_l=key.substr(0,28);
		copy_r=key.substr(0,28);
		for(int i=0;i<28;i++)
		{
			copy_l[i]=key[PC_1_l[i]-1];
			copy_r[i]=key[PC_1_r[i]-1];
		}
		once=false;
	}
	else
	{
		copy_l=key.substr(0,28);
		copy_r=key.substr(28,28);
	}
	l_sub+=copy_l.substr(turn,copy_l.size()-turn);
	r_sub+=copy_r.substr(turn,copy_r.size()-turn);
	l_sub+=copy_l.substr(0,turn);
	r_sub+=copy_r.substr(0,turn);
	l_sub+=r_sub;
	copy_l.clear();
	copy_l=l_sub;
	l_sub.clear();
	for(int i=0;i<48;i++)
	{
		l_sub+=copy_l[PC_2[i]-1];
	}
	key=copy_l;
	return l_sub;
}

string f(string pl,string key,int turn)
{
	string pl_sub[8];
	string pl_ok;
	for(int i=0;i<8;i++)
	{
		pl_sub[i]=pl.substr(4*i,4);
	}
	for(int i=0;i<8;i++)
	{
		E(pl_sub[i],i);
		pl_ok+=pl_sub[i];
		pl_sub[i]="";
	}
	string res;
	for(int i=0;i<48;i++)
	{
		(key[i]==pl_ok[i])?(res+="0"):(res+="1");
	}
	pl_ok="";
	for(int i=0;i<8;i++)
	{
		pl_sub[i]=res.substr(6*i,6);
		S(pl_sub[i],i);
		pl_ok+=pl_sub[i];
	}
	string ans;
	for(int i=0;i<32;i++)
	{
		ans+=pl_ok[P[i]-1];
	}
	return ans;
}
string convert_to_hex(string ans)
{
	string a[16];
	string want;
	for(int i=0;i<16;i++)
	{
		a[i]=ans.substr(i*4,4);
	}
	for(int i=0;i<16;i++)
	{
		if(a[i]=="0000")
		{
			want+="0";
		}
		else if(a[i]=="0001")
		{
			want+="1";
		}
		else if(a[i]=="0010")
		{
			want+="2";
		}
		else if(a[i]=="0011")
		{
			want+="3";
		}
		else if(a[i]=="0100")
		{
			want+="4";
		}
		else if(a[i]=="0101")
		{
			want+="5";
		}
		else if(a[i]=="0110")
		{
			want+="6";
		}
		else if(a[i]=="0111")
		{
			want+="7";
		}
		else if(a[i]=="1000")
		{
			want+="8";
		}
		else if(a[i]=="1001")
		{
			want+="9";
		}
		else if(a[i]=="1010")
		{
			want+="A";
		}
		else if(a[i]=="1011")
		{
			want+="B";
		}
		else if(a[i]=="1100")
		{
			want+="C";
		}
		else if(a[i]=="1101")
		{
			want+="D";
		}
		else if(a[i]=="1110")
		{
			want+="E";
		}
		else if(a[i]=="1111")
		{
			want+="F";
		}
	}
	return want;
}
string convert_to_binary(string ans)
{
	string a;
	string want;
	a=ans;
	for(int i=0;i<16;i++)
	{
		if(a[i]=='0')
		{
			want+="0000";
		}
		else if(a[i]=='1')
		{
			want+="0001";
		}
		else if(a[i]=='2')
		{
			want+="0010";
		}
		else if(a[i]=='3')
		{
			want+="0011";
		}
		else if(a[i]=='4')
		{
			want+="0100";
		}
		else if(a[i]=='5')
		{
			want+="0101";
		}
		else if(a[i]=='6')
		{
			want+="0110";
		}
		else if(a[i]=='7')
		{
			want+="0111";
		}
		else if(a[i]=='8')
		{
			want+="1000";
		}
		else if(a[i]=='9')
		{
			want+="1001";
		}
		else if(a[i]=='A')
		{
			want+="1010";
		}
		else if(a[i]=='B')
		{
			want+="1011";
		}
		else if(a[i]=='C')
		{
			want+="1100";
		}
		else if(a[i]=='D')
		{
			want+="1101";
		}
		else if(a[i]=='E')
		{
			want+="1110";
		}
		else if(a[i]=='F')
		{
			want+="1111";
		}
	}
	return want;
}
string convert_to_string(string ans)
{
	string a;
	string want;
	for(int i=0;i<8;i++)
	{
		float use=0;
		int value=0;
		a=ans.substr(8*i,8);
		for(int j=7;j>=0;j--)
		{
			if(a[j]=='1')
			{
				value+=pow(2,use);
			}
			use++;
		}
		want+=(char (value));
	}
	return want;
}
int main()
{
	string plaintext;
	string main_key;
	string buff;
	string plaintext_be;
	string main_key_be;
	string f_res;
	string l_plain;
	string temp;
	string result;
	string sub_key;
	string ans;
	string buffer[16];
	bool way;
	ifstream input;
	ofstream output; 
	input.open("DES-Key-Ciphertext.txt");
	output.open("des-out.txt");
	for(int ii=0;ii<5;ii++)
	{
		once=true;
		way=0;
		r_plain.clear();
		plaintext.clear();
		main_key.clear();
		buff.clear();
		plaintext_be.clear();
		main_key_be.clear();
		f_res.clear();
		l_plain.clear();
		temp.clear();
		result.clear();
		sub_key.clear();
		ans.clear();
		if(way)
		{

		//cout<<"input the plaintext"<<endl;
		//plaintext="Hi Mary!";
		getline(cin,plaintext,'.');
		//cout<<"input the main_key"<<endl;
		//cin>>main_key;
		main_key="security";
		plaintext_be.clear();
		main_key_be.clear();
		for(int i=0;i<8;i++)
		{
			plaintext_be+=toBinary(plaintext[i],1);
			main_key_be+=toBinary(main_key[i],1);
		}
		/*cout<<"this is initial"<<endl;
		cout<<plaintext_be<<endl;
		cout<<main_key_be<<endl;
		cout<<"============================="<<endl;*/
		// change the decimal to binary
		Initper(plaintext_be);//
		/*cout<<"after permutation"<<endl;
		cout<<plaintext_be<<endl;*/
		// do the initial permutation
		r_plain=plaintext_be.substr(32,32);
		l_plain=plaintext_be.substr(0,32);
		/*cout<<"the both part"<<endl;
		cout<<"left "<<l_plain<<endl;
		cout<<"right "<<r_plain<<endl;*/
		// split the string into left part and right part
		temp.clear();
		for(int i=0;i<16;i++)
		{
			sub_key=generate_subkey(main_key_be,step[i]);
			/*cout<<"====================================="<<endl;
			cout<<sub_key<<endl;
			cout<<"====================================="<<endl;*/
			f_res=f(r_plain,sub_key,step[i]);
			//get the result from f function
			for(int i=0;i<32;i++)
			{
				(f_res[i]==l_plain[i])?(temp+="0"):(temp+="1");
			}
			// exclusive or with the left part
			result=r_plain+temp;
			r_plain="";
			l_plain="";
			f_res="";
			r_plain=result.substr(32,32);
			l_plain=result.substr(0,32);
			/*cout<<"round "<<(i+1)<<endl;
			cout<<result<<endl;*/
			temp.clear();
			// change the left part and right part	
		}
		string reverse_result;
		string real;
		reverse_result.clear();
		real.clear();
		reverse_result=result.substr(32,32)+result.substr(0,32);
		for(int i=0;i<64;i++)
		{
			ans+=reverse_result[Fp[i]-1];
		}
		/*cout<<"this is result"<<endl;
		cout<<ans<<endl;*/
		real=convert_to_hex(ans);
		/*cout<<"finally......"<<endl;
		cout<<real<<endl;*/
		}
		/////////////////////
		/////////////////////
		else
		{
		//cout<<"input the plaintext"<<endl;
		input>>main_key;
		//getline(cin,plaintext,'.');
		//cout<<"input the main_key"<<endl;
		//cin>>main_key;
		//cout<<plaintext<<endl;
		input>>plaintext;
		plaintext_be.clear();
		main_key_be.clear();
		for(int i=0;i<8;i++)
		{
			main_key_be+=toBinary(main_key[i],1);
		}
		plaintext_be=convert_to_binary(plaintext);
		/*cout<<"this is initial"<<endl;
		cout<<plaintext_be<<endl;
		cout<<main_key_be<<endl;
		cout<<"============================="<<endl;*/
		// change the decimal to binary
		Initper(plaintext_be);//
	/*	cout<<"after permutation"<<endl;
		cout<<plaintext_be<<endl;*/
		// do the initial permutation
		r_plain=plaintext_be.substr(32,32);
		l_plain=plaintext_be.substr(0,32);
		/*cout<<"the both part"<<endl;
		cout<<"left "<<l_plain<<endl;
		cout<<"right "<<r_plain<<endl;*/
		// split the string into left part and right part
		for(int i=0;i<16;i++)
		{
			buffer[i]=generate_subkey(main_key_be,step[i]);
		}
		for(int i=15;i>=0;i--)
		{
			/*cout<<"====================================="<<endl;
			cout<<buffer<<endl;
			cout<<"====================================="<<endl;*/
			f_res=f(r_plain,buffer[i],step[i]);
			//get the result from f function
			for(int i=0;i<32;i++)
			{
				(f_res[i]==l_plain[i])?(temp+="0"):(temp+="1");
			}
			// exclusive or with the left part
			result=r_plain+temp;
			r_plain="";
			l_plain="";
			f_res="";
			r_plain=result.substr(32,32);
			l_plain=result.substr(0,32);
			/*cout<<"round "<<(i+1)<<endl;
			cout<<result<<endl;*/
			temp.clear();
			// change the left part and right part	
		}
		string reverse_result;
		string real;
		reverse_result=result.substr(32,32)+result.substr(0,32);
		ans.clear();
		for(int i=0;i<64;i++)
		{
			ans+=reverse_result[Fp[i]-1];
		}
		//cout<<"this is result"<<endl;
		//cout<<ans<<endl;
		real=convert_to_string(ans);
		//cout<<real<<endl;
		output<<real<<endl;;
		//cout<<"finally......"<<endl;
		//cout<<real<<endl;
			}
	}
	output.close();
	return 0;
 } //Hi Mary!
//security
