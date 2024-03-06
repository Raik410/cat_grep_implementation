#include "s21_grep.h"

int main(int argc, char *argv[]) {
  // Парсинг флагов grep
  grep_flags flags = get_grep_flags_struct(argc, argv);

  // Обработка дополнительных условий
  process_options(&flags, argc, argv);
  if (flags.err) exit(1);

  // Компиляция регулярного выражения
  regex_t regex_compiled = get_regex(&flags);
  if (flags.err) {
    regfree(&regex_compiled);
    exit(1);
  }

  // Обработка файлов
  for (int i = optind; i < argc; i++) {
    char *filename = argv[i];
    do_grep(filename, &flags, &regex_compiled);
  }

  regfree(&regex_compiled);
  return 0;
}

// Получение флагов утилиты grep
grep_flags get_grep_flags_struct(int argc, char *argv[]) {
  grep_flags flags = {0};
  flags.line_num = 1;

  int rez = 0;

  while ((rez = getopt(argc, argv, short_options)) != -1) {
    switch (rez) {
      case 'e':
        // Склевивание аргументов флага -e
        if (flags.e || flags.f) strcat(flags.restr, "\\|");
        strcat(flags.restr, optarg);
        flags.e = 1;
        break;
      case 'i':
        flags.i = 1;
        break;
      case 'v':
        flags.v = 1;
        break;
      case 'c':
        flags.c = 1;
        break;
      case 'l':
        flags.l = 1;
        break;
      case 'n':
        flags.n = 1;
        break;
      case 'h':
        flags.h = 1;
        break;
      case 's':
        flags.s = 1;
        break;
      case 'f':
        process_flag_f(optarg, &flags);
        flags.f = 1;
        break;
      case 'o':
        flags.o = 1;
        break;
      default:
        flags.err = 1;
    }
  }

  return flags;
}

void process_flag_f(char *filename, grep_flags *flags) {
  // Открытие файла
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "File %s not found\n", filename);
    flags->err = 1;
    return;
  }

  // Создание переменных для чтения файла
  char *line = NULL;
  size_t buf_len = 0;
  ssize_t line_len;

  while ((line_len = getline(&line, &buf_len, file)) != -1) {
    // Удаление символа переноса в конце строки
    if (line[strlen(line) - 1] == '\n') {
      line[strlen(line) - 1] = '\0';
    }

    // Склеивание регулярных выражений
    if (flags->e || flags->f) strcat(flags->restr, "\\|");
    strcat(flags->restr, line);
  }

  // Очистка памяти
  fclose(file);
  free(line);
}

void process_options(grep_flags *flags, int argc, char *argv[]) {
  // Приоритетность флагов
  if (flags->l) {
    flags->c = 0;
    flags->o = 0;
  }
  if (flags->c) {
    flags->o = 0;
  }

  // Проверка на совместимость флагов
  if (flags->v && flags->o) {
    fprintf(stderr, "./s21_grep: invalid options -- 'v' and -- 'o'");
    flags->err = 1;
  }

  // Проверка на наличие правильных полей (их количества)
  if (optind + 1 - (flags->f || flags->e) >= argc) flags->err = 1;

  // Получение регулярного выражения базового сценария (без -e -f)
  if (!flags->f && !flags->e && !flags->err) {
    strcat(flags->restr, argv[(optind)++]);
  }

  // Наличие нескольких файлов поиска
  if ((argc - optind) > 1) flags->multiple_files = 1;
}

regex_t get_regex(grep_flags *flags) {
  regex_t regex_comiled;
  int reflags = flags->i ? REG_ICASE : 0 | REG_NEWLINE;

  if (regcomp(&regex_comiled, flags->restr, reflags)) {
    fprintf(stderr, "Could not compile regex\n");
    flags->err = 1;
  }
  return regex_comiled;
}

void do_grep(char *filename, grep_flags *flags, regex_t *regex_comiled) {
  // Открытие файла
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "File %s not found\n", filename);
    flags->err = 1;
    return;
  }

  // Переменные для построчного чтение
  char *line = NULL;
  size_t buf_len = 0;
  ssize_t line_len;

  while ((line_len = getline(&line, &buf_len, file)) != -1) {
    // Обработка строки
    process_line(line, flags, regex_comiled, filename);

    // Если флаг -l и найдены строки совпадения, то вывести название файла и
    // прекратить поиск
    if (flags->l && flags->line_count) {
      printf("%s\n", filename);
      break;
    }
    flags->line_num++;
  }

  if (flags->c) {
    // Если несколько файлов и нет флага -h
    if (flags->multiple_files && !flags->h) {
      printf("%s:%d\n", filename, flags->line_count);
    } else {
      printf("%d\n", flags->line_count);
    }
  }

  flags->line_num = 1;
  flags->line_count = 0;

  free(line);
  fclose(file);
}

void process_line(char *line, grep_flags *flags, regex_t *regex_comiled,
                  char *filename) {
  int match = 0;

  int str_offset = 0;
  int str_len = 0;
  regmatch_t matches[MAX_GROUPS];
  do {
    match = !regexec(regex_comiled, line, MAX_GROUPS, matches, 0);

    if (match != flags->v) {
      flags->line_count++;
    }

    if (flags->l || flags->c) continue;

    if (flags->o && match) {
      str_offset = matches[0].rm_so;
      str_len = matches[0].rm_eo - matches[0].rm_so;
    }

    if ((flags->v && !match) || (!flags->v && match)) {
      line_print(line, flags, str_offset, str_len, filename);
    }

    if (flags->o) line = line + matches[0].rm_eo;
  } while (flags->o && match);
}

void line_print(char *line, grep_flags *flags, int offset, int length,
                char *filename) {
  if (flags->multiple_files && !flags->h) {
    printf("%s:", filename);
  }

  if (flags->n) {
    printf("%d:", flags->line_num);
  }

  if (!offset && !length) {
    printf("%s", line);
  } else {
    printf("%.*s", (int)length, &line[offset]);
  }

  if (flags->o == 1 || (line[strlen(line) - 1] -= '\n')) {
    printf("\n");
  }
}