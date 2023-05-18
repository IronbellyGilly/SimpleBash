#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct flags {
  int b;  // нумерует только непустые строки
  int e;  // также отображает символы конца строки как $
  int n;  // нумерует все выходные строки
  int s;  // сжимает несколько смежных пустых строк
  int t;  // также отображает табы как ^I
  int v;  // как е
  int fail;
};

void flag_checking(struct flags *meaning, int argc, char **argv);
void file_opening(int argc, char **argv, struct flags *meaning);
void printing(int *starting, int *numeration, struct flags *meaning, char ch_1,
              char *ch_2);

int main(int argc, char **argv) {
  struct flags meaning = {0};
  flag_checking(&meaning, argc, argv);
  return 0;
}

void flag_checking(struct flags *meaning, int argc, char **argv) {
  int rezult = 0;
  int option_index;
  struct option long_flags[] = {{"number-nonblank", no_argument, NULL, 'b'},
                                {"squeeze-blank", no_argument, NULL, 's'},
                                {"number", no_argument, NULL, 'n'},
                                {NULL, 0, NULL, 0}};
  while ((rezult = getopt_long(argc, argv, "+beEnsvtT", long_flags,
                               &option_index)) != -1) {
    switch (rezult) {
      case 'b':
        meaning->b = 1;
        break;
      case 'e':
        meaning->e = 1;
        meaning->v = 1;
        break;
      case 'E':
        meaning->e = 1;
        break;
      case 'n':
        meaning->n = 1;
        break;
      case 's':
        meaning->s = 1;
        break;
      case 't':
        meaning->t = 1;
        meaning->v = 1;
        break;
      case 'T':
        meaning->t = 1;
        break;
      case 'v':
        meaning->v = 1;
        break;
      case '?':
        meaning->fail = 1;
        break;
    }
  }
  if (meaning->fail == 1) {
    fprintf(stderr, "ERROR");
  } else {
    if (meaning->b == 1 && meaning->n == 1) {
      meaning->n = 0;
    }
    file_opening(argc, argv, meaning);
  }
}

void file_opening(int argc, char **argv, struct flags *meaning) {
  FILE *fp;
  int starting = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      if ((fp = fopen(argv[i], "r")) != NULL) {
        int numeration = 1;
        char ch_1;
        char ch_2[3] = {' ', ' ', '\n'};
        while ((ch_1 = getc(fp)) != EOF) {
          ch_2[0] = ch_2[1];
          ch_2[1] = ch_2[2];
          ch_2[2] = ch_1;
          if ((ch_2[0] != '\n' || ch_2[1] != '\n') ||
              (ch_2[2] != '\n' || meaning->s != 1)) {
            printing(&starting, &numeration, meaning, ch_1, ch_2);
          }
        }
        fclose(fp);
      } else {
        fprintf(stderr, "%s: file not found\n", argv[i]);
      }
    }
  }
}

void printing(int *starting, int *numeration, struct flags *meaning, char ch_1,
              char *ch_2) {
  int flag_value = 0;
  if (*starting == 0) {
    if (meaning->b == 1) {
      if (ch_2[2] != '\n' && ch_2[1] == '\n') {
        flag_value = 1;
      }
    } else if (meaning->n == 1) {
      flag_value = 1;
    }
  }
  if (flag_value == 1) {
    printf("%6d\t", *numeration);
    *numeration += 1;
  }
  if (ch_1 == '\n') {
    *starting = 0;
    if (meaning->e == 1) {
      printf("$");
    }
  } else {
    *starting = 1;
  }
  if (ch_1 == '\t' && (meaning->t == 1)) {
    printf("^I");
    flag_value = 2;
  }
  if (meaning->v == 1) {
    if (ch_1 != '\t' && ch_1 != '\n') {
      if ((ch_1 < 32 && ch_1 != 9) && ch_1 != 10) {
        printf("^%c", ch_1 + 64);
        flag_value = 2;
      } else if (ch_1 == 127) {
        printf("^%c", ch_1 - 64);
        flag_value = 2;
      }
    }
  }
  if (flag_value != 2) {
    printf("%c", ch_1);
  }
}
