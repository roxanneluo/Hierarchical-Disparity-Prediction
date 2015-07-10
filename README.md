# Fast Non-local Stereo Matching based on Hierarchical Disparity Prediction
##INSTALL DEPENDENCIES
1. libpng++
2. python
3. g++ (which supports c++0x or c++11. g++-4.8 and above versions will work.)

### Install for Ubuntu
#### Install g++
First, install g++-4.8, for example, by 
```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc-4.8
sudo apt-get install g++-4.8
```
Then change `CXX` in `makefile` to use a suitable g++ version.
#### Install libpng++
```
sudo apt-get install libpng++-dev
```

### Install for Mac
`brew install libpng`

##USAGE

Let `$ROOT_DIR` be the root directory of this project.
### Obtain testdata
```
cd $ROOT_DIR
wget http://bcmi.sjtu.edu.cn/~luoxuan/iccv2015/testdata_iccv2015.zip
unzip testdata_iccv2015.zip
rm testdata_iccv2015.zip
```
then the datasets would be saved in folder `testdata`.

### The Simplest Way
First run `make all`.

1. For Middlebury datasets like halfsize, fullsize, etc.
  1. To run on a single Testcase,

      `python run_single.py Algo Dataset Testcase Tolerance1 [Tolerance2] [Tolerance3]...`

      e.g., `python run_single.py Algo halfsize Aloe 1 2`.

      Arguments:
      - `Algo.bin`: the name of the algorithm. Replace *Algo* with mst, st, rt or
  dp\_mst, dp\_st, dp\_rt
      - `Dataset`: the index for the dataset and it is used to switch which GMM
model to use. Here halfsize is 0 and fullsize is 1. 
      - `Testcase`: eg., Aloe, Baby1, ...
To run on a single testcase,
      - `Tolerance*`[int]: Note that the OutputDisparity = ComputedDisparity \*
        Scale. |OutputDisparity - GNDDisparity| >= Scale \* Tolence is regarded erroneous.

      Outputs:
      - `results/Dataset/Testcase_left_Algo.pgm`: Left output disparity map.
      - `results/Dataset/err_Tolerance*_Testcase_left_Algo.ppm`: Error Map for
        each Tolerance. Erroneous pixels are shown in red if they are closer 
        than gnd and in green if they are farther than gnd.

      - in `stdout`: error >= Tolerance*: CorrectNumber TotalNumberOfPixels \n
        error rate: ErrorRate 
  2. To run on one or more  whole datasets and generate an HTML of all error rates, 
    open `super_run_nonocc.py`, specify the algorithms you want to compare 
    in `ALGORITHMS`. You may change the checker and whether to use LAB color
    space in `CHECKER` and `use_lab`. Run

      `python -u super_run_nonocc.py Tolerance HTMLPre Dataset1 [Dataset2] ... |tee results/LogFileName`,

      e.g., `python -u super_run_nonocc.py 1 test halfsize |tee results/Log_test_err_ge_1_halfsize.txt`,
  
      then the HTML file would be saved at
        `results/SuperReport_HTMLPre_err_ge_Tolerance_Dataset1[_Dataset2_...].html` and the stdout would be saved at
        `results/LogFileName`.
2. For KITTI, run

  `python run_all_kitti.py Algo MaxDisparity Scale`

  e.g., `python run_all_kitti.py dp_mst 160 1`

3. For waseda (Low-quality dataset), run

  `python run_all_waseda.py Algo MaxDisparity Scale`

  e.g., `python run_all_waseda.py Algo 31 8`

In the sequel, we give more detailed description of the codes.

### Main Tree-based Algorithms and Their DP Versions 

We implement 3 tree-based algorithms MST, ST and RT and their DP versions in
`main/*.cpp`


To use them, first make the them by

`make all`.

You can use `make main` if you just want to make *\*.cpp* in `main` folder.
Then the executable would be made in folder `bin` with suffix *.bin*.


To run them, 

`./Algo.bin LeftInput.ppm RightInput.ppm MaxDisparity [Scale] [LeftOutput.pgm]
[Dataset] [UseLab]`

e.g., `./dp_mst.bin testdata/halfsize/Aloe/left.ppm testdata/halfsize/Aloe/right.ppm 135 2 results/halfsize/Aloe_left_dp_mst.pgm 0 0`.

- `UseLab` is 1 if you want to use LAB color space in defining the matching cost and
edge weights, and is 0 otherwise

`Dataset` would be neglected if you run `mst.out` or `st.out` or
`rt.out`, but it has to be provided if you want to specify `use_lab`.

Default values for the optional arguments:

- `Scale`: 256/MaxDisparity
- `LeftOutput.pgm`: default\_left.pgm
- `Dataset`: 1 (<=> fullsize)
- `UseLab`: 0

Note that the *OutputDisparity = ComputedDisparity \* Scale*.


### Checker
To obtain the error rate in the nonoccluded regions, make and run it:

```
make checker
./bin/checker/checker_nonocc.bin LeftOutput.pgm GNDLeftDisp.pgm GNDRightDisp.pgm Tolerance Scale [ErrMap] [ErrAllRed].
```
e.g., `./bin/checker/checker_nonocc.bin results/halfsize/Aloe_left_dp_mst.pgm testdata/halfsize/Aloe/displeft.pgm testdata/halfsize/Aloe/dispright.pgm 1 2 results/halfsize/err_1_Aloe_left_dp_mst.ppm`.

- `GNDLeftDisp.pgm`, `GNDRightDisp.pgm`: the ground truth disparity map for the left and right images.
- `Tolerance`[int]: |OutputDisparity - GNDDisparity| >= Scale \* Tolence is regarded erroneous.
- `ErrMap`: the filename for the error map. If not specified, the error map is
  not generated.
