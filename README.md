# Fast Non-local Stereo Matching based on Hierarchical Disparity Prediction

## USAGE

We implement 3 tree-based algorithms MST, ST and RT and their DP versions in
`main/*.cpp`


To use them, first make the them by

`make all`.

You can use `make main` if you just want to make *\*.cpp* in `main` folder.
Then the executable would be made in `bin` with suffix *.out*.


To run them, 

`./*.out LeftInput.ppm RightInput.ppm MaxDisparity [Scale] [LeftOutput.pgm]
[Dataset] [UseLab]`

- `Dataset` is the index for the dataset and it is used to switch which GMM
model to use. Here halfsize is 0 and fullsize is 1. 
- `UseLab` is 1 if want to use LAB color space in defining the matching cost and
edge weights, and is 0 otherwise
`Dataset` and `UseLab` would be neglected if you run `mst.out` or `st.out` or
`rt.out`.
Default values for the optional arguments:
  - `Scale`: 256/MaxDisparity
  - `LeftOutput.pgm`: default\_left.pgm
  - `Dataset`: 1 <=> fullsize
  - `UseLab`: 0
