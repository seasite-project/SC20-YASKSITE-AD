#ifndef _OFFSITE_RUN_PARSE_H
#define _OFFSITE_RUN_PARSE_H

#include <vector>
#include <getopt.h>

struct my_option
{
    option gnu_opt;
    char* desc;
    my_option(const char*name, int has_arg, int *flag, int val, char* desc);
    my_option();
};

char** str_split(char* a_str, const char a_delim);
std::vector<int> getRange(char* range);
std::vector<char*> splitChar(char* range, const char delim=':');
int findChar(std::vector<char*> vec, const char* str);

struct os_parser
{
    int iter;
    int cores;
    char* size;
    char* path;
    char *prgname;
    int corrector_steps;
    int stages;
    char* fold;
    char* opt;
    char* kernel;
    int radius;
    char* machine;
    int numOptions;
    my_option *long_options;
    option *gnuOptions;
    os_parser();
    ~os_parser();
    bool parse_arg(int argc, char **argv);
    int dump_arg();
    void help();
};

#endif
