#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* secureCheck;

void putFlag() { puts(getenv("FLAG")); }

void meow() {
  secureCheck = __builtin_extract_return_addr(__builtin_return_address(0));
  char name[0x10] = {0};
  puts("        .");
  puts("       -.\\_.--._.______.-------.___.---------.___");
  puts("       )`.                                       `-._");
  puts("      (                                              `---.");
  puts("      /o                                                  `.");
  puts("     (                                                      \\");
  puts("   _.'`.  _                                                  L");
  puts("   .'/| \"\" \"\"\"\"._                                            |");
  puts("      |          \\             |                             J");
  puts("                  \\-._          \\                             L");
  puts("                  /   `-.        \\                            J");
  puts("                 /      /`-.      )_                           `");
  puts("                /    .-'    `    J  \"\"\"\"-----.`-._             |\\");
  puts("              .'   .'        L   F            `-. `-.___        \\`.");
  puts("           ._/   .'          )  )                `-    .'\"\"\"\"`.  \\)");
  puts("__________((  _.'__       .-'  J              _.-'   .'        `. \\");
  puts("                   \"\"\"\"\"\"\"((  .'--.__________(   _.-'___________)..|----------------._____");
  puts("                            \"\"                \"\"\"               ``U'");
  puts("-----._______");
  puts("             `-------.__________.--");
  puts("                       `-.                      ______________.---------------------------");
  puts("                          `----'`---------'----'");

  puts("What's your name?");
  read(0, name, 0x40);
  if (__builtin_extract_return_addr(__builtin_return_address(0)) != secureCheck) {
    puts("NONONO, don't hack this cute cat!");
    abort();
  }
}

int main() {
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  meow();
  return 0;
}
