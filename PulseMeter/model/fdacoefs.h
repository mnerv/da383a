/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 * Generated by MATLAB(R) 9.11 and Signal Processing Toolbox 8.7.
 * Generated on: 28-Mar-2022 07:54:29
 */

/*
 * Discrete-Time IIR Filter (real)
 * -------------------------------
 * Filter Structure    : Direct-Form II, Second-Order Sections
 * Number of Sections  : 16
 * Stable              : Yes
 * Linear Phase        : No
 */
#include <cstdint>
#include <cstddef>

/**
 * Fs: 5000
 * Fstop1: 60
 * Fpass1: 100
 * Fpass2: 500
 * Fstop2: 800
 */

/* 
 * Expected path to tmwtypes.h 
 * /Applications/MATLAB_R2021b.app/extern/include/tmwtypes.h 
 */
#define MWSPT_NSEC 33
std::size_t NL[MWSPT_NSEC] = { 1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1 };
double NUM[MWSPT_NSEC][3] = {
  {
     0.2612228859468,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2612228859468,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2496999544801,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2496999544801,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2401326001467,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2401326001467,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2324008725606,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2324008725606,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2263819075734,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2263819075734,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2219675282608,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2219675282608,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2190728369152,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2190728369152,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2176400572048,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
     0.2176400572048,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
                   1,                 0,                 0
  }
};
std::size_t DL[MWSPT_NSEC] = { 1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1 };
double DEN[MWSPT_NSEC][3] = {
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.515688483253,   0.9187031139296
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.969619747146,    0.983697929936
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.413100292464,   0.7772673683753
  },
  {
                   1,                 0,                 0
  },
  {
                   1,    -1.93714630064,   0.9513820788601
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.338962786437,   0.6626882219986
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.903772728473,   0.9185832512736
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.868241828393,   0.8841358524115
  },
  {
                   1,                 0,                 0
  },
  {
                   1,    -1.29067847694,   0.5732228155721
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.828804031077,   0.8464896914484
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.266766854818,   0.5077058615319
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.782550047593,    0.803164116002
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.267616224207,   0.4663447198804
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.723361487682,   0.7491184305497
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.297516473952,   0.4524436896494
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.630422101478,   0.6678340578627
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.376101324414,   0.4813731633634
  },
  {
                   1,                 0,                 0
  }
};