#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER 990

struct flags {
  int e;
  int i;  // Игнорирует различия регистра.
  int v;  // Инвертирует смысл поиска соответствий.
  int c;  // Выводит только количество совпадающих строк.
  int l;  // Выводит только совпадающие файлы.
  int n;  // Предваряет каждую строку вывода номером строки из файла ввода.
  int h;  // Выводит совпадающие строки, не предваряя их именами файлов.
  int s;  // Подавляет сообщения об ошибках о несуществующих или нечитаемых
          // файлах.
  int f;  // Получает регулярные выражения из файла.
  int o;  // Печатает только совпадающие (непустые) части совпавшей строки.
  int fail;
};

int flag_f(char *pattern, char *path);
void flag_cheking(int argc, char **argv, struct flags *meaning, char *patternE,
                  char *patternF, char *pattern);
void grep(int argc, char **argv, struct flags *meaning, char *patterE,
          char *buffer, char *pattern);
void compiler(char *path, struct flags *meaning, char *pattern,
              int count_file_state);
void reader(FILE *fp, struct flags *meaning, regex_t regex, char *path,
            int count_file_state);
void param(struct flags *meaning, int matches_of_line, char *filename,
           int state_counter);

int main(int argc, char **argv) {
  if (argc > 2) {
    struct flags meaning = {0};
    char pattern[BUFFER] = {0};
    char pattern_E[BUFFER] = {0};
    char pattern_F[BUFFER] = {0};
    flag_cheking(argc, argv, &meaning, pattern_E, pattern_F, pattern);
  } else {
    fprintf(stderr, "ERROR\n");
  }
  return 0;
}
void flag_cheking(int argc, char **argv, struct flags *meaning, char *pattern_e,
                  char *pattern_f, char *pattern) {
  int rezult = 0;
  while ((rezult = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != -1) {
    switch (rezult) {
      case 'e':
        meaning->e = 1;
        strcat(pattern_e, optarg);
        strcat(pattern_e, "|");
        break;
      case 'i':
        meaning->i = 1;
        break;
      case 'v':
        meaning->v = 1;
        break;
      case 'c':
        meaning->c = 1;
        break;
      case 'l':
        meaning->l = 1;
        break;
      case 'n':
        meaning->n = 1;
        break;
      case 'o':
        meaning->o = 1;
        break;
      case 's':
        meaning->s = 1;
        break;
      case 'h':
        meaning->h = 1;
        break;
      case 'f':
        meaning->f = 1;
        snprintf(pattern_f, BUFFER, "%s", optarg);
        break;
      default:
        meaning->fail = 1;
        break;
    }
  }
  if (meaning->fail != 1) {
    int counter = 0;
    if (meaning->e == 1) {
      for (int i = 0; pattern_e[i] != '\0'; i++) {
        counter++;
      }
      if (pattern_e[counter - 1] == '|') {
        pattern_e[counter - 1] = '\0';
      } else {
        pattern_e[counter] = '\0';
      }
    }
    grep(argc, argv, meaning, pattern_e, pattern_f, pattern);
  }
}

int flag_f(char *pattern, char *filename) {
  FILE *file;
  int error = 0, i = 0;
  int ch_1;
  if ((file = fopen(filename, "r")) != NULL) {
    while ((ch_1 = fgetc(file)) != EOF) {
      if (ch_1 == '\n') {
        pattern[i++] = '|';
      }
      if (ch_1 != 10 && ch_1 != 13) {
        pattern[i++] = ch_1;
      }
    }
    fclose(file);
  } else {
    error = 1;
  }
  if (pattern[i - 1] == '|') {
    pattern[i - 1] = '\0';
  }
  return error;
}

void grep(int argc, char **argv, struct flags *meaning, char *pattern_E,
          char *buffer, char *pattern) {
  int pattern_f = 0;
  int file_counter = 0, state_counter = 0;
  if (meaning->e != 1 && meaning->f != 1) {
    snprintf(pattern, BUFFER, "%s", argv[optind++]);
  }
  if (meaning->f == 1) {
    pattern_f = flag_f(pattern, buffer);
    if (meaning->e == 1) {
      strcat(pattern, "|");
      strcat(pattern, pattern_E);
    }
  }
  if (meaning->e == 1 && meaning->f != 1) {
    snprintf(pattern, BUFFER, "%s", pattern_E);
  }
  if (pattern_f != 1) {
    if (argc - optind > 1) {
      file_counter = 1;
    }
    for (int i = optind; i < argc; i++) {
      if (file_counter && meaning->h != 1) {
        state_counter = 1;
      }
      compiler(argv[i], meaning, pattern, state_counter);
    }
  }
}

void reader(FILE *file, struct flags *meaning, regex_t regex, char *filename,
            int state_counter) {
  char *string;
  int state = 0;
  char temp[BUFFER] = {0};
  int nline = 1, matches_of_line = 0;
  size_t nmatch = 1;
  regmatch_t pmatch[1];
  while (fgets(temp, sizeof(temp) - 1, file) != NULL) {
    int match = 0;
    int coinc = regexec(&regex, temp, nmatch, pmatch, 0);

    if (strchr(temp, '\n') == NULL) {
      strcat(temp, "\n");
    }
    if (coinc == 0 && meaning->v != 1) {
      match = 1;
    }
    if (coinc == REG_NOMATCH && meaning->v == 1) {
      match = 1;
    }
    int is_path_enter = 0;
    if (match && meaning->n == 1 && meaning->c != 1 && meaning->l != 1) {
      if (state_counter) {
        printf("%s:%d:", filename, nline);
        is_path_enter = 1;
      } else {
        printf("%d:", nline);
      }
      state = 1;
    }
    if (match && meaning->o != 1 && meaning->c != 1 && meaning->l != 1) {
      if (state_counter && is_path_enter != 1) {
        printf("%s:%s", filename, temp);
      } else {
        printf("%s", temp);
      }
    }
    if (match && meaning->o == 1 && meaning->l != 1 && meaning->c != 1) {
      if (meaning->v == 1) {
        if (state_counter) {
          if (state) {
            printf("%s", temp);
          } else {
            printf("%s:%s", filename, temp);
          }
        } else {
          printf("%s", temp);
        }
      }
      string = temp;
      while (coinc == 0) {
        if (state_counter) {
          printf("%s:", filename);
        }
        for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
          printf("%c", string[i]);
        }
        printf("\n");
        int count = 0;
        while (count != pmatch[0].rm_eo) {
          string++;
          count++;
        }
        coinc = regexec(&regex, string, nmatch, pmatch, 0);
      }
    }
    matches_of_line += match;
    nline++;
  }
  param(meaning, matches_of_line, filename, state_counter);
}

