#ifndef S21_CAT
#define S21_CAT
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
  int next_line_number;
  int enter_count;
  int new_line;
  int new_line_n;
} Config;

const struct option long_options[] = {
    {"number-nonblank", no_argument, NULL, 'b'},
    {"number", no_argument, NULL, 'n'},
    {"squeeze-blank", no_argument, NULL, 's'},
    {NULL, 0, NULL, 0}};

Config parser(int argc, char** argv);
void process_flags_b_n(Config* flags, const int* c);
void process_flags_t(Config* flags, int* c);
void cat(char* filename, Config* flags);
void process_flags(FILE* file, Config* flags);
void process_flags_e(Config* flags, const int* c);
void process_flags_v(int* c);
int process_flags_s(Config* flags, const int* c);

#endif