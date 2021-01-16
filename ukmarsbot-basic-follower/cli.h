
#ifndef CLI_H
#define CLI_H

typedef int command(const char * const argv[]);



const int MAX_ARGC = 16;
struct Args {
  char * argv[MAX_ARGC];
  int argc;
};


void clearLine();
int getLine();
Args cliSplit();
void cliPrompt();

#endif