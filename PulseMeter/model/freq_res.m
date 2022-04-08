clc
clear
close all
format short

% Exercise 10.5 in the book
frequencies = 0 : 8000;
fs = 1000;
omega = 2 * pi * frequencies / fs;  % convert to angular frequency

z_transform = @(omega) exp(i .* omega);
H_z = @(z) (14 - 28 * z.^(-1)) ./ (1 + 0.5 * z.^(-1));

figure
semilogx(frequencies, abs(H_z(z_transform(omega))))
grid on
xlabel('frequency [log_{10} Hz]')
ylabel('gain')
title('frequency response exercise 10.5')

