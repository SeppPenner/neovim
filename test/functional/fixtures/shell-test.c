// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#ifdef _MSC_VER
#include <Windows.h>
#define usleep(usecs) Sleep(usecs/1000)
#else
#include <unistd.h>
#endif

static void wait(void)
{
  fflush(stdout);
  usleep(10*1000);
}

static void help(void)
{
  puts("A simple implementation of a shell for testing termopen().");
  puts("");
  puts("Usage:");
  puts("  shell-test --help");
  puts("    Prints this help to stdout.");
  puts("  shell-test");
  puts("  shell-test EXE");
  puts("    Prints \"ready $ \" to stderr.");
  puts("  shell-test -t {prompt text}");
  puts("    Prints \"{prompt text} $ \" to stderr.");
  puts("  shell-test EXE \"prog args...\"");
  puts("    Prints \"ready $ prog args...\\n\" to stderr.");
  puts("  shell-test -t {prompt text} EXE \"prog args...\"");
  puts("    Prints \"{prompt text} $ progs args...\" to stderr.");
  puts("  shell-test REP {count} \"line line line\"");
  puts("    Prints \"{lnr}: line line line\\n\" to stdout {count} times.");
  puts("    I.e. for `shell-test REP 97 \"test\"'");
  puts("      0: test");
  puts("      ...");
  puts("      96: test");
  puts("  shell-test INTERACT");
  puts("    Prints \"interact $ \" to stderr, and waits for \"exit\" input.");
}

int main(int argc, char **argv)
{
  if (argc == 2 && strcmp(argv[1], "--help") == 0) {
    help();
  }

  if (argc >= 2) {
    if (strcmp(argv[1], "-t") == 0) {
      if (argc < 3) {
        fprintf(stderr,"Missing prompt text for -t option\n");
        return 5;
      } else {
        fprintf(stderr, "%s $ ", argv[2]);
        if (argc >= 5 && (strcmp(argv[3], "EXE") == 0)) {
          fprintf(stderr, "%s\n", argv[4]);
        }
      }
    } else if (strcmp(argv[1], "EXE") == 0) {
      fprintf(stderr, "ready $ ");
      if (argc >= 3) {
        fprintf(stderr, "%s\n", argv[2]);
      }
    } else if (strcmp(argv[1], "REP") == 0) {
      if (argc < 4) {
        fprintf(stderr, "Not enough REP arguments\n");
        return 4;
      }
      int count = 0;
      if (sscanf(argv[2], "%d", &count) != 1) {
        fprintf(stderr, "Invalid count: %s\n", argv[2]);
        return 4;
      }
      for (uint8_t i = 0; i < count; i++) {
        printf("%d: %s\n", (int) i, argv[3]);
      }
    } else if (strcmp(argv[1], "UTF-8") == 0) {
      // test split-up UTF-8 sequence
      printf("\xc3"); wait();
      printf("\xa5\n"); wait();

      // split up a 2+2 grapheme clusters all possible ways
      printf("ref: \xc3\xa5\xcc\xb2\n"); wait();

      printf("1: \xc3"); wait();
      printf("\xa5\xcc\xb2\n"); wait();

      printf("2: \xc3\xa5"); wait();
      printf("\xcc\xb2\n"); wait();

      printf("3: \xc3\xa5\xcc"); wait();
      printf("\xb2\n"); wait();
    } else if (strcmp(argv[1], "INTERACT") == 0) {
      char input[256];
      char cmd[100];
      int arg;
      int input_argc;

      while (1) {
        fprintf(stderr, "interact $ ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
          break;  // EOF
        }

        input_argc = sscanf(input, "%99s %d", cmd, &arg);
        if(1 == input_argc) {
          arg = 0;
        }
        if (strcmp(cmd, "exit") == 0) {
            return arg;
        } else {
          fprintf(stderr, "command not found: %s\n", cmd);
        }
      }
    } else {
      fprintf(stderr, "Unknown first argument: %s\n", argv[1]);
      return 3;
    }
    return 0;
  } else if (argc == 1) {
    fprintf(stderr, "ready $ ");
    return 0;
  } else {
    fprintf(stderr, "Missing first argument\n");
    return 2;
  }
}
