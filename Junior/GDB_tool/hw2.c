#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/user.h>
#include <elf.h>
#include <capstone/capstone.h>
#include <string.h>
#include <sys/mman.h>
int ind=0;

void errquit(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}
unsigned long long get_entry_point(char* s)
{
    int f = open(s,O_RDONLY);
    char contents[1024];
    read(f,contents,1024);
    Elf64_Ehdr* h = (Elf64_Ehdr*) contents;
    close(f);
    return h->e_entry;
}

void dissamble_addr(unsigned long long addr,char* s,unsigned long long origin,unsigned long long start)
{
    csh handle; //tell the API what kind of the assemble is
    cs_insn *insns;
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
        return;
    Elf64_Shdr* SH_text;
    int f = open(s,O_RDONLY);
    char contents[1024*1024];
    read(f,contents,1024*1024);
    Elf64_Ehdr* h = (Elf64_Ehdr*) contents;
    Elf64_Shdr* SH_strtable = (Elf64_Shdr*) (contents+h->e_shoff+(h->e_ehsize)*h->e_shstrndx);
    char* name = (char*) (contents+SH_strtable->sh_offset);
    for(int i = 0 ; i<h->e_shnum ; i++)
    {
        SH_text = (Elf64_Shdr*) (contents+h->e_shoff+(h->e_ehsize)*i);
        if(SH_text->sh_type == SHT_PROGBITS && strcmp(".text",name+SH_text->sh_name) == 0) break;
    }

    int n = cs_disasm(handle,contents+SH_text->sh_offset+(origin-SH_text->sh_addr),SH_text->sh_size,origin,0,&insns);
    int cnt = 0;
    for (int i=0;i<n;i++) {
        if(cnt == 5) break;
        if(insns[i].address>=start)
        {
            printf("       %lx: ",insns[i].address);
            for (size_t j=0;j<16;j++) {
                if (j < insns[i].size)
                    printf("%02x ",insns[i].bytes[j]);
                else
                    printf("   ");
            }
            printf("%-12s %s\n",insns[i].mnemonic,insns[i].op_str);
            cnt++;
        }
    }
    if(cnt<5) printf("** the address is out of the range of the text section.\n");
    cs_free(insns,n); // 释放空间
    cs_close(&handle);
    close(f);
}
int find_ind(unsigned long long index[],unsigned long long addr)
{
    for(int i = 0 ; i<ind ; i++)
    {
        if(index[i]==addr) return i;
    }
    return -1;
}
static long main_min = 0, main_max = 0;
void* anchor_addr;
unsigned long long threshold = 0;
long anchor_value[1024*512];
void get_base(int child) {
    threshold = 0;
	int fd, sz;
	char buf[16384], *s = buf, *line, *saveptr;
    char filename[1024];
    memset(anchor_value,0,1024*512);
    sprintf(filename,"/proc/%d/maps",child);
	if((fd = open(filename, O_RDONLY)) < 0) errquit("get_base/open");
	if((sz = read(fd, buf, sizeof(buf)-1)) < 0) errquit("get_base/read");
	buf[sz] = 0;
	close(fd);
	while((line = strtok_r(s, "\n\r", &saveptr)) != NULL) { s = NULL;
		if(strstr(line, " rw-p ") == NULL && strstr(line, " rwxp ") == NULL) continue;
		sscanf(line,"%lx-%lx",&main_min,&main_max);
        for(int i = 0 ; i<(main_max-main_min)/8 ; i++)
        {
            long* pos = (long*) (anchor_addr+threshold);
            void* addr = (void*) (main_min+i*8);
            anchor_value[threshold+i] = ptrace(PTRACE_PEEKTEXT, child, addr, NULL);
        }
        threshold += (main_max-main_min)/8;
	}
	return;
}
void set_base(int child) {
    threshold = 0;
	int fd, sz;
	char buf[16384], *s = buf, *line, *saveptr;
    char filename[1024];
    sprintf(filename,"/proc/%d/maps",child);
	if((fd = open(filename, O_RDONLY)) < 0) errquit("get_base/open");
	if((sz = read(fd, buf, sizeof(buf)-1)) < 0) errquit("get_base/read");
	buf[sz] = 0;
	close(fd);
	while((line = strtok_r(s, "\n\r", &saveptr)) != NULL) { s = NULL;
		if(strstr(line, " rw-p ") == NULL && strstr(line, " rwxp ") == NULL) continue;
		sscanf(line,"%lx-%lx",&main_min,&main_max);
        for(int i = 0 ; i<(main_max-main_min)/8 ; i++)
        {
            long* pos = (long*) (anchor_addr+threshold);
            void* addr = (void*) (main_min+i*8);
            int a = ptrace(PTRACE_POKETEXT, child, addr, (void*)anchor_value[i+threshold]);
            //printf("%d\n",a);
        }
        threshold += (main_max-main_min)/8;
	}
	return;
}
int main(int argc,char *argv[])
{
    unsigned char data [] = {0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30};
    int i = 0;
    pid_t child;
    int cnt = 0;
    unsigned int temp;
    unsigned long long loc;
    if(argc < 2){
        fprintf(stderr,"usage %s program\n",argv[0]);
        return -1;
    }
    if((child = fork()) < 0) errquit("fork");
    if(child == 0) 
    {
        if(ptrace(PTRACE_TRACEME,0,0,0,0) < 0) errquit("ptrace");
        execvp(argv[1],argv+1);
        errquit("execvp");
    } 
    else 
    {
        int status;
        long long counter = 0;
        if(waitpid(child,&status,0) <0 ) errquit("wait");
        ptrace(PTRACE_SETOPTIONS,child,0,PTRACE_O_EXITKILL);
        //printf out the information of the elf64
        unsigned long long entry_point = get_entry_point(argv[1]);
        printf("** program '%s' loaded. entry point 0x%llx\n",argv[1],entry_point);
        dissamble_addr(entry_point,argv[1],entry_point,entry_point);
        char cmd[1024];
        long origin_cmd[1024];
        unsigned long long b_loc[1024];
        unsigned long long index[1024];
        memset(origin_cmd,0,1024);
        memset(b_loc,0,1024);
        memset(index,0,1024);
        bool  sign = false;
        int used_ind = -1;
        struct user_regs_struct regs,anchor_regs;
        while(true)
        {   
            printf("(sdb) ");
            memset(cmd,'\0',1024);
            gets(cmd);
            if(strcmp("si",cmd) == 0)
            {
                
                if(ptrace(PTRACE_SINGLESTEP,child,0,0) <0) errquit("ptarce@parent");
                if(waitpid(child,&status,0) < 0) errquit("wait");
                if(sign)
                {
                    long breakpoint_instruction = (origin_cmd[used_ind] & ~0xFF) | 0xCC;
                    int check = ptrace(PTRACE_POKETEXT, child, (void *)index[used_ind], (void *)breakpoint_instruction);
                    sign = false;
                }
                if(status == 0)
                {
                    printf("** the target program terminated.\n");
                }
                else
                {
                    int s = ptrace(PTRACE_GETREGS,child,0,&regs);
                    int counter_ind = find_ind(index,regs.rip);
                    if(counter_ind == -1)
                    {
                        dissamble_addr(entry_point,argv[1],entry_point,regs.rip);
                    }
                    else
                    {
                        printf("** hit a breakpoint at %llx.\n",regs.rip);
                        dissamble_addr(entry_point,argv[1],entry_point,regs.rip);
                        int counter_ind = find_ind(index,regs.rip);
                        ptrace(PTRACE_POKETEXT, child, (void *)regs.rip, (void *)origin_cmd[counter_ind]);
                    }
                } 
            }
            if(strcmp("cont",cmd) == 0)
            {
                if(ptrace(PTRACE_CONT,child,0,0) <0) errquit("ptarce@parent");
                if(waitpid(child,&status,0) < 0) errquit("wait");
                if(sign)
                {
                    
                    long breakpoint_instruction = (origin_cmd[used_ind] & ~0xFF) | 0xCC;
                    int check = ptrace(PTRACE_POKETEXT, child, (void *)index[used_ind], (void *)breakpoint_instruction);
                    sign = false;
                }
                if(status == 0)
                {
                    printf("** the target program terminated.\n");
                }
                else
                {
                    int s = ptrace(PTRACE_GETREGS,child,0,&regs);
                    printf("** hit a breakpoint at %llx.\n",regs.rip-1);
                    int counter_ind = find_ind(index,regs.rip-1);
                    ptrace(PTRACE_POKETEXT, child, (void *)(regs.rip-1), (void *)origin_cmd[counter_ind]);
                    regs.rip -=1;
                    sign = true;
                    ptrace(PTRACE_SETREGS,child,0,&regs);
                    used_ind = counter_ind;
                    dissamble_addr(entry_point,argv[1],entry_point,(regs.rip));
                    
                }
            }
            if(strcmp("anchor",cmd) == 0)
            {
                //use mmap to allocate the address
                if(threshold == 0)
                {
                    anchor_addr = mmap(NULL, 4096*1024, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
                }
                get_base(child);
                int s = ptrace(PTRACE_GETREGS,child,0,&anchor_regs);
                printf("** dropped an anchor\n");
            }
            if(strcmp("timetravel",cmd) == 0)
            {
                //use mmap to allocate the address
                set_base(child);
                int s = ptrace(PTRACE_SETREGS,child,0,&anchor_regs);
                printf("** go back to the anchor point\n");
                s = ptrace(PTRACE_GETREGS,child,0,&regs);
                int counter_ind = find_ind(index,regs.rip);
                if(counter_ind == -1)
                {
                    dissamble_addr(entry_point,argv[1],entry_point,regs.rip);
                }
                else
                {
                    dissamble_addr(entry_point,argv[1],entry_point,regs.rip);
                    int counter_ind = find_ind(index,regs.rip);
                    ptrace(PTRACE_POKETEXT, child, (void *)regs.rip, (void *)origin_cmd[counter_ind]);
                }
                //set the break point
                for(int i = 0 ; i<ind ; i++)
                {
                    if(index[i] > regs.rip)
                    {
                        long breakpoint_instruction = (origin_cmd[i] & ~0xFF) | 0xCC;
                        int check = ptrace(PTRACE_POKETEXT, child, (void *)index[i], (void *)breakpoint_instruction);
                    }
                }
            }
            if(strncmp("break ",cmd,6) == 0)
            {
                //get the address location
                unsigned long long addr;
                sscanf(cmd,"break 0x%llx",&addr);
                index[ind] = addr;
                origin_cmd[ind] = ptrace(PTRACE_PEEKTEXT, child, (void *)addr, NULL);
                long breakpoint_instruction = (origin_cmd[ind] & ~0xFF) | 0xCC;
                int check = ptrace(PTRACE_POKETEXT, child, (void *)addr, (void *)breakpoint_instruction);
                printf("** set a breakpoint at 0x%llx.\n",addr);
                ind++;
            }
            
        }
    }
}