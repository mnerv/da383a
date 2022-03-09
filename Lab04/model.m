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
20
40
100
200
290
600
800
1000
2000
3000
];

v = [
40.61
40.52
39.56
33.04
24.48
18.41
9.03
7.39
5.44
1.31
1.19
] ./ 46.15;

hold on
semilogx(f, v)
title('1.2 - IIR measurements')
grid on
xlabel('frequency (log_{10} Hz)')
ylabel('normalised voltage')
legend('computed', 'measurement')

data = [
];
f = [
    100
    200
    300
    400
    500
    600
    700
    750
    800
    850
    900
    950
    1000
    1050
    1100
    1150
    1200
    1250
    1300
    1350
];

v = [
    37.29
    36.44
    36.25
    38.19
    39.19
    38.95
    36.42
    34.31
    30.38
    26.46
    21.73
    17.06
    12.17
    8.27
    6.79
    2.93
    2.33
    1.17
    1.18
    2.19
];

figure
plot(f, v)
%semilogx(f, v)
grid on
%xlabel('frekvenser (log_{10} Hz)')
xlabel('frekvenser (Hz)')
ylabel('mV')
title('3.1')

function gain = Hsum(w, Hz, b, k)
    for s = 1 : length(w)
        w(s) = sum(Hz(b, w(s), k));
    end
    gain = w;
end

