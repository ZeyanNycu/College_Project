/*
 * Lab problem set for INP course
 * by Chun-Ying Huang <chuang@cs.nctu.edu.tw>
 * License: GPLv2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <map>
#include <stdio.h>
#include <vector>
#include <map>
#include <sstream>
#include <arpa/inet.h>
#include <regex>
using namespace std;
#define err_quit(m) { perror(m); exit(-1); }
string filename = "000000";
string root;
map <string,int> loc;
struct header_t
{
    unsigned short id; // identification number
    uint16_t flags;
    unsigned short q_count; // number of question entries
    unsigned short ans_count; // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count; // number of resource entries
};
struct Question
{
    unsigned short qtype;
    unsigned short qclass;
};
struct R_DATA
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
struct RES_RECORD
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
struct SOA
{
    unsigned int serial;
    unsigned int refresh;
    unsigned int retry;
    unsigned int expire;
    unsigned int minimum;
};
struct c_s
{
    unsigned short val;
};
struct ipv6
{
    unsigned short ip[8];
};
typedef struct
{
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;
char back[200];
map <string,int> type_value;
map <string,int> class_value;
unsigned char* get_the_name_of_query(unsigned char* buf,int &step)
{
    unsigned char* txt = new unsigned char [512];
    for(int i = 0 ; i<512 ; i++) txt[i] = '\0';
    unsigned char* pos = (unsigned char*) (buf+12);
    int now = pos[0]-'\0';
    step++;
    step += now;
    while(now != 0)
    {
        for(int i = 0 ; i<512 ; i++)
        {
            if(txt[i] == '\0')
            {
                for(int j = i; j<=i+now ; j++) 
                {
                    txt[j] = pos[0];
                    pos++;
                } 
                break;
            }
        }
        now = pos[0]-'\0';
        step++;
        step += now; 
    }
    
    return txt;
}
string convert(unsigned char* find)
{
    string s = (char *) find;
    for(int i = 0 ; i<s.length() ; i++)
    {
        if(s[i]-'\0'<40)       s[i] = '.';
    }
    return s;
}
vector <string> split_string(string s,char F)
{
    int pre = 0;
    int ind = 0;
    vector <string> ans;
    for(int i = 0 ; i<s.length() ; i++)
    {
        if(s[i] == F)
        {
            string temp = s.substr(pre,i-pre);
            pre = i+1;
            ans.push_back(temp);
        }
    }
    string temp = s.substr(pre,s.length()-pre);
    ans.push_back(temp);
    return ans;
}
void ChangetoDnsNameFormat(string& host) 
{
    int lock = 0 , i;
    int ind = 0;
    host = "."+host;
    for(int i = 0 ; i<host.length() ; i++)
    {
        if(i == 0 || host[i] == '.')
        {
            int cnt = 0;
            for(int j = i+1 ; j<host.length() ; j++)
            {
                if(host[j] == '.' || host[j] == ' ')    break;
                cnt++;
            }
             host[i] = cnt;
        }
    }
}
void zone_fill_none(RES_RECORD* r,string filename,int aside,unsigned char* all)
{

    filename = filename.substr(0,filename.length()-1);
    int fd = open(filename.c_str(),O_RDONLY);
    char buf[2000];
    char* str;
    for(int i = 0 ; i<2000 ; i++) buf[i] = '\0';
    cout << fd << endl;
    int check = read(fd,buf,2000); // the first line is the domain name
    cout << check << endl;
    string s = buf; 
    vector <string> arr;
    arr = split_string(s,'\n');
    vector <string> want;
    want = split_string(arr[1],',');
    want[4] = want[4].substr(0,want[4].length()-1);
    cout << want[4] << endl;
    r = (RES_RECORD*) (all+aside);
    r -> ttl = htonl(atoi(want[1].c_str()));
    r -> _class = htons(class_value[want[2]]);
    r -> type = htons(type_value[want[3]]);
    int cal = 0;
    want = split_string(want[4],' ');
    ChangetoDnsNameFormat(want[0]);
    for(int i = 0 ; i<want[0].length() ; i++)   all[aside+10+i] = want[0][i];
    ChangetoDnsNameFormat(want[1]);
    for(int i = 0 ; i<want[1].length() ; i++)   all[aside+10+i+want[0].length()] = want[1][i];
    SOA* t = (SOA*) (all+aside+10+want[0].length()+want[1].length());
    unsigned int a = atoi(want[2].c_str());

    t -> serial = htonl(a);
    a = atoi(want[5].c_str());
    t -> expire = htonl(a);
    a = atoi(want[6].c_str());
    t -> minimum = htonl(a);
    a = atoi(want[3].c_str());
    t -> refresh = htonl(a);
    a = atoi(want[4].c_str());
    t -> retry = htonl(a);
    cal = want[0].length()+want[1].length()+sizeof(SOA);
    r -> data_len = htons(cal);
    for(int i = 0 ; i<200 ; i++)
    {
        if(back[i] != '\0')
        {
            all[aside+10+want[0].length()+want[1].length()+sizeof(SOA)+i] = back[i];
        }
    }
    
    close(fd);
}
unsigned short hex_to_int(string s)
{
    unsigned short ans = 0;
    int base = 1;
    for(int i = s.length()-1 ; i>=0 ; i--)
    {
        if(s[i]-'0'>=0 && s[i]-'9'<=9) ans += (s[i]-'0')*base;
        else if(s[i] == 'a')    ans += (10)*base;
        else if(s[i] == 'b')    ans += (11)*base;
        else if(s[i] == 'c')    ans += (12)*base;
        else if(s[i] == 'd')    ans += (13)*base;
        else if(s[i] == 'e')    ans += (14)*base;
        else if(s[i] == 'f')    ans += (15)*base;
        base = base*16;
    }
    return ans;
}
void zone_fill_match(RES_RECORD* r,string filename,int aside,unsigned char* all,string domain,int q_class,int q_type)
{
    //read the file
    regex reg("^([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})\\.([0-9a-zA-Z]{1,61}\\.)*.+$");
    filename = filename.substr(0,filename.length()-1);
    int fd = open(filename.c_str(),O_RDONLY);
    char buf[2000];
    char* str;
    for(int i = 0 ; i<2000 ; i++) buf[i] = '\0';
    int check = read(fd,buf,2000); // the first line is the domain name
    string s = buf;
    vector <string> arr;
    arr = split_string(s,'\n');\
    string special = arr[0];
    special = special.substr(0,special.length()-1);
    for(int i = 1; i<arr.size() ; i++)
    {
        vector <string> temp;
        temp = split_string(arr[i],',');
        string combine = temp[0]+"."+special;
        if(temp[0] == "@" && special == domain && q_type == type_value[temp[3]]) //write the NS
        {
            if(q_type == 2)//write addition and answer
            {
                r = (RES_RECORD*) (all+aside);
                r -> ttl = htonl(atoi(temp[1].c_str()));
                r -> _class = htons(class_value[temp[2]]);
                r -> type = htons(type_value[temp[3]]);
                int cal = 0;
                int cal1 = 0;
                temp[4] = temp[4].substr(0,temp[4].length()-1);
                string add_dns = temp[4];
                ChangetoDnsNameFormat(temp[4]);
                for(int i = 0 ; i<temp[4].length() ; i++)   all[aside+10+i] = temp[4][i];
                cal = temp[4].length();
                r -> data_len = htons(cal);
                for(int j = 0 ; j<arr.size() ; j++)
                {
                    temp = split_string(arr[j],',');
                    combine = temp[0]+"."+special;
                    if(combine == add_dns)
                    {
                        if(temp[3] == "A")
                        {
                            ChangetoDnsNameFormat(add_dns);
                            for(int k = 0 ; k<add_dns.length() ; k++)   all[aside+10+cal+k] = add_dns[k];
                            
                            r = (RES_RECORD*) (all+aside+10+cal+add_dns.length());
                            r -> ttl = htonl(atoi(temp[1].c_str()));
                            r -> _class = htons(class_value[temp[2]]);
                            r -> type = htons(type_value[temp[3]]);
                            temp[4] = temp[4].substr(0,temp[4].length()-1);
                            temp = split_string(temp[4],'.');
                            for(int i = 0 ; i<temp.size() ; i++)   all[aside+10+cal+add_dns.length()+10+i] = atoi(temp[i].c_str());
                            cal1 = 4;
                            r -> data_len = htons(cal1);
                            break;
                        }
                        else if(temp[3] == "AAAA")
                        {
                            ChangetoDnsNameFormat(add_dns);
                            for(int k = 0 ; k<add_dns.length() ; k++)   all[aside+10+cal+k] = add_dns[k];
                            
                            r = (RES_RECORD*) (all+aside+10+cal+add_dns.length());
                            r -> ttl = htonl(atoi(temp[1].c_str()));
                            r -> _class = htons(class_value[temp[2]]);
                            r -> type = htons(type_value[temp[3]]);
                            temp[4] = temp[4].substr(0,temp[4].length()-1);
                            temp = split_string(temp[4],':');
                            struct ipv6* c = (struct ipv6*) (all+aside+10+cal+add_dns.length()+10);
                            for(int i = 0 ; i<8 ; i++)
                            {
                                if(temp[i].length() == 0)
                                {
                                    while(i<7)
                                    {
                                        c -> ip[i] = 0;
                                        i++;
                                    }
                                    c -> ip[i] = htons(hex_to_int(temp[temp.size()-1]));
                                }
                                else
                                {
                                    c -> ip[i] = htons(hex_to_int(temp[i]));
                                }
                            }
                            cal1 = 16;
                            r -> data_len = htons(cal1);
                            break;
                        }
                    }
                }
                for(int i = 0 ; i<200 ; i++)
                {
                    if(back[i] != '\0')
                    {
                        all[aside+10+cal+add_dns.length()+10+cal1+i] = back[i];
                    }
                }
            }
            else if(q_type == 15)
            {
                header_t* a;
                a = (header_t*) all;
                a -> auth_count = htons(1);
                a -> add_count = htons(1);
                r = (RES_RECORD*) (all+aside);
                r -> ttl = htonl(atoi(temp[1].c_str()));
                r -> _class = htons(class_value[temp[2]]);
                r -> type = htons(type_value[temp[3]]);
                int cal = 0;
                int cal1 = 0;
                int cal2 = 0;
                temp = split_string(temp[4],' ');
                temp[1] = temp[1].substr(0,temp[1].length()-1);
                string add_dns = temp[1];
                string add_dns1;
                struct c_s* now = (struct c_s*) (all+aside+10);
                now->val = htons(atoi(temp[0].c_str()));
                ChangetoDnsNameFormat(temp[1]);
                for(int i = 0 ; i<temp[1].length() ; i++)   all[aside+10+2+i] = temp[1][i];
                cal = temp[1].length()+2;
                r -> data_len = htons(cal);
                for(int j = 1 ; j<arr.size() ; j++) // fill the authority 
                {
                    temp = split_string(arr[j],',');
                    if(temp[3] == "NS")
                    {
                        add_dns1 = arr[0].substr(0,arr[0].length()-1);
                        ChangetoDnsNameFormat(add_dns1);
                        for(int k = 0 ; k<add_dns1.length() ; k++)   all[aside+10+cal+k] = add_dns1[k];  
                        r = (RES_RECORD*) (all+aside+10+cal+add_dns1.length());
                        r -> ttl = htonl(atoi(temp[1].c_str()));
                        r -> _class = htons(class_value[temp[2]]);
                        r -> type = htons(type_value[temp[3]]);
                        temp[4] = temp[4].substr(0,temp[4].length()-1);
                        ChangetoDnsNameFormat(temp[4]);
                        for(int i = 0 ; i<temp[4].size() ; i++)   all[aside+10+cal+add_dns1.length()+10+i] = temp[4][i];
                        cal1 = temp[4].length();
                        r -> data_len = htons(cal1);
                        break;
                    }
                }
                for(int j = 0 ; j<arr.size() ; j++)
                {
                    temp = split_string(arr[j],',');
                    combine = temp[0]+"."+special;
                    if(combine == add_dns)
                    {
                        if(temp[3] == "A")
                        {
                            ChangetoDnsNameFormat(add_dns);
                            for(int k = 0 ; k<add_dns.length() ; k++)   all[aside+10+cal+add_dns1.length()+10+cal1+k] = add_dns[k];
                            r = (RES_RECORD*) (all+aside+10+cal+add_dns1.length()+10+cal1+add_dns.length());
                            r -> ttl = htonl(atoi(temp[1].c_str()));
                            r -> _class = htons(class_value[temp[2]]);
                            r -> type = htons(type_value[temp[3]]);
                            temp = split_string(temp[4],'.');
                            for(int i = 0 ; i<temp.size() ; i++)   all[aside+10+cal+add_dns1.length()+10+cal1+add_dns.length()+10+i] = atoi(temp[i].c_str());
                            cal2 = 4;
                            r -> data_len = htons(cal2);
                            break;
                        }
                        else if(temp[3] == "AAAA")
                        {
                            ChangetoDnsNameFormat(add_dns);
                            for(int k = 0 ; k<add_dns.length() ; k++)   all[aside+10+cal+add_dns1.length()+10+cal1+k] = add_dns[k];
                            r = (RES_RECORD*) (all+aside+10+cal+add_dns1.length()+10+cal1+add_dns.length());
                            r -> ttl = htonl(atoi(temp[1].c_str()));
                            r -> _class = htons(class_value[temp[2]]);
                            r -> type = htons(type_value[temp[3]]);
                            temp[4] = temp[4].substr(0,temp[4].length()-1);
                            temp = split_string(temp[4],':');
                            struct ipv6* c = (struct ipv6*) (all+aside+10+cal+add_dns1.length()+10+cal1+add_dns.length()+10);
                            for(int i = 0 ; i<8 ; i++)
                            {
                                if(temp[i].length() == 0)
                                {
                                    while(i<7)
                                    {
                                        c -> ip[i] = 0;
                                        i++;
                                    }
                                    c -> ip[i] = htons(hex_to_int(temp[temp.size()-1]));
                                }
                                else
                                {
                                    c -> ip[i] = htons(hex_to_int(temp[i]));
                                }
                            }
                            cal2 = 16;
                            r -> data_len = htons(cal2);
                            break;
                        }
                    }
                }
                for(int i = 0 ; i<200 ; i++)
                {
                    if(back[i] != '\0')
                    {
                        all[aside+10+cal+add_dns1.length()+10+cal1+add_dns.length()+cal2+10+i] = back[i];
                    }
                }
            }
            else if(q_type == 16) // the txt one and do not have addition
            {
                header_t* a;
                string add_dns;
                a = (header_t*) all;
                a -> auth_count = htons(1);
                a -> add_count = htons(0);
                r = (RES_RECORD*) (all+aside);
                r -> ttl = htonl(atoi(temp[1].c_str()));
                r -> _class = htons(class_value[temp[2]]);
                r -> type = htons(type_value[temp[3]]);
                int cal = 0;
                int cal1 = 0;
                temp[4] = temp[4].substr(0,temp[4].length()-1);
                temp = split_string(temp[4],'\"');
                ChangetoDnsNameFormat(temp[1]);
                temp[1] = temp[1].substr(0,temp[1].length()-1);
                cout << temp[1] << endl;
                for(int i = 0 ; i<temp[1].length() ; i++)   all[aside+10+i] = temp[1][i];
                cal = temp[1].length();
                r -> data_len = htons(cal);
                for(int j = 1 ; j<arr.size() ; j++)
                {
                    temp = split_string(arr[j],',');
                    if(temp[3] == "NS")
                    {
                        add_dns = arr[0].substr(0,arr[0].length()-1);
                        ChangetoDnsNameFormat(add_dns);
                        for(int k = 0 ; k<add_dns.length() ; k++)   all[aside+10+cal+k] = add_dns[k];
                        
                        r = (RES_RECORD*) (all+aside+10+cal+add_dns.length());
                        r -> ttl = htonl(atoi(temp[1].c_str()));
                        r -> _class = htons(class_value[temp[2]]);
                        r -> type = htons(type_value[temp[3]]);
                        temp[4] = temp[4].substr(0,temp[4].length()-1);
                        ChangetoDnsNameFormat(temp[4]);
                        for(int i = 0 ; i<temp[4].size() ; i++)   all[aside+10+cal+add_dns.length()+10+i] = temp[4][i];
                        cal1 = temp[4].length();
                        r -> data_len = htons(cal1);
                        break;
                    }
                }
                for(int i = 0 ; i<200 ; i++)
                {
                    if(back[i] != '\0')
                    {
                        all[aside+10+cal+add_dns.length()+10+cal1+i] = back[i];
                    }
                }
            }
            else if(q_type == 6)
            {
                header_t* b;
                string add_dns;
                b = (header_t*) all;
                b -> auth_count = htons(1);
                b -> add_count = htons(0);
                r = (RES_RECORD*) (all+aside);
                r -> ttl = htonl(atoi(temp[1].c_str()));
                r -> _class = htons(class_value[temp[2]]);
                r -> type = htons(type_value[temp[3]]);
                int cal = 0;
                int cal1 = 0;
                temp[4] = temp[4].substr(0,temp[4].length()-1);
                temp = split_string(temp[4],' ');

                ChangetoDnsNameFormat(temp[0]);
                for(int i = 0 ; i<temp[0].length() ; i++)   all[aside+10+i] = temp[0][i];
                ChangetoDnsNameFormat(temp[1]);
                for(int i = 0 ; i<temp[1].length() ; i++)   all[aside+10+i+temp[0].length()] = temp[1][i];
                SOA* t = (SOA*) (all+aside+10+temp[0].length()+temp[1].length());
                unsigned int a = atoi(temp[2].c_str());

                t -> serial = htonl(a);
                a = atoi(temp[5].c_str());
                t -> expire = htonl(a);
                a = atoi(temp[6].c_str());
                t -> minimum = htonl(a);
                a = atoi(temp[3].c_str());
                t -> refresh = htonl(a);
                a = atoi(temp[4].c_str());
                t -> retry = htonl(a);
                cal = temp[0].length()+temp[1].length()+sizeof(SOA);
                r -> data_len = htons(cal);
                for(int j = 1 ; j<arr.size() ; j++)
                {
                    temp = split_string(arr[j],',');
                    if(temp[3] == "NS")
                    {
                        add_dns = arr[0].substr(0,arr[0].length()-1);
                        ChangetoDnsNameFormat(add_dns);
                        for(int k = 0 ; k<add_dns.length() ; k++)   all[aside+10+cal+k] = add_dns[k];
                        
                        r = (RES_RECORD*) (all+aside+10+cal+add_dns.length());
                        r -> ttl = htonl(atoi(temp[1].c_str()));
                        r -> _class = htons(class_value[temp[2]]);
                        r -> type = htons(type_value[temp[3]]);
                        temp[4] = temp[4].substr(0,temp[4].length()-1);
                        ChangetoDnsNameFormat(temp[4]);
                        for(int i = 0 ; i<temp[4].length() ; i++)   all[aside+10+cal+add_dns.length()+10+i] = temp[4][i];
                        cal1 = temp[4].length();
                        r -> data_len = htons(cal1);
                        break;
                    }
                }
                for(int i = 0 ; i<200 ; i++)
                {
                    if(back[i] != '\0')
                    {
                        all[aside+10+cal+add_dns.length()+10+cal1+i] = back[i];
                    }
                }
            }
            else if(q_type == 5)
            {
                header_t* a;
                string add_dns;
                a = (header_t*) all;
                a -> auth_count = htons(1);
                a -> add_count = htons(0);
                r = (RES_RECORD*) (all+aside);
                r -> ttl = htonl(atoi(temp[1].c_str()));
                r -> _class = htons(class_value[temp[2]]);
                r -> type = htons(type_value[temp[3]]);
                int cal = 0;
                int cal1 = 0;
                temp[4] = temp[4].substr(0,temp[4].length()-1);
                ChangetoDnsNameFormat(temp[4]);
                for(int i = 0 ; i<temp[4].length() ; i++)   all[aside+10+i] = temp[4][i];
                cal = temp[4].length();
                r -> data_len = htons(cal);
                for(int j = 1 ; j<arr.size() ; j++)
                {
                    temp = split_string(arr[j],',');
                    if(temp[3] == "NS")
                    {
                        add_dns = arr[0].substr(0,arr[0].length()-1);
                        ChangetoDnsNameFormat(add_dns);
                        for(int k = 0 ; k<add_dns.length() ; k++)   all[aside+10+cal+k] = add_dns[k];
                        
                        r = (RES_RECORD*) (all+aside+10+cal+add_dns.length());
                        r -> ttl = htonl(atoi(temp[1].c_str()));
                        r -> _class = htons(class_value[temp[2]]);
                        r -> type = htons(type_value[temp[3]]);
                        temp[4] = temp[4].substr(0,temp[4].length()-1);
                        ChangetoDnsNameFormat(temp[4]);
                        for(int i = 0 ; i<temp[4].length() ; i++)   all[aside+10+cal+add_dns.length()+10+i] = temp[4][i];
                        cal1 = temp[4].length();
                        r -> data_len = htons(cal1);
                        break;
                    }
                }
                for(int i = 0 ; i<200 ; i++)
                {
                    if(back[i] != '\0')
                    {
                        all[aside+10+cal+add_dns.length()+10+cal1+i] = back[i];
                    }
                }
            }

            return;
        }
        else if(combine == domain && q_type == type_value[temp[3]] && q_type == 1) 
        {
            header_t* a;
            a = (header_t*) all;
            a -> auth_count = htons(1);
            a -> add_count = htons(0);
            r = (RES_RECORD*) (all+aside);
            r -> ttl = htonl(atoi(temp[1].c_str()));
            r -> _class = htons(class_value[temp[2]]);
            r -> type = htons(type_value[temp[3]]);
            int cal = 0;
            int cal1 = 0;
            temp[4] = temp[4].substr(0,temp[4].length()-1);
            temp = split_string(temp[4],'.');
            for(int i = 0 ; i<temp.size() ; i++)   all[aside+10+i] = atoi(temp[i].c_str());
            cal = 4;
            r -> data_len = htons(cal);
            for(int j = 1 ; j<arr.size() ; j++)
            {
                temp = split_string(arr[j],',');
                combine = temp[3];
                if( combine == "NS")
                {
                    domain = arr[0].substr(0,arr[0].length()-1);
                    ChangetoDnsNameFormat(domain);
                    for(int k = 0 ; k<domain.length() ; k++)   all[aside+10+cal+k] = domain[k];
                    r = (RES_RECORD*) (all+aside+10+cal+domain.length());
                    r -> ttl = htonl(atoi(temp[1].c_str()));
                    r -> _class = htons(class_value[temp[2]]);
                    r -> type = htons(type_value[temp[3]]);
                    temp[4] = temp[4].substr(0,temp[4].length()-1);
                    ChangetoDnsNameFormat(temp[4]);
                    for(int k = 0 ; k<temp[4].length() ; k++)   all[aside+10+cal+domain.length()+10+k] = temp[4][k];
                    cal1 = temp[4].length();
                    r -> data_len = htons(cal1);
                    break;
                }
            }
            cout << "hello" << endl;
            for(int i = 0 ; i<200 ; i++)
            {
                if(back[i] != '\0')
                {
                    all[aside+10+cal+domain.length()+10+cal1+i] = back[i];
                }
            }
            return;
        }
        else if(combine == domain && q_type == type_value[temp[3]] && q_type == 28) 
        {
            header_t* a;
            a = (header_t*) all;
            a -> auth_count = htons(1);
            a -> add_count = htons(0);
            r = (RES_RECORD*) (all+aside);
            r -> ttl = htonl(atoi(temp[1].c_str()));
            r -> _class = htons(class_value[temp[2]]);
            r -> type = htons(type_value[temp[3]]);
            int cal = 0;
            int cal1 = 0;
            temp[4] = temp[4].substr(0,temp[4].length()-1);
            temp = split_string(temp[4],':');
            struct ipv6* c = (struct ipv6*) (all+aside+10);

            for(int i = 0 ; i<8 ; i++)
            {
                if(temp[i].length() == 0)
                {
                    while(i<7)
                    {
                        c -> ip[i] = 0;
                        i++;
                    }
                    c -> ip[i] = htons(hex_to_int(temp[temp.size()-1]));
                }
                else
                {
                    c -> ip[i] = htons(hex_to_int(temp[i]));
                }
            }
            cal = 16;
            r -> data_len = htons(cal);
            for(int j = 1 ; j<arr.size() ; j++)
            {
                temp = split_string(arr[j],',');
                combine = temp[3];
                if( combine == "NS")
                {
                    ChangetoDnsNameFormat(domain);
                    for(int k = 0 ; k<domain.length() ; k++)   all[aside+10+cal+k] = domain[k];
                    r = (RES_RECORD*) (all+aside+10+cal+domain.length());
                    r -> ttl = htonl(atoi(temp[1].c_str()));
                    r -> _class = htons(class_value[temp[2]]);
                    r -> type = htons(type_value[temp[3]]);
                    temp[4] = temp[4].substr(0,temp[4].length()-1);
                    ChangetoDnsNameFormat(temp[4]);
                    for(int k = 0 ; k<temp[4].length() ; k++)   all[aside+10+cal+domain.length()+10+k] = temp[4][k];
                    cal1 = temp[4].length();
                    r -> data_len = htons(cal1);
                    break;
                }
            }
            for(int i = 0 ; i<200 ; i++)
            {
                if(back[i] != '\0')
                {
                    all[aside+10+cal+domain.length()+10+cal1+i] = back[i];
                }
            }
            return;
        }
    }
    //match the rule it has
    //write the NS
}
int match_rule(string filename,string domain,int q_class,int q_type)
{
    filename = filename.substr(0,filename.length()-1);
    int fd = open(filename.c_str(),O_RDONLY);
    char buf[2000];
    char* str;
    for(int i = 0 ; i<2000 ; i++) buf[i] = '\0';
    int check = read(fd,buf,2000); // the first line is the domain name
    string s = buf;
    vector <string> arr;
    arr = split_string(s,'\n');\
    string special = arr[0];
    special = special.substr(0,special.length()-1);
    for(int i = 1; i<arr.size() ; i++)
    {
        vector <string> temp;
        temp = split_string(arr[i],',');
        string combine = temp[0]+"."+special;
        if(temp[0] == "@" && special == domain && q_type == type_value[temp[3]]) 
        {
            close(fd);   
            return i;
        }
        else if(combine == domain && q_type == type_value[temp[3]]) 
        {
            close(fd);   
            return i;
        }
    }
    return 0;
}
bool match_zone_rule(string filename,string filename2)
{
    if(filename == filename2) return true;
    int pre = 0;
    for(int i = 0 ; i<filename2.length() ; i++)
    {
        if(filename2[i] == '.')
        {
            pre = i+1;
            break;
        }
    }
    string filename3 = filename2.substr(pre,filename2.length()-pre);
    //cout << filename3 << " " << filename << endl;
    if(filename3 == filename) return true;
    return false;
}
int fill_the_request(RES_RECORD* r,string filename,unsigned char* find,int aside,unsigned char* all,int q_class,int q_type,int next)
{
    regex reg("^([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})\\.([0-9a-zA-Z]{1,61}\\.)*.+$");
    int fd = open(filename.c_str(),O_RDONLY);
    char buf[2000];
    char* str;
    for(int i = 0 ; i<2000 ; i++) buf[i] = '\0';
    int check = read(fd,buf,2000);
    for(int i = 0 ; i<check ; i++)
    {
        if(buf[i] == '\n')
        {
            str = (buf+i+1);
            break;
        }
    }
    int pre = 0;
    string match = str;
    for(int i = 0 ; i<match.length() ; i++)
    {
        if(match[i] == ',')
        {
            string temp = match.substr(pre,i-pre);
            string temp1 = convert(find);
            temp1 = temp1.substr(1,temp1.length()-1);
            temp1 = temp1+".";
            if(match_zone_rule(temp,temp1)) //here need to set up the rule to know met which type 
            // 0-> do not match any line
            // 1-> match one of the line and give which one
            {
                string need;
                cout << "I am in " << endl;
                int pre_j = i+1;
                for(int j = i+1; j<match.length() ; j++)
                {
                    if(match[j] == '\n')
                    {
                        need = match.substr(pre_j,j-pre_j);
                        break;
                    }
                }
                if(match_rule(need,temp1,q_class,q_type))
                {
                    cout << "the next check" << endl;
                    zone_fill_match(r,need,aside,all,temp1,q_class,q_type);
                    close(fd);
                    return 1;
                }
                else 
                {
                    header_t* temp;
                    temp = (header_t*) all;
                    cout << "happy" << endl;
                    temp -> auth_count = htons(1);
                    zone_fill_none(r,need,aside,all);
                    close(fd);
                    return 0;
                }
            }
        }
        if(match[i] == '\n') pre = i+1;
    }
    string temp1 = convert(find);
    temp1 = temp1.substr(1,temp1.length()-1);
    if( regex_match(temp1, reg) ){
        header_t* a;
        a = (header_t*) all;
        a -> auth_count = htons(0);
        a -> add_count = htons(0);
        r = (RES_RECORD*) (all+aside);
        r -> ttl = htonl(1);
        r -> _class = htons(1);
        r -> type = htons(1);
        vector <string> need;
        need = split_string(temp1,'.');
        for(int i = 0 ; i<4 ; i++)  all[aside+10+i] = atoi(need[i].c_str());
        int cal = 4;
        r -> data_len = htons(cal);
        for(int i = 0 ; i<200 ; i++)
        {
            if(back[i] != '\0')
            {
                all[aside+10+cal+i] = back[i];
            }
        }
        return 1;
    }
    return -1;
    //if find the zone file
}
int main(int argc, char *argv[]) {
	int s;
	struct sockaddr_in sin;
    struct sockaddr_in dest;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	long aaa = strtol(argv[1], NULL, 0);
	sin.sin_port = htons(aaa);
    header_t* dns;
    Question* q;
    string filename = argv[2];
	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		err_quit("socket");
    type_value["A"]=1;
    type_value["NS"]=2;
    type_value["MX"]=15;
    type_value["TXT"]=16;
    type_value["SOA"]=6;
    type_value["CNAME"]=5;
    type_value["AAAA"]=28;


    class_value["IN"] = 1;
    class_value["CS"] = 2;
    class_value["CH"] = 3;
    class_value["HS"] = 4;
	if(bind(s, (struct sockaddr*) &sin, sizeof(sin)) < 0)
		err_quit("bind");
	int fd;
	while(1) {
		struct sockaddr_in csin;
		socklen_t csinlen = sizeof(csin);
		int rlen;
        unsigned char buf[512];
		for(int i = 0 ; i<512 ; i++) buf[i] = '\0';
		if((rlen = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*) &csin, &csinlen)) < 0) {
			perror("recvfrom");
			break;
		}
        unsigned char* find;
        header_t* temp;
        RES_RECORD* response;
        int next = 0;
        find = get_the_name_of_query(buf,next);
        next++;
        q = (Question*) (buf+sizeof(header_t)+next);
        temp = (header_t*) buf;
        //record the back of the file
        for(int i = 0 ; i<200 ; i++) back[i] = '\0';
        for(int i = sizeof(header_t)+next+3 ; i<rlen ; i++) back[i-(sizeof(header_t)+next+3)] = buf[i];
        //fill the name of the server part
        string temp1 = (char* )find;
        temp1 = temp1 + '\0';
        int happy_new_year = htons(fill_the_request(response,filename,find,sizeof(header_t)+next+3+temp1.length(),buf,q->qclass-'\0',q->qtype-'\0',next)); // find the right zone-file in the config.txt

        if(happy_new_year == 65535)
        {

            int fd = open(filename.c_str(),O_RDONLY);
            char name[2000];
            char* str;
            for(int i = 0 ; i<2000 ; i++) name[i] = '\0';
            int check = read(fd,name,2000);
            string use = name;
            int val = 21250;
            char dns_servers[10][100];
            for(int i = 0 ; i<use.length() ; i++)
            {
                if(use[i] == '\n')
                {
                    string ip = use.substr(0,i);
                    ip = ip.substr(0,ip.length()-1);
                    int s1 =socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP); //UDP packet for DNS queries
                    dest.sin_family = AF_INET;
                    dest.sin_port = htons(53);
                    dest.sin_addr.s_addr = inet_addr(ip.c_str()); //dns servers
                    socklen_t destlen = sizeof(csin);
                    if( sendto(s1,(char*)buf,512,0,(struct sockaddr*)&dest,sizeof(dest)) < 0)
                    {
                        perror("recvfrom");
			            break;
                    }
                    if(recvfrom (s1,(char*)buf , 512 , 0 , (struct sockaddr*) &dest, &destlen) < 0)
                    {
                        perror("recvfrom");
			            break;
                    }
                    sendto(s, buf, 512, 0,  (struct sockaddr*) &csin, sizeof(csin));   
                    break;
                }
            }
        }
        else
        {
            for(int i = 0 ; i<temp1.length() ; i++) buf[sizeof(header_t)+next+3+i] = temp1[i];
            temp -> ans_count = happy_new_year;
            temp = (header_t*) buf;
            temp ->flags = 0b000000010000101;
            sendto(s, buf, 512, 0,  (struct sockaddr*) &csin, sizeof(csin));
        }
	}

	close(s);
}