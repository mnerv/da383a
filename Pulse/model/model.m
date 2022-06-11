% Examples of IIR Filter Design
% https://youtu.be/mSM1fCMZrZ8

%passband  = [(0.4 * pi) (0.6 * pi)] / pi;
%stopband  = [(0.2 * pi) (0.8 * pi)] / pi;
%passrip   = -20 * log10(0.95);
%stopatten = -20 * log10(0.01);
%
%[Ne, Wne] = ellipord(passband, stopband, passrip, stopatten);  % filter order
%[Be, Ae]  = ellip(Ne, passrip, stopatten, Wne)                 % filter coefficients
%
%[Ne, Wne] = buttord(passband, stopband, passrip, stopatten);  % filter order
%[Be, Ae]  = butter(Ne, Wne)               % filter coefficients
%% ECG

% Frequency in Hz
F_s = 1000;
F_stop1 = 0.2;

F_pass1 = 0.8;
F_pass2 = 5.0;

F_stop2 = 6.0;

% Attenuation in dB
A_pass  =  1;
A_stop1 = 40;
A_stop2 = 80;

% Normalise frequency to the half of F_s
passband = [F_pass1 F_pass2] * 2 * pi / (2 * pi * F_s / 2);
stopband = [F_stop1 F_stop2] * 2 * pi / (2 * pi * F_s / 2);

passripple = -20 * log10(0.95);
stopatten  = -20 * log10(0.001);

[Ne, Wne] = buttord(passband, stopband, passripple, stopatten);
[z, p, k]  = butter(Ne, Wne);
[B, A] = butter(Ne, Wne)
%sos = zp2sos(z, p, k);
%freqz(sos, 512, F_s)

%fvtool(B, A)

writematrix([B ; A], 'ecg_filter.csv')

%% Test

% Frequency in Hz
%F_s = 10000;
%F_stop1 = 60;
%
%F_pass1 = 100;
%F_pass2 = 150;
%
%F_stop2 = 180;
%
%% Attenuation in dB
%A_pass  =  1;
%A_stop1 = 60;
%A_stop2 = 80;
%
%% Normalise frequency to the half of F_s
%passband = [F_pass1 F_pass2] * 2 * pi / (2 * pi * F_s / 2);
%stopband = [F_stop1 F_stop2] * 2 * pi / (2 * pi * F_s / 2);
%
%passripple = -20 * log10(0.95);
%stopatten  = -80;
%
%[Ne, Wne] = buttord(passband, stopband, passripple, stopatten);
%[z, p, k] = butter(Ne, Wne);
%[Be, Ae] = butter(Ne, Wne);
%
%sos = zp2sos(z, p, k);
%freqz(sos,512,F_s)
%
%format long
%fprintf("length: %d", length(Be))
%Be
%fprintf("length: %d", length(Ae))
%Ae
%
%writematrix(transpose(Be), 'b.csv')
%writematrix(transpose(Ae), 'a.csv')

%csvdata = transpose([Be ; Ae])
%writematrix(csvdata, 'coefficients.csv')

% Low-pass filter FIR 'equiripple'

%F_s = 3000;
%F_pass = 250;
%F_stop = 500;
%
%F_max  = 2 * pi * F_s / 2;
%F_pass = (2 * pi * F_pass) / F_max;
%F_stop = (2 * pi * F_stop) / F_max;
%
%pass_ripple = -20 * log10(0.95);
%stop_atten  = -80;
%
%b = firceqrip('minorder',[F_pass F_stop],[0.02 1e-4]);
%fvtool(b)


% IIR Low-pass Chebyshev
F_s    = 1000;
F_pass = 5;
F_stop = 10;

pass_rippl = -20 * log10(0.9);
stop_atten = -20 * log10(0.001);

% Normalise frequency
F_max  = 2 * pi * F_s / 2;
F_pass = (2 * pi * F_pass) / F_max;
F_stop = (2 * pi * F_stop) / F_max;

% Chebyshev 1
[Nc1, Wnc1] = cheb1ord(F_pass, F_stop, pass_rippl, stop_atten);
[Bc1, Ac1]  = cheby1(Nc1, pass_rippl, Wnc1)

writematrix([Bc1 ; Ac1], 'Low-pass-Chebyshev.csv')

% IIR High-pass Chebyshev
F_s    = 1000;
F_pass = 0.8;
F_stop = 0.1;

pass_rippl = -20 * log10(0.9);
stop_atten = -20 * log10(0.001);

% Normalise frequency
F_max  = 2 * pi * F_s / 2;
F_pass = (2 * pi * F_pass) / F_max;
F_stop = (2 * pi * F_stop) / F_max;

% Chebyshev 1
[Nc1, Wnc1] = cheb1ord(F_pass, F_stop, pass_rippl, stop_atten);
[Bc1, Ac1]  = cheby1(Nc1, pass_rippl, Wnc1)

writematrix([Bc1 ; Ac1], 'High-pass-Chebyshev.csv')

% IIR - Pass-Band Elliptic
F_s = 1000;

F_stop1 = 100;

F_pass1 = 125;
F_pass2 = 180;

F_stop2 = 200;

F_max = 2 * pi * F_s / 2;

passband = 2 * pi * [F_pass1 F_pass2] / F_max;
stopband = 2 * pi * [F_stop1 F_stop2] / F_max;

pass_rippl = -20 * log10(0.9);
stop_atten = -20 * log10(0.001);

[Ne, Wne] = ellipord(passband, stopband, pass_rippl, stop_atten);
[Be, Ae] = ellip(Ne, pass_rippl, stop_atten, Wne)

writematrix([Be ; Ae], 'passband-elliptic.csv')

% IIR - Pass-Band Butterworth
F_s = 1000;

F_stop1 = 0.1415;

F_pass1 = 0.8;
F_pass2 = 5;

F_stop2 = 29;

F_max = 2 * pi * F_s / 2;

passband = 2 * pi * [F_pass1 F_pass2] / F_max;
stopband = 2 * pi * [F_stop1 F_stop2] / F_max;

pass_rippl = -20 * log10(0.9);
stop_atten = 80;

[Ne, Wne] = buttord(passband, stopband, pass_rippl, stop_atten);
[Be, Ae]  = butter(Ne, Wne)

writematrix([Be ; Ae], 'passband-butterworth.csv')

