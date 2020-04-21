# SC20-YASKSITE-AD

This is the Artifact Description repository for the YaskSite paper.

### How to run YaskSite stand-alone ###
* Install YaskSite as described in the [repository](https://github.com/seasite-project/YaskSite).
* Go to its [example folder](https://github.com/seasite-project/YaskSite/tree/master/example), build and run the `perf` example. In case no LIKWID is installed use `perf_wo_likwid`.
* The base stencils like Wave3D, Heat3D are available by default in YaskSite.
* The machine files used for the paper is available [here](https://github.com/seasite-project/SC20-YASKSITE-AD/tree/master/machines)

### How to use YaskSite as the backend for Offsite ###
* Install Offsite by following the instructions in its [repository](https://github.com/seasite-project/Offsite).
* Install YaskSite by following the instructions in its [repository](https://github.com/seasite-project/YaskSite).
* Go to [example folder](https://github.com/seasite-project/YaskSite/tree/master/example) of YaskSite and build it. Specifically we need the `offsite_adapter` executable.
* Set the `PATH` to include this example folder. That is `PATH=$PATH:[path/to/example/folder]`
* Now YaskSite can be used as backend to Offsite, just specify `--tool yasksite` as option to `offsite_tune`. 
For example for auto-tuning with [CLX](https://github.com/seasite-project/SC20-YASKSITE-AD/blob/master/machines/CascadelakeSP_Gold-6248.yml) and Heat3D stencil come to this folder and use the following:
```
offsite_tune --tool yasksite --machine machines/CascadelakeSP_Gold-6248.yml --compiler icc --impl impls/pirk/ --kernel kernels/pirk/ --method methods/implicit/radauIIA7.ode --ivp ivps/Heat3D.  ivp --mode MODEL --bench bench/OMP_BARRIER_CascadelakeSP_Gold-6248_icc19.0.2.187.bench --config config_clx.tune --verbose --filter-yasksite-opt
```
* Use config_clx.tune to control the fold variants.

### How to validate the Offsite predictions using actual runs ###
* Go to [run_variants/YaskSite](), after Offsite run, i.e., `cd run_variants/YaskSite`.
* `mkdir build && cd build` - Create a build folder.
* `CC=icc CXX=icpc cmake .. -DyaskSite_DIR=/path/where/YaskSite/is/installed`.Note that the path is same as the one provided to `-DCMAKE_INSTALL_PREFIX` when installing YaskSite.
* `make`
* Now you can run the PIRK variants. For example variant A with 2 threads, Wave3D_radius2 ODE problem, RADAU II A7 method (s=4, m=6), fold 1:1:8, N=400, and spatial tuning on CascadeLake SP Gold-6248 (SNC off) can be run as:
```
threads=2
taskset -c 0-$((threads-1)) ./ys_A_il -c $threads -C 6 -S 4 -r 2 -k Wave3D_radius2 -m ../../../machines/CascadelakeSP_Gold-6248.yml -f 8:1:1 -o spatial -s 400:400:400
```
