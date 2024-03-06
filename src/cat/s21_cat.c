#include "s21_cat.h"

int main(int argc, char* argv[]) {
  Config flags = parser(argc, argv);

  if (optind < argc) {
    while (optind < argc) {
      cat(argv[optind], &flags);
      optind++;
    }
  } else {
    process_flags(stdin, &flags);
  }

  return 0;
}

Config parser(int argc, char** argv) {
  Config flags = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1};

  int opt;
  int option_index;
  const char* short_options = "benstvET";

  while ((opt = getopt_long(argc, argv, short_options, long_options,
                            &option_index)) != -1) {
    switch (opt) {
      case 'b':
        flags.b = 1;
        break;
      case 'e':
        flags.e = 1;
        flags.v = 1;
        break;
      case 'n':
        flags.n = 1;
        break;
      case 's':
        flags.s = 1;
        break;
      case 't':
        flags.t = 1;
        flags.v = 1;
        break;
      case 'T':
        flags.t = 1;
        break;
      case 'E':
        flags.e = 1;
        break;
      case 'v':
        flags.v = 1;
        break;
      default:
        exit(EXIT_FAILURE);
    }
  }
  return flags;
}

void process_flags_b_n(Config* flags, const int* c) {
  if (flags->b) {
    if (flags->new_line && flags->b && *c != '\n') {
      printf("%6d\t", flags->next_line_number++);
    }
    if (*c == EOF) flags->new_line = 0;
    if (*c == '\n') {
      flags->new_line = 1;
    } else {
      flags->new_line = 0;
    }
  } else if (flags->n) {
    if (flags->new_line_n && flags->n) {
      printf("%6d\t", flags->next_line_number++);
    }
    if (*c == EOF) flags->new_line_n = 0;
    if (*c == '\n') {
      flags->new_line_n = 1;
    } else {
      flags->new_line_n = 0;
    }
  }
}

void process_flags_t(Config* flags, int* c) {
  if (flags->t) {
    if (*c == '\t') {
      printf("^");
      *c = *c + 64;
    }
  }
}

void process_flags_e(Config* flags, const int* c) {
  if (flags->e) {
    if (*c == '\n') {
      printf("$");
    }
  }
}

void process_flags_v(int* c) {
  if (*c == 9 || *c == 10) {
    printf("%c", *c);
  } else if (*c >= 32 && *c < 127) {
    printf("%c", *c);
  } else if (*c == 127) {
    printf("^?");
  } else if (*c >= 128 + 32) {
    printf("M-");
    (*c < 128 + 127) ? printf("%c", *c - 128) : printf("^?");
  } else {
    (*c > 32) ? printf("M-^%c", *c - 128 + 64) : printf("^%c", *c + 64);
  }
}

int process_flags_s(Config* flags, const int* c) {
  if (flags->s) {
    if (*c == '\n') {
      if (flags->enter_count >= 2) {
        flags->enter_count = 2;
        return 1;
      }
      (flags->enter_count)++;
    } else {
      flags->enter_count = 0;
    }
  }
  return 0;
}

void process_flags(FILE* file, Config* flags) {
  int c;

  while ((c = fgetc(file)) != EOF) {
    int skip_iteration = process_flags_s(flags, &c);
    if (skip_iteration) continue;

    process_flags_b_n(flags, &c);
    process_flags_t(flags, &c);
    process_flags_e(flags, &c);

    if (flags->v) {
      process_flags_v(&c);
    } else {
      putchar(c);
    }
  }
}

void cat(char* filename, Config* flags) {
  FILE* file = fopen(filename, "r");

  if (file == NULL) {
    fprintf(stderr, "s21_cat: %s: No such file or directory", filename);
    return;
  }

  process_flags(file, flags);

  fclose(file);
}