- `ErrAllRed`[int]: By default, if `ErrMap` is specified, erroneous pixels are shown
  in red if they are closer than gnd and in green if they are farther than gnd.
  Erroneous pixels are all shown in red if *ErrAllRed* != 0.

Another checker is `checker/checker_left_nonocc.cpp`, which compute the number
of correct pixels and total number of pixels in non-occluded regions. Pixels
with disparity > 0 are regarded non-occluded.

### Util
Some codes that might be useful are in folder `util`. Fo example, all GND disparity maps
are in form *.ppm* for *fullsize* dataset. You can convert them to pgm by
using `rgb2gray.py`. Make codes in `util` by `make util`. And check each *.py* for
their specific usage.


### Generate Statistical Data for GMM Generation and Other Matlab-related Tests
These codes are in `gen_data`. Make and run them: 

```
make gen_data
./bin/gen_data/gen_concur_dp_Algo.bin LeftInput.ppm RightInput.ppm MaxDisparity Scale GNDLeftDisparityMap Dataset Algo ConcurFolder SupportFolder
```

e.g.,
```
make gen_data
mkdir -p results/stat/ && mkdir -p results/stat/concur_test && mkdir -p results/stat/support_test
./bin/gen_data/gen_concur_dp_mst.bin testdata/halfsize/Aloe/left.ppm testdata/halfsize/Aloe/right.ppm 135 2 testdata/halfsize/Aloe/displeft.pgm halfsize mst results/stat/halfsize/concur_test resutls/stat/halfsize/support_test
```

Outputs:

- `ConcurFolder/concur_Dataset_Algo_l,l-1.txt`: the concurrence matrix, i.e., # of
concurrence for *D_l* in smaller layer *l* and *D_{l-1}* in larger layer *l-1*,
- `SupportFolder/support_Dataset_Algo_l.txt`, the support vector, i.e., # of stable pixels for each disparity candidate in layer *l*. It is the unnormalized Pr(D\_l).
 
To run them on a whole dataset, run

`python gen_concur.py Algo DatasetFolder ResultFolder Dataset [UseLab]`

e.g., 

```
mkdir -p results/stat
python gen_concur.py dp_mst testdata/halfsize results/stat halfsize
```

Arguments:

- `DatasetFolder`: e.g., testdata/fullsize
- `ResultsFolder`: *ConcurFolder* would be `ResultFolder/concur_CurrentDateTime/`
  and *SupportFolder* = `ResultFolder/support_CurrentDateTime/`. 

### Compute GMM needed for DPM
1. Generate Concurrence Matrices as above.
2. Open `matlab/readConcur.m`. Change `filename_format` to use *ConcurFolder*
   and *Algo*.
3. Open Matlab, `cd matlab` and find the GMM

    `>> findGMMDrawPicForPaper(FullOrHalf, K, DatasetRatios, Times, Width);`
    e.g., `>> findGMMDrawPicForPaper('halfsize', 4, [1 0.5 0.3], 20, 0.1);`
  
    Arguments:

    - `FullOrHalf`: "fullsize" or "halfsize"
    - `K`: find a GMM using 1,2,...,K gaussians.
    - `DatasetRatios`[array]: for each *ratio* in array *DatasetRatios*, random
      sample *ratio\*(#of all testcases)* testcases from the dataset and
      generate GMM for each *ratio*.
    - `Times`: For each k in [1,K] and for each ratio in *DatasetRatios*, try *Times* to find the best GMM because ill-conditioned sigma sometimes makes it hard to find one GMM by a single trial. 

    Outputs:
    
    - `../results/pic/GMM/GMM_l_S.eps`, `../results/pic/GMM/GMM_l_S.fig`: *l* is layer index.
    -  `../results/GMM up to K Kernels for Times times gnd vs. Algo
       CurrentDateTime.html` 

### Simulate the DPM process and draw figures 
To draw `PixelIntv_l`, `concur_l,{l-1}`, simulated and statistical (true) `P(D_l|D_{l-1})` (sgl), `P(D_{l-1}|D_l)` (lgs), and to obtain HTMLs for the uncover ratio, interval ratio and error rate w.r.t. true `P(D_{l-1}|D_l)`, 
`cd matlab`, open Matlab and run 

`>> drawPicsForPaper(HTMLNameSpec, FullOrHalf, Test, Draw, Save,
Level, DatasetRatio)`

e.g., `>> drawPicsForPaper('test', 'halfsize', false, true, true,
3, 0.5)`

Arguments:

- `HTMLNameSpec`[str]: the HTMLs would be saved at `../results/Prefix Report HTMLNameSpec
  FullOrHalf Prec DatasetRatio.html`, where *Prefix* is "Uncover Ratio" or "E[err]" or "Interval".
- `FullOrHalf`[str]: "fullsize" or "halfsize".
- `Test`[bool]: true if just test a few testcases, see `getDataSet.m`. Otherwise, get
  results for the complete dataset.
- `Draw`[bool]: true if want to show the figures. Uncomment the figures you want
  to draw in function `drawAll`.
- `Save`[bool]: The figures would be saved in folders `sgl`, `lgs`, `PixelIntv` and `concur` under folder `../results/pic/FullOrHalf/`.
- `Level`[int]: number of layers.
- `DatasetRatio`[double]: used to choose corresponding GMM.

### Branches

- master: the up-to-date reliable core code
- real-reorganize: the up-to-date core code. possibly unreliable. If it is
reliable, it gets merged with master.
- add-kitti-tool: contain kitti tools in addition to the core code, master.
- other-algo: contain others' related algorithms such as libelas in addition to
add-kitti-tool


