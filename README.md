# SC20-YASKSITE-AD

This is the Artifact Description repository for the YaskSite paper.

### How to run YaskSite stand-alone ###
* Install YaskSite as described in the [repository](https://github.com/seasite-project/YaskSite).
* Go to its [example folder](https://github.com/seasite-project/YaskSite/tree/master/example), build and run the `perf` example. In case no LIKWID is installed use `perf_wo_likwid`.
* The base stencils like Wave3D, Heat3D are available by default in YaskSite.
* The machine files used for the paper is available [here](https://github.com/seasite-project/SC20-YASKSITE-AD/mc_files)

### How to use YaskSite as the backend for Offsite ###
* Install Offsite by following the instructions in its [repository](https://github.com/seasite-project/Offsite).
* Install YaskSite by following the instructions in its [repository](https://github.com/seasite-project/YaskSite).
* Go to [example folder](https://github.com/seasite-project/YaskSite/tree/master/example) of YaskSite and build it. Specifically we need the `offsite_adapter` executable.
* Set the `PATH` to include this example folder. That is `PATH=$PATH:[path/to/example/folder]`
* Now YaskSite can be used as backend to Offsite, just specify `--tool yasksite` as option to `offsite_tune`. 
For example for auto-tuning with [CLX](https://github.com/seasite-project/SC20-YASKSITE-AD/mc_file/CascadelakeSP_Gold-6248.yml) and Heat3D stencil go to Offsite folder and use the following:
```
offsite_tune --tool yasksite --machine examples/machines/CascadelakeSP_Gold-6248.yml --compiler icc --impl examples/impls/pirk/ --kernel examples/kernels/pirk/ --method examples/methods/implicit/radauIIA7.ode --ivp examples/ivps/${ivp}.  ivp --mode MODEL --bench examples/bench/OMP_BARRIER_CascadelakeSP_Gold-6248_icc19.0.2.187.bench --config config_clx.tune --verbose --filter-yasksite-opt
```