void param(struct flags *meaning, int matches_of_line, char *filename,
           int state_counter) {
  int c_only = 0;
  if (meaning->c == 1 &&
      (meaning->v != 1 && meaning->l != 1 && meaning->o != 1 &&
       meaning->e != 1 && meaning->n != 1 && meaning->h != 1 &&
       meaning->i != 1)) {
    c_only = 1;
  }
  if (c_only != 1) {
    if (state_counter != 1 && meaning->h == 1 && meaning->l == 1 &&
        (meaning->c == 1 && meaning->o != 1) && matches_of_line > 1) {
      matches_of_line = 1;
    }
    if (meaning->n == 1 && meaning->l == 1 &&
        (meaning->c == 1 && meaning->o != 1) && matches_of_line > 1) {
      matches_of_line = 1;
    }
    if (meaning->v == 1 && meaning->l == 1 &&
        (meaning->c == 1 && meaning->o != 1) && matches_of_line > 1) {
      matches_of_line = 1;
    }
  }

  if (meaning->c == 1) {
    state_counter ? printf("%s:%d\n", filename, matches_of_line)
                  : printf("%d\n", matches_of_line);
  }
  if (meaning->l == 1 && matches_of_line > 0) {
    printf("%s\n", filename);
  }
}

void compiler(char *filename, struct flags *meaning, char *pattern,
              int state_counter) {
  regex_t regex;
  FILE *file;
  int flag_temp = REG_EXTENDED;
  if (meaning->i == 1) {
    flag_temp = REG_ICASE;
  }
  if ((file = fopen(filename, "r")) != NULL) {
    regcomp(&regex, pattern, flag_temp);
    reader(file, meaning, regex, filename, state_counter);
    regfree(&regex);
    fclose(file);
  } else {
    if (meaning->s != 1) {
      fprintf(stderr, "No file\n");
    }
  }
}
