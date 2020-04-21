#include <yaskSite.h>
#include "offsite_run_parse.h"
#include "offsite_helper.h"
#include <string.h>
#include "timing.h"

#define IMPL_RUN \
{\
    ys_RHS_LC_predictor->run();\
    for(int i=0; i<corrector_steps-1; ++i)\
    {\
        ys_RHS_LC->run();\
    }\
    ys_RHS->run();\
    ys_Approx->run();\
    ys_Update->run();\
}\

void main(int argc, char** argv)
{

    os_parser optParse;
    if(argc < 2)
    {
        printf("Usage : %s [opts]\n", argv[0]);
        optParse.help();
        exit(-1);
    }

    optParse.parse_arg(argc, argv);

    printf("initnig");
    //mc_file optional is passed to initialize with machine configuration
    MPI_Manager mpiMan(&argc, &argv, optParse.machine);
    printf("finished inting");

    int corrector_steps = optParse.corrector_steps;
    int stages = optParse.stages;

    std::vector<int> size = getRange(optParse.size);
    int dim = (int)size.size();
    int ncores = optParse.cores;


    std::vector<int> fold;
    if(strcmp(optParse.fold, "auto")==0)
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
        fold = getRange(optParse.fold);
    }

    char* opt = optParse.opt;

    std::string str_RHS_LC_predictor("ys_RHS_LC_predictor_jil_plain");
    std::string str_RHS_LC("ys_RHS_LC_jil_plain");
    std::string str_RHS("ys_RHS_jl_plain");
    std::string str_Approx("ys_Approx_ji_plain");
    std::string str_Update("ys_Update_j_plain");


    char* kernel = optParse.kernel;
    char *rem_str;
    STRINGIFY(rem_str, "_folding%d_%d_%d_yasksite_%s", fold[0], fold[1], fold[2], kernel);

    str_RHS_LC_predictor += std::string(rem_str);
    str_RHS_LC += std::string(rem_str);
    str_RHS += std::string(rem_str);
    str_Approx += std::string(rem_str);
    str_Update += std::string(rem_str);

    int radius = optParse.radius;

    yaskSite* ys_RHS_LC_predictor = createStencil(str_RHS_LC_predictor.c_str() , &mpiMan, size, ncores, fold, opt, radius);
    yaskSite* ys_RHS_LC = createStencil(str_RHS_LC.c_str(), &mpiMan, size, ncores, fold, opt, radius);
    yaskSite* ys_RHS = createStencil(str_RHS.c_str(), &mpiMan, size, ncores, fold, opt, radius);
    yaskSite* ys_Approx = createStencil(str_Approx.c_str(), &mpiMan, size, ncores, fold, opt, radius);
    yaskSite* ys_Update = createStencil(str_Update.c_str(), &mpiMan, size, ncores, fold, opt, radius);

    //couple the stencils (kernels)
    for(int i=0; i<stages; ++i)
    {
        char *grid_ys_RHS, *grid_ys_LC_in, *grid_ys_LC_out, *grid_ys_Approx_in;
        STRINGIFY(grid_ys_RHS, "__Y___%d",i);
        STRINGIFY(grid_ys_LC_in, "F_%d",i);
        STRINGIFY(grid_ys_LC_out, "Y_%d",i);
        STRINGIFY(grid_ys_Approx_in, "F_%d",i);

        //RHS should always be the bigger grid (remember halo if stencil is there)
        (*ys_RHS_LC_predictor)[grid_ys_RHS][0] << (*ys_RHS_LC)[grid_ys_RHS][0]; //TODO separate in and out grids in RHS
        (*ys_RHS)[grid_ys_RHS][0] << (*ys_RHS_LC)[grid_ys_RHS][0]; //TODO separate in and out grids in RHS
        ys_RHS->totalTime = corrector_steps-1; //offset time to have offset b/w RHS_LC and RHS
        (*ys_Approx)[grid_ys_Approx_in] << (*ys_RHS)[grid_ys_RHS][1];

        free(grid_ys_RHS);
        free(grid_ys_LC_in);
        free(grid_ys_LC_out);
        free(grid_ys_Approx_in);
    }
    (*ys_Update)["dy"][0] << (*ys_Approx)["data"][0];
    (*ys_Update)["data"][0] << (*ys_RHS_LC_predictor)["data"][0];

    IMPL_RUN; //warm-up, bring all data from memory
    double ctr=0;
    double time=0;
    INIT_TIME(impl);
    START_TIME(impl);
    while(time < 1)
    {
        IMPL_RUN;
        ++ctr;
        STOP_TIME(impl);
        time = GET_TIME(impl);
    }

    printf("Total time per iter = %f sec\n", time/ctr);

    double mlups = ctr*1e-6;
    for(int i=0; i<(int)size.size(); ++i)
    {
        mlups = mlups*(double)size[i];
    }
    printf("Total performance = %f MLUPS\n", mlups/time);

    ys_RHS_LC->calcECM(false);
    ys_RHS_LC->printECM();

    double RHS_LC_ECM_mlups = ys_RHS_LC->getPerfECM();
    double freq = ys_RHS_LC->getCpuFreq();

    double RHS_LC_cy_lup = freq*1e9/(RHS_LC_ECM_mlups*1e6);

    std::vector<double> RHS_LC_ECM = ys_RHS_LC->getECM();

    double RHS_LC_sat = ys_RHS_LC->getSaturation()[0];

    double sat = (RHS_LC_cy_lup*RHS_LC_sat)/(RHS_LC_ECM.back());
    printf("saturating at = %f\n", sat);

    printf("block_x = %d\n", ys_RHS_LC->bx);
    printf("block_y = %d\n", ys_RHS_LC->by);
    printf("block_z = %d\n", ys_RHS_LC->bz);

 
    //Report performance
    delete ys_RHS_LC_predictor;
    delete ys_RHS_LC;
    delete ys_RHS;
    delete ys_Approx;
    delete ys_Update;
}
