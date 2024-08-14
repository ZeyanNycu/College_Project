#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <elf.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#define BUF_SIZE 1024
#define S_IFMT 00170000
#define S_IFLNK 0120000
static int(*__libc_start_main_origin) (int *(main) (int, char * *, char * *), int argc, char * * ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end)) = NULL;
static int(*real_open)(const char *pathname, int flags, mode_t mode) = NULL;
static long main_min = 0, main_max = 0;
static char* SANDBOX_CONFIG;
static int LOGGER_FD,SANDBOX_CONFIG_FD,READ_FD;
static void* handle;
void print_str_table(char* str_table, int str_table_size,int index) {
    char* str = str_table;
    int i = 0;
    int ind =0;
    while (i < str_table_size) {
        if(ind != index) printf("%d: %s\n", i, str);
        str += strlen(str) + 1;
        i += strlen(str) + 1;
        ind += 1;
    }
}
static void get_base(char* cmd) {
	int fd, sz;
	char buf[16384], *s = buf, *line, *saveptr;
	if((fd = open("/proc/self/maps", O_RDONLY)) < 0) errquit("get_base/open");
	if((sz = read(fd, buf, sizeof(buf)-1)) < 0) errquit("get_base/read");
	buf[sz] = 0;
	close(fd);
	while((line = strtok_r(s, "\n\r", &saveptr)) != NULL) { s = NULL;
		if(strstr(line, " r--p ") == NULL) continue;
		if(strstr(line, cmd) != NULL) {
			if(sscanf(line, "%lx-%lx ", &main_min, &main_max) != 2) errquit("get_base/main");
		}
        return;
	}
	_exit(-fprintf(stderr, "** get_base failed.\n"));
}
static void get_information()
{
    int fd, sz;
	char buf[16384], *s = buf, *line, *saveptr;
    fd = open("/proc/self/environ", O_RDONLY);
    if((sz = read(fd, buf, sizeof(buf)-1)) < 0) errquit("get_base/read");
	buf[sz] = 0;
    char* str = buf;
    char config[] = "SANDBOX_CONFIG";
    char log_fd[] = "LOGGER_FD";
    int i = 0;
    SANDBOX_CONFIG = malloc(100*sizeof(char));
    while (i < sz) {
        if(strlen(str)>14 && strncmp(str,config,14) == 0){
            strncpy(SANDBOX_CONFIG,str+15,strlen(str+15));
        }
        if(strlen(str)>9 && strncmp(str,log_fd,9) == 0) LOGGER_FD = atoi(str+10);
        str += strlen(str) + 1;
        i += strlen(str) + 1;
    }
    return;
}
void write_to_logger(char* text,int size)
{
    write(LOGGER_FD,text,size);
}

