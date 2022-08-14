# Digital Signal Processing University labs

The lab includes low pass filter and digital filter FIR and IIR. A simple DFT
and FFT (recursive and iterative) implementation.

## Labs

### Lab01 - Low-pass filter (Passive)

Low-pass filter with passive components. The ESP32 on sample and output the sampled signal.

### Lab02 - High-pass filter (Passive)

The ESP32 isn't doing anything here, it is only sampling a signal from a signalgenerator and output it on a DAC enabled pin.

### Lab03 - FIR Filter

This is the first implementation of the digital filter.

### Lab04 - IIR Filter

The IIR filter coefficients is generated using matlab, note that the value matlab generates is for their Second Order Filter with Sections. Every output from the section has a scaling factor attached to it. In general in this form the filter is more stable than the one we're implementing.

### Lab05 - DFT Implementation

Implemenation of DFT algorithm in C++ and Python. There's a matlab script that plots a signal with `f = 800 Hz`. The matlab FFT function is later applied to it and the frequency graph is later plotted.

**NOTE**: We see that it only takes in one value in the `fft` function. This function returns a complex vector which we later divide it by the sample count `N` and take the absolute value of it to get vector of magnitude, `N` is the normalisation factor.

Video on [Discrete Fourier Transform - Simple Step by Step](https://youtu.be/mkGsMWi_j4Q) which go through how the DFT algorithm works. This knowledge is later used to implements the FFT algorithm.

### Lab06 - FFT Implementations

There are **FFT** recursive and iterative implementation in Python and C++. We later compare the speed of the **DFT**, **FFT recursive** and **FFT iterative**. **DFT** will be the slowest of them all because the algorithm speed is `O(N^2)` and both **recursive** and **iterative** **FFT** are `O(Nlog(N))` which try to approximate the `O(N)`.

The FFT algorithm is an algorithm that splits the DFT into even and odd parts recursively. This makes that the multiplication and summation of the problem is done once for each frequency bucket.

Video on [The FFT Algorithm - Simple Step by Step](https://youtu.be/htCj9exbGo0) by Simon Xu. The video explains how the FFT works and there's a C++ recursive implementation.

## Project - Pulse sensor Heart rate monitor

This project calculate the BPM using a pulse sensor that is light based. The BPM value and the signal over time is later displayed on an OLED screen.

Because the signal can be noisy it is first pass through a low- and high-pass IIR filter.