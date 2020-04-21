#include "offsite_helper.h"
#include <yaskSite.h>

std::vector<int> getFoldFromKernel(char *kernel_name)
{
    std::vector<int> fold;
    std::string::size_type pos = std::string(kernel_name).find(std::string("folding"));

    if(pos == std::string::npos)
    {
        int vecLen = 1;
        if(strcmp(INSTR, "AVX")==0)
        {
            vecLen = 4;
        }
        else if(strcmp(INSTR, "AVX512")==0)
        {
            vecLen = 8;
        }
        fold = {vecLen,1,1};
    }
    else
    {
        std::string kernel_fold = std::string(kernel_name).substr(pos+7, 5);
        printf("check kernel fold = %s\n", kernel_fold.c_str());
        for(int i=kernel_fold.length()-1; i>=0; i=i-2)
        {
            char cur_folding = kernel_fold[i];
            fold.push_back(atoi(&cur_folding));
        }
    }

    return fold;
}

//ys_LC_jil_plain_folding1_8_1_
std::string getOptimisationFromKernel(char* kernel_name)
{
    std::string::size_type pos = std::string(kernel_name).find(std::string("spatial"));
    if(pos == std::string::npos)
    {
        return std::string("plain");
    }
    else
    {
        return std::string("spatial");
    }
}

yaskSite* createStencil(const char *kernel_name, MPI_Manager* mpiMan, std::vector<int> size, int ncores, std::vector<int> fold, char* opt, int radius)
{
    //std::vector<int> fold = getFoldFromKernel(kernel_name);
    //std::string opt = getOptimisationFromKernel(kernel_name);

    int dim = (int)size.size();
    int dt = 1;

    char* non_const_kernel_name;
    STRINGIFY(non_const_kernel_name, "%s", kernel_name);
    printf("stencil = %s\n", kernel_name);
    yaskSite* stencil = new yaskSite(mpiMan, non_const_kernel_name, dim, radius, fold[0], fold[1], fold[2], true, false);
    if(dim==3)
    {
        stencil->setDim(size[0], size[1], size[2], dt);
    }
    else if(dim == 2)
    {
        stencil->setDim(size[0], size[1], 1, dt);
    }
    else if(dim==1)
    {
        stencil->setDim(size[0], 1, 1, dt);
    }
    else
    {
        printf("Error : Size string is wrong\n");
    }

    stencil->setThread(ncores, 1);

    if(strcmp(opt, "spatial")==0)
    {
        stencil->spatialTuner("L3", "L2", 0.5, 0.5);
    }

    stencil->init();

    return stencil;
}
