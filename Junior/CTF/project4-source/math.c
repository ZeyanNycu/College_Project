#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void clearInput(char buf[5]) {
  int remain = 1;
  for (int i = 0; i < 5; i++) {
    if (buf[i] == '\n') {
      remain = 0;
      buf[i] = '\0';
    }
  }
  if (remain) {
    int c;
    while ((c = getchar()) && c != '\n') {
    }
  }
}

int main() {
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  unsigned int myNumber = 77697987;
  char buf[5] = {0};
  printf("My number is %d\nInput a number larger than me: ", myNumber);
  read(0, buf, 4);
  clearInput(buf);

  int yourNumber = strtol(buf, NULL, 10);

  if (myNumber > yourNumber) {
    printf("77697987 > %d, you lose!\n", yourNumber);
  } else {
    printf("%d > 77697987 !?\n", yourNumber);
    puts(getenv("FLAG"));
  }
  return 0;
}
