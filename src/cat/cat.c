#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct options {
   bool show_end;        //-e
  bool show_all;        //-v
  bool show_tabs;       //-t
  bool squeezed;        //-s
  bool non_blank_rows;  //-b
  bool all_rows;        //-n
  int empty_lines;
  int count;
} options;

void cat_print(FILE *fp, options *s_opt);
void cat_file(int argc, char *argv[], options *s_opt);
void parse_options(int argc, char *argv[], options *s_opt);
void parse_short(char *arg, options *s_opt);
void parse_long(char *argv, options *s_opt);
void cat_cook(int *c, int *prev, options *s_opt, FILE *fp);
void cook_not_printed(int *c, int *prev, options *s_opt);
void cook_empty(int *c, int *prev, options *s_opt);
void cook_squeezed(int *c, FILE *fp, options *s_opt);
void cook_numbers(int prev, options *s_opt);

int main(int argc, char *argv[]) {
  if (argc == 1)
    cat_print(stdin, NULL);
  else {
    if (argv[1][0] == '-') {
      options s_opt = {0};
      parse_options(argc, argv, &s_opt);
      if (!s_opt.count)
        fprintf(stderr, "invalid options\n");
      else {
        // printf("parsed options successfully: %d\n", s_opt.count);
        cat_file(argc, argv, &s_opt);
      }
    } else
      cat_file(argc, argv, NULL);
  }
  return 0;
}

void cat_print(FILE *fp, options *s_opt) {
  int c;
  int prev = '\n';
  if (s_opt) {
    s_opt->empty_lines = 0;
    s_opt->count = 1;
  }
  while ((c = fgetc(fp)) != EOF) {
    if (s_opt) cat_cook(&c, &prev, s_opt, fp);
    if (c != EOF) fputc(c, stdout);
    prev = c;
  }
}

void cat_cook(int *c, int *prev, options *s_opt, FILE *fp) {
  cook_empty(c, prev, s_opt);
  cook_squeezed(c, fp, s_opt);
  if (*c != EOF) {
    cook_numbers(*prev, s_opt);
    cook_not_printed(c, prev, s_opt);
  }
}

void cat_file(int argc, char *argv[], options *s_opt) {
  int i = s_opt ? 1 + s_opt->count : 1;
  for (; i < argc; i++) {
    FILE *fp = fopen(argv[i], "r");
    if (fp) {
      cat_print(fp, s_opt);
      fclose(fp);
    } else
      fprintf(stderr, "%s: no such file or directory\n", argv[i]);
  }
}

void parse_options(int argc, char *argv[], options *s_opt) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      s_opt->count++;
      if (!strstr(argv[i] + 1, "-"))
        parse_short(argv[i], s_opt);
      else if (argv[i][1] == '-')
        parse_long(argv[i], s_opt);
    }
    if (!s_opt->count) break;
  }
}

void parse_short(char *arg, options *s_opt) {
  int len = strlen(arg);
  for (int j = 1; j < len; j++) {
    if (s_opt->count) {
      switch (arg[j]) {
        case 'v':
          s_opt->show_all = true;
          break;

        case 'e':
          s_opt->show_end = true;
          s_opt->show_all = true;
          break;

        case 'E':
          s_opt->show_end = true;
          break;

        case 'T':
          s_opt->show_tabs = true;
          break;

        case 't':
          s_opt->show_tabs = true;
          s_opt->show_all = true;
          break;

        case 's':
          s_opt->squeezed = true;
          break;

        case 'n':
          s_opt->all_rows = true;
          break;

        case 'b':
          s_opt->non_blank_rows = true;
          break;

        default:
          s_opt->count = 0;
          break;
      }
    } else
      break;
  }
}

void parse_long(char *argv, options *s_opt) {
  if (!strcmp(argv + 2, "number-nonblank"))
    s_opt->non_blank_rows = true;
  else if (!strcmp(argv + 2, "number"))
    s_opt->all_rows = true;
  else if (!strcmp(argv + 2, "squeeze-blank"))
    s_opt->squeezed = true;
  else
    s_opt->count = 0;
}

void cook_not_printed(int *c, int *prev, options *s_opt) {
  if (*c == '\n') {
    if (s_opt->show_end) {
      if (s_opt->non_blank_rows && *prev == '\n') {
        printf("%8s", "\t$");
      } else if (s_opt->non_blank_rows && *prev != '\n')
        printf("$");
      else if (!s_opt->non_blank_rows)
        printf("$");
    }
  } else if (*c == '\t') {
    if (s_opt->show_tabs) {
      printf("^");
      *c = 'I';
    }
  } else if (s_opt->show_all) {
    if (*c <= 31) {
      printf("^");
      *c += 64;
    } else if (*c == 127) {
      printf("^");
      *c = '?';
    } else if (*c >= 128 && *c < 128 + 32) {
      printf("M-^");
      *c -= 64;
    }
  }
}

void cook_empty(int *c, int *prev, options *s_opt) {
  if (*prev == '\n' && *c == '\n')
    s_opt->empty_lines++;
  else
    s_opt->empty_lines = 0;
}

void cook_squeezed(int *c, FILE *fp, options *s_opt) {
  if (s_opt->squeezed && s_opt->empty_lines > 1) {
    while (*c == '\n') *c = fgetc(fp);
    s_opt->empty_lines = 0;
  }
}

void cook_numbers(int prev, options *s_opt) {
  if (prev == '\n' && (s_opt->all_rows || s_opt->non_blank_rows)) {
    if (!(s_opt->non_blank_rows && s_opt->empty_lines > 0)) {
      printf("%6d\t", s_opt->count);
      s_opt->count++;
    }
  }
}
