clc
clear
close all

d = csvread('sample_data.csv');
x = d(:,1)
y = d(:,2)

figure
stem(x, y)
grid on
title('Sample data')
xlabel('sample [n]')
ylabel('amplitude')

d = csvread('fft_data.csv');
x = d(:,1)
y = d(:,2)

figure
stem(x, y)
grid on
title('FFT')
xlabel('frequency [Hz]')
ylabel('amplitude')

