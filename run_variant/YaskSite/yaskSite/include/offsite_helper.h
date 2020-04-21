#ifndef _OFFSITE_HELPER_
#define _OFFSITE_HELPER_

#include <yaskSite.h>
#include <vector>
#include <string>

std::vector<int> getFoldFromKernel(char *kernel_name);
std::string getOptimisationFromKernel(char* kernel_name);
yaskSite* createStencil(const char *kenrel_name, MPI_Manager* mpiMan, std::vector<int> size, int ncores, std::vector<int> fold, char* opt, int radius);

#endif
