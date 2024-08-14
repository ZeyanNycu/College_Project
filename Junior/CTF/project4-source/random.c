#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void printTerminalTime(time_t t) {
  char buffer[32];
  struct tm* tm_info = localtime(&t);
  strftime(buffer, 32, "%H:%M:%S > ", tm_info);
  printf("%s", buffer);
}

int main() {
  setbuf(stdout, NULL);
  time_t timer = time(NULL);
  srand((uint32_t)timer);
  printTerminalTime(timer);
  printf("%s", "Please enter your password: ");

  uint32_t input = 0;
  scanf("%u", &input);
  if (input == rand()) {
    puts(getenv("FLAG"));
  } else {
    printTerminalTime(time(NULL));
    puts("Access denied!");
  }

  return 0;
}
