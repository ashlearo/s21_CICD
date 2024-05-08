#include "s21_cat.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

void parser(int argc, char *argv[], opt *options);
void reader(char *name, opt options);

int main(int argc, char *argv[]) {
  if (argc == 1) {
    fprintf(stderr, "not option\n");
    fprintf(stderr, "usage: s21_cat [-benstuv] [file ...]\n");
    exit(1);
  }
  opt options = {0};
  parser(argc, argv, &options);
  for (int i = optind; i < argc; i++) {
    reader(argv[i], options);
  }
  return 0;
}

void parser(int argc, char *argv[], opt *options) {
  int option;
  int option_index;
  const struct option long_options[] = {{"number-nonblank", 0, 0, 'b'},
                                        {"number", 0, 0, 'n'},
                                        {"squeeze-blank", 0, 0, 's'},
                                        {0, 0, 0, 0}};

  while ((option = getopt_long(argc, argv, "+benstvET", long_options,
                               &option_index)) != -1) {
    switch (option) {
      case 'b':
        options->b = 1;
        break;
      case 'e':
        options->e = 1;
        options->v = 1;
        break;
      case 'E':
        options->e = 1;
        break;
      case 'n':
        options->n = 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 't':
        options->t = 1;
        options->v = 1;
        break;
      case 'T':
        options->t = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      default:
        printf("n/a\n");
        exit(1);
        break;
    }
  }
  if (options->b == 1) {
    options->n = 0;
  }
}

void reader(char *name, opt options) {
  (void)options;
  FILE *f = fopen(name, "r");
  if (f) {
    char cur;
    char prev = '\n';
    int str_count = 0;
    int empty_count = 0;
    while ((cur = fgetc(f)) != EOF) {
      // -s flag

      if (cur == '\n' && prev == '\n') {
        empty_count++;
      } else {
        empty_count = 0;
      }
      if (empty_count > 1 && cur == '\n' && options.s == 1) {
        continue;
      }

      // -b and -n flags

      if (prev == '\n' && (options.n || (options.b && cur != '\n'))) {
        printf("%6d\t", ++str_count);
      }

      // -e flag

      if (cur == '\n' && options.e) {
        printf("$");
      }

      // -v and -t flags
      if (cur < 32 && cur != '\t' && cur != '\n' && options.v) {
        printf("^%c", cur + 64);
      } else if (cur == 127 && options.v) {
        printf("^?");
      } else if (cur == '\t' && options.t) {
        printf("^I");
      } else {
        printf("%c", cur);
      }
      prev = cur;
    }

    fclose(f);
  } else {
    fprintf(stderr, "cat: %s: No such file or directory \n", name);
  }
}
