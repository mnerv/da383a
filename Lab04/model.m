clc
clear
close all
format short

%% Task 1.1
fs = 10000;
f  = 0 : 5000;
w  = 2 * pi * f / fs;

Hz = @(b, w, k) b .* exp(-i * w * k);

b = [0.1];
a = [0.9];
k = 0 : length(b);
l = 1 : length(a);

part_a = Hsum(w, Hz, b, k);
part_b = 1 - Hsum(w, Hz, a, l);

HH = part_a ./ part_b;

figure
semilogx(f, abs(HH))
grid on
xlabel('frequency (log_{10} Hz)')
ylabel('| H(z) |')
title('1.1 - IIR frequency response')

%% Task 1.2
figure
semilogx(f, abs(HH) ./ 2)

f = [
    10
    40
    100
    200
    290
    600
    1000
    3000
];

v = [
    46.25
    41.53
    28.33
    16.21
    12.38
    7.52
    3.05
    2.62
] ./ 54.00;

hold on
semilogx(f, v)
title('1.2 - IIR measurements')
grid on
xlabel('frequency (log_{10} Hz)')
ylabel('normalised voltage')
legend('computed', 'measurement')

function gain = Hsum(w, Hz, b, k)
    for s = 1 : length(w)
        w(s) = sum(Hz(b, w(s), k));
    end
    gain = w;
end

