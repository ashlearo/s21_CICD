#include "s21_grep.h"

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parser(int argc, char *argv[], opt *options, char *pattern);
void pattern_maker(char *argv[], opt options, char *pattern, char *str);
void reader(char *name, opt options, int pattern, char *str, regex_t regex,
            int num_file, int *exit_flag);
void f_func(char *argv[], opt options, char *pattern, char *name);

int main(int argc, char *argv[]) {
  opt options = {0};
  regex_t regex;

  if (argc == 1) {
    fprintf(stderr, "usage: grep [-ivclnhso] [-e pattern] [-f file]\n");
    fprintf(stderr, "[pattern] [file ...]\n");
  } else {
    int exit_flag = 0;
    char str[MAX_LENGTH];
    char pattern[MAX] = {0};

    parser(argc, argv, &options, pattern);
    pattern_maker(argv, options, pattern, 0);

    int a;

    if (options.i) {
      a = regcomp(&regex, pattern, REG_ICASE | REG_EXTENDED);
    } else {
      a = regcomp(&regex, pattern, REG_EXTENDED);
    }

    int file_ind;
    if (!options.f && !options.e) {
      file_ind = optind + 1;
    } else {
      file_ind = optind;
    }
    int number_of_files = argc - file_ind;

    for (int i = file_ind; i < argc; i++) {
      reader(argv[i], options, a, str, regex, number_of_files, &exit_flag);
    }

    regfree(&regex);
    if (exit_flag == 1) {
      exit(1);
    }
  }

  return 0;
}

void pattern_maker(char *argv[], opt options, char *pattern, char *str) {
  if (!options.f && !options.e) {
    strcat(pattern, argv[optind]);
  } else {
    if (pattern[0] == 0 && str) {
      strcat(pattern, str);
    } else if (str) {
      if (pattern[strlen(pattern) - 1] != '|') {
        strcat(pattern, "|");
      }
      strcat(pattern, str);
    }
  }
}

void parser(int argc, char *argv[], opt *options, char *pattern) {
  int option;
  while ((option = getopt_long(argc, argv, "e:ivclnhsf:o", 0, 0)) != -1) {
    switch (option) {
      case 'e':
        options->e = 1;
        pattern_maker(argv, *options, pattern, optarg);
        break;
      case 'i':
        options->i = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      case 'c':
        options->c = 1;
        break;
      case 'l':
        options->l = 1;
        break;
      case 'h':
        options->h = 1;
        break;
      case 'n':
        options->n = 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 'f':
        options->f = 1;
        f_func(argv, *options, pattern, optarg);
        break;
      case 'o':
        options->o = 1;
        break;
      default:
        exit(1);
        break;
    }
  }
}

void f_func(char *argv[], opt options, char *pattern, char *name) {
  FILE *f = fopen(name, "r");  // открывается файл в режиме чтения
  if (f) {                     // если файл существует
    char str[MAX];  // создаём строковый массив для хранения считываемых строк
    while (
        fgets(str, MAX,
              f)) {  // пока gets что-то считывает в строку (файл не закончился)
      if (str[strlen(str)] == '\0' && str[strlen(str) - 1] == '\n' &&
          strlen(str) > 1) {
        str[strlen(str) - 1] = '\0';
      }  // заменяем символ переноса строки на нуль-терминатор (условно говоря,
         // обрезаем оследний символ строки, если её длина больше 1)
      pattern_maker(argv, options, pattern,
                    str);  // загружаем считанную строку в патерн мэйкер
    }
    fclose(f);  // не забываем закрыть файл
  } else {
    fprintf(
        stderr, "grep: %s: No such file or directory\n",
        name);  // выводим сообщение об ошибке, в случае, если такого файла нет
  }
}

void reader(char *name, opt options, int a, char *str, regex_t regex,
            int num_file, int *exit_flag) {
  regmatch_t pmatch[2];
  FILE *f = fopen(name, "r");
  if (f) {
    int match_counter = 0;
    int line = 0;
    while ((a == 0) && fgets(str, MAX_LENGTH, f)) {
      int b = regexec(&regex, str, 2, pmatch, 0);
      line++;
      if ((b == 0 && options.v == 0) || (b != 0 && options.v == 1))
        match_counter++;
      if (((b == 0 && options.v == 0) || (b != 0 && options.v == 1)) &&
          !(options.c || options.l)) {
        if (num_file >= 2 && options.h != 1) printf("%s:", name);
        if (options.n) printf("%d:", line);

        if (options.o && !options.v) {
          char *copy_str = str;
          while (regexec(&regex, copy_str, 2, pmatch, 0) == 0) {
            printf("%.*s\n", (int)(pmatch[0].rm_eo - pmatch[0].rm_so),
                   &copy_str[pmatch[0].rm_so]);
            copy_str += pmatch->rm_eo;
          }
        } else {
          printf("%s", str);
          if (!(str[strlen(str) - 1] == '\n')) printf("\n");
        }
      }
    }
    if (options.c) {
      if (num_file >= 2 && options.h != 1) printf("%s:", name);
      if (options.l) {
        if (match_counter > 0) {
          printf("1\n");
        } else {
          printf("0\n");
        }
      } else {
        printf("%d\n", match_counter);
      }
    }
    if (options.l) {
      if (match_counter > 0) {
        printf("%s\n", name);
      }
    }
    fclose(f);
  } else {
    *exit_flag = 1;
    if (!options.s) {
      fprintf(stderr, "grep: %s: No such file or directory\n", name);
    }
  }
}
