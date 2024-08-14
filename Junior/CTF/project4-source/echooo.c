#include <stdio.h>
#include <string.h>
#include <unistd.h>

int canLogin;

int main() {
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  char buf[128] = {0};
  printf("What is your name: ");
  read(0, buf, 127);
  buf[strcspn(buf, "\r\n")] = 0;
  printf(buf);
  if (canLogin) {
    printf(", here is your flag: \n");
    puts(getenv("FLAG"));
  } else
    printf(", you cannot get the flag\n");
}
