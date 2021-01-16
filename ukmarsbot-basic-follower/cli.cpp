#include <arduino.h>
#include "cli.h"
const int INPUT_BUFFER_SIZE = 32;
// extern char inputLine[INPUT_BUFFER_SIZE];

char inputLine[INPUT_BUFFER_SIZE];
uint8_t index = 0;

void clearLine() {
  index = 0;
  inputLine[index] = 0;
}

int getLine() {
  String line_string;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      Serial.println();
      return 1;
    } else if (c == 8) {
      if (index > 0) {
        inputLine[index] = 0;
        index--;
        Serial.print(c);
        Serial.print(' ');
        Serial.print(c);
      }
    } else if (isPrintable(c)) {
      c = toupper(c);
      Serial.print(c);
      if (index < INPUT_BUFFER_SIZE - 1) {
        inputLine[index++] = c;
        inputLine[index] = 0;
      }
    } else {
      // drop the character silently
    }
  }
  return 0;
}

Args cliSplit() {
  Args args = {0};
  char* token;
  for (token = strtok(inputLine, " "); token != NULL; token = strtok(NULL, " ")) {
    args.argv[args.argc] = token;
    args.argc++;
    if (args.argc == MAX_ARGC)
      break;
  }
  return args;
}

void cliPrompt() {
  Serial.print('>');
  Serial.print(' ');
}