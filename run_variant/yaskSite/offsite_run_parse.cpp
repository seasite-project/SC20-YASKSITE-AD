#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "offsite_run_parse.h"

my_option::my_option(const char* name_, int has_arg_, int* flag_, int val_, char*desc_): desc(desc_)
{
    gnu_opt = {name_, has_arg_, flag_, val_};
}

my_option::my_option()
{
}

os_parser::os_parser():iter(100), cores(1), size(NULL), path("default"), prgname("a.out"), corrector_steps(1), stages(1), fold("auto"),opt("plain"), kernel(""), radius(-1), machine(NULL), numOptions(13)
{
    long_options = new my_option[numOptions+1];

    long_options[0]  = {"iter",    required_argument, 0,  'i', "Iterations to be carried out" };
    long_options[1]  = {"cores",   required_argument, 0,  'c', "Number of cores to be used" };
    long_options[2]  = {"size",   required_argument, 0,  's', "Domain size g_z:g_y:g_x " };
    long_options[3]  = {"path",    required_argument, 0,  'p', "Path valid values default or serpentine" };
    long_options[4]  = {"corrector_step",    required_argument, 0,  'C', "number of corrector steps (m)" };
    long_options[5]  = {"stage",    required_argument, 0,  'S', "number of stages (s)" };
    long_options[6]  = {"fold",    required_argument, 0,  'f', "Vector Folding L_z:L_y:L_x" };
    long_options[7] =  {"opt",     required_argument, 0,  'o', "Optimisations [options: plain,spatial,temporal]; use ':' for combining"};
    long_options[8]  = {"kernel",    required_argument, 0,  'k', "kernel name" };
    long_options[9]  = {"radius",    required_argument, 0,  'r', "radius of stencil" };
    long_options[10]  = {"machine",    required_argument, 0,  'm', "machine file" };
    long_options[11] = {"help",    no_argument,       0,  'h', "Prints this help informations" };
    long_options[12] = {0,         0,                 0,   0 ,  0 };

    gnuOptions = new option[numOptions+1];

    for(int i=0; i<numOptions+1; ++i)
    {
        gnuOptions[i] = long_options[i].gnu_opt;
    }
}

os_parser::~os_parser()
{
    delete[] long_options;
    delete[] gnuOptions;
}

bool os_parser::parse_arg(int argc, char **argv)
{
    prgname = argv[0];
    while (1) {
        int option_index = 0, c;
        c = getopt_long(argc, argv, "0:i:c:s:p:C:S:f:o:k:r:m:h",
                gnuOptions, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 0:
                {
                    printf("No parameters specified.\n To see options use %s -h\n", prgname);
                    return false;
                    break;
                }
            case 'i':
                {
                    iter = atoi(optarg);
                    break;
                }
            case 'c':
                {
                    cores = atoi(optarg);
                    break;
                }
            case 's':
                {
                    size = optarg;
                    break;
                }
            case 'p':
                {
                    path = optarg;
                    break;
                }
            case 'C':
                {
                    corrector_steps = atoi(optarg);
                    break;
                }
            case  'S':
                {
                    stages = atoi(optarg);
                    break;
                }
            case 'f':
                {
                    fold = optarg;
                    break;
                }
            case 'o':
                {
                    opt = optarg;
                    break;
                }
            case  'k':
                {
                    kernel = optarg;
                    break;
                }
            case  'r':
                {
                    radius = atoi(optarg);
                    break;
                }
            case  'm':
                {
                    machine = optarg;
                    break;
                }
            case 'h':
                {
                    help();
                    return 0;
                    break;
                }
            default:
                break;
        }
    }

    if(size == NULL)
    {
        printf("Domain size not specified\n");
    }
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    count += last_comma < (a_str + strlen(a_str) - 1);
    count++;

    result = (char **) malloc(sizeof(char*) * count);
    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            if(idx > count) {
                printf("Error in parsing\n");
            };
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        if(idx != count - 1) {
            printf("Error in parsing\n");
        };

        *(result + idx) = 0;
    }

    return result;
}

std::vector<int> getRange(char* range)
{
    char* range_dup = strdup(range);
    char** splitted_range = str_split(range_dup, ':');

    std::vector<int> rangeVec;

    int i=0;
    while(splitted_range[i])
    {
        rangeVec.push_back(atoi(splitted_range[i]));
        ++i;
    }

    free(range_dup);
    free(splitted_range);
    return rangeVec;
}

std::vector<char*> splitChar(char* range, const char delim)
{
    char* range_dup = strdup(range);
    char** splitted_range = str_split(range_dup, delim);

    std::vector<char*> rangeVec;

    int i=0;
    while(splitted_range[i])
    {
        rangeVec.push_back(splitted_range[i]);
        ++i;
    }

    free(range_dup);
    free(splitted_range);
    return rangeVec;
}

/*checks if a string is present in the vector and returns the idx*/
int findChar(std::vector<char*> vec, const char* str)
{
    int ret = -1;
    for(int i=0; i<((int)vec.size()); ++i)
    {
        if(strcmp(vec[i],str)==0)
        {
            ret = i;
        }
    }

    return ret;
}

void os_parser::help()
{
    printf("Usage: %s [OPTION]...\n",prgname);
    printf("Valid options are:\n\n");
    char* HLINE = "────────────────────────────────────────────────────────────────────────────────────────────────────────";
    printf("%s\n",HLINE);
    printf("\t%s\t\t\t%s\n", "options", "description");
    printf("%s\n",HLINE);
    for(int i=0; i<numOptions-1; ++i)
    {
        char* long_opt;
        asprintf(&long_opt,"--%s", long_options[i].gnu_opt.name);
        printf("-%c or  %-18s |\t %-70s |\n", ((char) long_options[i].gnu_opt.val), long_opt, long_options[i].desc);
        free(long_opt);
    }

    printf("%s\n\n", HLINE);
    exit(0);
}
