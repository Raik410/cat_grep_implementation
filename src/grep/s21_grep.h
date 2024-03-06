#ifndef GREP_H
#define GREP_H
#define _POSIX_C_SOURCE 200809L
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define _GNU_SOURCE

#define size_str 1024
#define MAX_GROUPS 10
#define short_options "e:ivclnhsf:o"

typedef struct {
  char e;  // несколько шаблонов запроса
  int count_e;
  char i;  // поиск без учета регистра
  char v;  // не соответсвующее шаблону
  char c;  // количество совпадений
  char l;  // только имя файлов, в которых найден шаблон
  char n;  // номера найденных строк
  char h;  // выводит совпадающие строки, не предваряя их именами файлов
  char s;  // подавляет сообщения об ошибках о несуществующих или нечитаемых
           // файлах.
  char f;  // файл шаблонов
  char o;  // печатает только совпадающие (непустые) части совпавшей строки.
  char restr[size_str];
  char multiple_files;
  int line_num;
  int line_count;
  char err;
} grep_flags;

grep_flags get_grep_flags_struct(int argc, char *argv[]);
void process_flag_f(char *filename, grep_flags *flags);
void process_options(grep_flags *flags, int argc, char *argv[]);
regex_t get_regex(grep_flags *flags);
void do_grep(char *filename, grep_flags *flags, regex_t *regex_comiled);
void process_line(char *line, grep_flags *flags, regex_t *regex_comiled,
                  char *filename);
void line_print(char *line, grep_flags *flags, int offset, int length,
                char *filename);

#endif