int fake_system(const char *command)
{
    bool sign = false; //false outside the txt True inside the rule
    //read the config.txt file
    int  sz;
    int len = 0;
	char buff[1024*1024];
    SANDBOX_CONFIG_FD  = open(SANDBOX_CONFIG, O_RDONLY);
    read(SANDBOX_CONFIG_FD,buff,1024*1024);
    char* token = strtok(buff,"\n\r");
    while( token != NULL ) {
        if(strcmp(token,"END system-blacklist") == 0) sign = false;
        if(sign)
        {
            //read the whole file first
            bool pass = true; //false means banned
            if(strcmp(token,command) == 0)
            {
                char* b = malloc(BUF_SIZE*sizeof(char));
                memset(b,0,BUF_SIZE);
                sprintf(b,"[logger] system(%s)\n",command);
                write_to_logger(b,BUF_SIZE);
                return -1;
            }
        }
        if(strcmp(token,"BEGIN system-blacklist") == 0) sign = true;
        token = strtok(NULL, "\n\r");
    }
    close(SANDBOX_CONFIG_FD);
    //logger the content to the specific file
    char* b = malloc(BUF_SIZE*sizeof(char));
    sprintf(b,"[logger] system(%s)\n",command);
    write_to_logger(b,strlen(b));
    int size = system(command);
    return size;
}
ssize_t fake_write(int fd, const void *buf, size_t count)
{
    char* a = malloc(BUF_SIZE*sizeof(char));
    sprintf(a,"%d-%d-write.log",getpid(),fd);
    bool sign = false; //false outside the txt True inside the rule
    //read the config.txt file
    int  sz;
    int len = 0;
	char buff[1024*1024];
    SANDBOX_CONFIG_FD  = open(SANDBOX_CONFIG, O_RDONLY);
    read(SANDBOX_CONFIG_FD,buff,1024*1024);
    char* token = strtok(buff,"\n\r");
    while( token != NULL ) {
        if(strcmp(token,"END write-blacklist") == 0) sign = false;
        if(sign)
        {
            //read the whole file first
            bool pass = true; //false means banned
            if(count>strlen(token))
            {
                for(int i = 0 ; i<count-strlen(token) ; i++)
                {
                    if( strncmp(token,buf+i,strlen(token)) == 0 )
                    {
                        pass = false;
                        break;
                    }
                }
                if(!pass)
                {
                    char* b = malloc(BUF_SIZE*sizeof(char));
                    memset(b,0,BUF_SIZE);
                    sprintf(b,"[logger] write(%d, %p, %d) = %d\n",fd,buf,count,-1);
                    write_to_logger(b,BUF_SIZE);
                    errno = EIO;
                    close(fd);
                    return -1;
                }
            }
            //
        }
        if(strcmp(token,"BEGIN write-blacklist") == 0) sign = true;
        token = strtok(NULL, "\n\r");
    }
    close(SANDBOX_CONFIG_FD);
    //logger the content to the specific file
    char* b = malloc(BUF_SIZE*sizeof(char));
    int size = write(fd,buf,count);
    sprintf(b,"[logger] write(%d, %p, %d) = %d\n",fd,buf,count,size);
    write_to_logger(b,strlen(b));
    int new_open = open(a, O_WRONLY | O_CREAT | O_APPEND , 0644);
    write(new_open,buf,size);
    close(new_open);
    return size;
}
int fake_connect(int sockfd, const struct sockaddr *saddr,socklen_t addrlen)
{
    bool sign = false; //false outside the txt True inside the rule
    //read the config.txt file
    int  sz;
    int len = 0;
	char buff[1024*1024];
    SANDBOX_CONFIG_FD  = open(SANDBOX_CONFIG, O_RDONLY);
    read(SANDBOX_CONFIG_FD,buff,1024*1024);
    char* token = strtok(buff,"\n\r");
    struct sockaddr_in* ptr = (struct sockaddr_in*) saddr;
    char* name;
    while( token != NULL ) {
        char temp1[BUF_SIZE];
        if(strcmp(token,"END connect-blacklist") == 0) sign = false;
        if(sign)
        {
            //read the whole file first
            bool pass = true; //false means banned
            char temp[BUF_SIZE];
            char temp_port[BUF_SIZE];
            int len,status;
            memset(temp,0,BUF_SIZE);
            memset(temp_port,0,BUF_SIZE);
            for(int i = 0 ; i<strlen(token) ; i++)
            {
                if(token[i] == ':')
                {
                    len = i+1;
                    break;
                }
            }
            strncpy(temp,token,len-1);
            strncpy(temp_port,token+len,strlen(token)-len);
            struct addrinfo hints,*res;
            memset(&hints, 0, sizeof hints); // make sure the struct is empty
            hints.ai_family = AF_UNSPEC;     // don't care if it's IPv4 or IPv6
            hints.ai_socktype = SOCK_STREAM;
            if ((status = getaddrinfo(temp, NULL, &hints, &res)) != 0) {
                fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
                exit(2);
            }
            struct addrinfo *p;
            char ipstr[INET6_ADDRSTRLEN];
            char ipstr_0[INET6_ADDRSTRLEN];
            for (p = res; p != NULL; p = p->ai_next) {
                void *addr;
                char *ipver;

                // Get the pointer to the address itself,
                // different fields in IPv4 and IPv6:
                if (p->ai_family == AF_INET) { // IPv4
                    struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
                    addr = &(ipv4->sin_addr);
                    ipver = "IPv4";
                    inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
                    addr = &(ptr->sin_addr);
                    inet_ntop(p->ai_family, addr, ipstr_0, sizeof ipstr_0);
                    char temp_2[BUF_SIZE];
                    sprintf(temp_2,"%d",ntohs(ptr->sin_port));
                    name = ipstr_0;
                    if(strcmp(ipstr,ipstr_0) == 0 && strcmp(temp_2,temp_port) == 0)
                    {
                        errno = ECONNREFUSED;
                        //write the logger
                        char* b = malloc(BUF_SIZE*sizeof(char));
                        memset(b,0,BUF_SIZE);
                        sprintf(b,"[logger] connect(%d, \"%s\", %d) = %d\n",sockfd,ipstr_0,addrlen,-1);
                        write_to_logger(b,BUF_SIZE);
                        return -1;
                    }
                }
            }
        }
        if(strcmp(token,"BEGIN connect-blacklist") == 0) sign = true;
        token = strtok(NULL, "\n\r");
    }
    close(SANDBOX_CONFIG_FD);
    int ret = connect(sockfd,saddr,addrlen);
    char* b = malloc(BUF_SIZE*sizeof(char));
    memset(b,0,BUF_SIZE);
    sprintf(b,"[logger] connect(%d, \"%s\", %d) = %d\n",sockfd,name,addrlen,ret);
    write_to_logger(b,BUF_SIZE);
    return ret;
}
int fake_getaddrinfo(const char *restrict node,
                       const char *restrict service,
                       const struct addrinfo *restrict hints,
                       struct addrinfo **restrict res)
{
    bool sign = false; //false outside the txt True inside the rule
    //read the config.txt file
    int  sz;
    int len = 0;
	char buff[1024*1024];
    SANDBOX_CONFIG_FD  = open(SANDBOX_CONFIG, O_RDONLY);
    read(SANDBOX_CONFIG_FD,buff,1024*1024);
    char* token = strtok(buff,"\n\r");
    while( token != NULL ) {
        if(strcmp(token,"END getaddrinfo-blacklist") == 0) sign = false;
        if(sign)
        {
            //read the whole file first
            bool pass = true; //false means banned
            char temp[BUF_SIZE];
            int len;
            if(strcmp(node,token) == 0)
            {

                errno = EAI_NONAME;
                char* b = malloc(BUF_SIZE*sizeof(char));
                memset(b,0,BUF_SIZE);
                if(service == NULL) sprintf(b,"[logger] getaddrinfo(\"%s\", \"(null)\", %p,%p) = %d\n",node,hints,res,-2);
                else sprintf(b,"[logger] getaddrinfo(\"%s\", \"(%s)\", %p,%p) = %d\n",node,service,hints,res,-2);
                write_to_logger(b,BUF_SIZE);
                return -1;
            }
        }
        if(strcmp(token,"BEGIN getaddrinfo-blacklist") == 0) sign = true;
        token = strtok(NULL, "\n\r");
    }
    close(SANDBOX_CONFIG_FD);
    char* b = malloc(BUF_SIZE*sizeof(char));
    memset(b,0,BUF_SIZE);
    int ret = getaddrinfo(node,service,hints,res);
    if(service == NULL) sprintf(b,"[logger] getaddrinfo(\"%s\", \"(null)\", %p,%p) = %d\n",node,hints,res,ret);
    else sprintf(b,"[logger] getaddrinfo(\"%s\", \"(%s)\", %p,%p) = %d\n",node,service,hints,res,ret);
    write_to_logger(b,BUF_SIZE);
    return ret;
}
ssize_t fake_read(int fd, void *buf, size_t count)
{
    char* a = malloc(BUF_SIZE*sizeof(char));
    sprintf(a,"%d-%d-read.log",getpid(),fd);
    bool sign = false; //false outside the txt True inside the rule
    //read the config.txt file
    int  sz;
    int len = 0;
	char buff[1024*1024];
    SANDBOX_CONFIG_FD  = open(SANDBOX_CONFIG, O_RDONLY);
    int size = read(fd,buf,count);
    read(SANDBOX_CONFIG_FD,buff,1024*1024);
    char* token = strtok(buff,"\n\r");
    while( token != NULL ) {
        if(strcmp(token,"END read-blacklist") == 0) sign = false;
        if(sign)
        {
            //read the whole file first
            bool pass = true; //false means banned
            if(size>strlen(token))
            {
                for(int i = 0 ; i<size-strlen(token) ; i++)
                {
                    if( strncmp(token,buf+i,strlen(token)) == 0 )
                    {
                        pass = false;
                        break;
                    }
                }
                if(!pass)
                {
                    char* b = malloc(BUF_SIZE*sizeof(char));
                    memset(b,0,BUF_SIZE);
                    sprintf(b,"[logger] read(%d, %p, %d) = %d\n",fd,buf,count,-1);
                    write_to_logger(b,BUF_SIZE);
                    errno = EIO;
                    close(fd);
                    return -1;
                }
            }
            //
        }
        if(strcmp(token,"BEGIN read-blacklist") == 0) sign = true;
        token = strtok(NULL, "\n\r");
    }
    close(SANDBOX_CONFIG_FD);
    //logger the content to the specific file
    char* b = malloc(BUF_SIZE*sizeof(char));
    sprintf(b,"[logger] read(%d, %p, %d) = %d\n",fd,buf,count,size);
    write_to_logger(b,strlen(b));
    int new_open = open(a, O_WRONLY | O_CREAT | O_APPEND, 0777);
    write(new_open,buf,size);
    close(new_open);
    return size;
}
int fake_open(const char *pathname, int flags, mode_t mode)
{
    if(!(flags & O_CREAT)) mode = 0;
    char* a = malloc(BUF_SIZE*sizeof(char));
    bool sign = false; //false outside the txt True inside the rule
    //read the config.txt file
    int  sz;
    int len = 0;
	char buf[16384];
    int target_fd = open(pathname,flags,mode);
    if(target_fd != -1)
    {
        struct stat file_target;
        fstat(target_fd,&file_target);
        int target_inode = file_target.st_ino;
        SANDBOX_CONFIG_FD  = open(SANDBOX_CONFIG, O_RDONLY);
        read(SANDBOX_CONFIG_FD,buf,16384);
        char* token = strtok(buf,"\n\r");
        while( token != NULL ) {
            if(strcmp(token,"END open-blacklist") == 0) sign = false;
            if(sign)
            {
                //check if it is symbolic link file
                struct stat st;
                int now_fd = open(token,flags,mode);
                if(now_fd != -1)
                {
                    fstat(now_fd,&st);
                    int now_inode = st.st_ino;
                    if(now_inode == target_inode)
                    {
                        errno = EACCES;
                        sprintf(a,"[logger] open(%s,%d,%d) = %d\n",pathname,flags,mode,-1);
                        write_to_logger(a,BUF_SIZE);
                        return -1;
                    }
                    close(now_fd);
                }
            }
            if(strcmp(token,"BEGIN open-blacklist") == 0) sign = true;
            token = strtok(NULL, "\n\r");
        }
        close(SANDBOX_CONFIG_FD);
    }
    sprintf(a,"[logger] open(%s,%d,%d) = %d\n",pathname,flags,mode,target_fd);
    write_to_logger(a,BUF_SIZE);
    return target_fd;
}
int get_index(char* s,char* buf,int offset,char* target,uint64_t size)
{
    Elf64_Sym* temp;
    int ans = -1;
    for(int i = 0 ; i<size/sizeof(Elf64_Sym) ; i++)
    {
        temp = (Elf64_Sym*) (buf+offset+sizeof(Elf64_Sym)*i);
        if(strcmp(s+temp->st_name,target) == 0)
        {
            ans = i;
            break;
        }
    }
    return ans;
}
void (*fake_func)();
long get_address_and_logger(char* buf,char* target,int num,Elf64_Ehdr* EH)
{
    Elf64_Shdr* SH_sym;
    Elf64_Shdr* SH_str;
    Elf64_Shdr* SH_got;
    Elf64_Shdr* SH_strtable = (Elf64_Shdr*) (buf+EH->e_shoff+num*EH->e_shstrndx);
    Elf64_Shdr* SH_rela_plt;
    Elf64_Sym* sym;
    char* name;
    name = (char*) (buf+SH_strtable->sh_offset);
    int n = 0;
    for(int i=0 ; i<EH->e_shnum ; i++)
    {
        SH_sym = (Elf64_Shdr*) (buf+EH->e_shoff+num*i);
        if(SH_sym->sh_type == SHT_DYNSYM && strcmp(".dynsym",name+SH_sym->sh_name) == 0)
        {
            break;
        }
        n = n + 1;
    }
        //Go to the data section string section
    for(int i=0 ; i<EH->e_shnum ; i++)
    {
        SH_str = (Elf64_Shdr*) (buf+EH->e_shoff+num*i);
        if(SH_str->sh_type == SHT_STRTAB && strcmp(".dynstr",name+SH_str->sh_name) == 0)
        {
            break;
        }
        n = n + 1;
    }
    char* s = (char*) (buf+SH_str->sh_offset);
    int idx = get_index(s,buf,SH_sym->sh_offset,target,SH_sym->sh_size);
    if(idx == -1) return -1;
    //find the relocation table
    for(int i=0 ; i<EH->e_shnum ; i++)
    {
        SH_rela_plt = (Elf64_Shdr*) (buf+EH->e_shoff+num*i);
        if(strcmp(".rela.plt",name+SH_rela_plt->sh_name) == 0)
        {
            break;
        }
    }
    for(int i=0 ; i<EH->e_shnum ; i++)
    {
        SH_got = (Elf64_Shdr*) (buf+EH->e_shoff+num*i);
        if(strcmp(".got",name+SH_got->sh_name) == 0)
        {
            break;
        }
    }
    Elf64_Rela* function;
    for(int i = 0 ; i<SH_rela_plt->sh_size/SH_rela_plt->sh_entsize ; i++)
    {
        function = (Elf64_Rela*) (buf+SH_rela_plt->sh_offset+i*sizeof(Elf64_Rela));
        unsigned long a = function->r_info >> 32;
        if(a == idx) break;
    }
    long result = function->r_offset-function->r_addend;
    //put my function into the open function
    long min_GOT = main_min+result;
	min_GOT = min_GOT-(min_GOT % 4096);
	void *addr = (void*) min_GOT;
    long len = 2;
	int check = mprotect(addr,4096*len,(PROT_WRITE | PROT_READ | PROT_EXEC));
    char* tmp1 = malloc(1024*sizeof(char));
    memset(tmp1,0,1024);
    sprintf(tmp1,"fake_%s",target);
    void* handle =dlopen("sandbox.so",RTLD_LAZY);
    fake_func = dlsym(handle,tmp1);
    void* p = (void*) (main_min+result);
    memcpy(p,(&fake_func),sizeof(void*));
    return result;
}
int __libc_start_main(int *(main) (int, char * *, char * *), int argc, char * * ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end))
{
    // Try to open the default .so file to make sure that we can find the default function
    int ret;
    handle = dlopen("/lib/x86_64-linux-gnu/libc.so.6",RTLD_LAZY);
    void* origin = dlsym(handle,"__libc_start_main");
    __libc_start_main_origin = origin;
    // Got the logger file descriptor
    // Got the config name
    get_information();

    // Hijack the GOT table of the /proc/self/exe
        // step 1 -> get the real program location
    char exe_path[BUF_SIZE];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    exe_path[len] = '\0';
        // step 2 -> get the got table of the real program
    int fd = open(exe_path, O_RDONLY);
    
    if (fd == -1) {
        perror("open");
        close(fd);
        ret =__libc_start_main_origin(main,argc,ubp_av,init,fini,rtld_fini,stack_end);
       _Exit(ret);
    }
        // step 2-1 -> read the file of the elf
    int size = 0;
    char* buf = malloc(sizeof(char)*(1024*1024*1024));
    size = read(fd,buf,1024*1024*1024);
    Elf64_Ehdr* EH = (Elf64_Ehdr*) buf;
    int num = EH->e_ehsize;
        // step 2-2 -> get the program name to check if the API should 
            // find the name is .dynsym
    get_base(exe_path);
    get_address_and_logger(buf,"open",num,EH);
    get_address_and_logger(buf,"read",num,EH);
    get_address_and_logger(buf,"write",num,EH);
    get_address_and_logger(buf,"connect",num,EH);
    get_address_and_logger(buf,"getaddrinfo",num,EH);
    get_address_and_logger(buf,"system",num,EH);
    ret =__libc_start_main_origin(main,argc,ubp_av,init,fini,rtld_fini,stack_end);
    _Exit(ret);
}