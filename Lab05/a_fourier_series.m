clc
clear
close all

f = 800
N = 600
T = 1 / f;
L = N * T;  %Length 0f the Signal=L=N×T
Y = @(x) sin(50.0 * 2.0 * pi * x) + 0.5 * sin(80.0 * 2.0 * pi * x);

t = (0 : N - 1) .* T;
output = Y(t);

figure
plot(t, output)

fft_output = fft(output);

P2 = abs(fft_output/N);
P1 = P2(1:N/2+1);
P1(2:end-1) = 2 * P1(2:end-1);

f = f * (0 : (N / 2)) / N;
figure
plot(f,P1)
title('Single-Sided Amplitude Spectrum of X(t)')
xlabel('f (Hz)')
ylabel('|P1(f)|')

