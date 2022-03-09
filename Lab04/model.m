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

f = [
50
100
150
200
250
300
350
400
450
500
550
600
650
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
];

v = [
35.98
36.54
35.73
36.64
37.30
37.68
37.11
38.06
34.88
33.48
32.70
32.66
34.60
36.49
31.21
19.18
9.91
5.36
2.75
1.50
1.47
0.26
0.099
0.044
0.15
0.20
];

figure
plot(f, v)
%semilogx(f, v)
grid on
%xlabel('frekvenser (log_{10} Hz)')
xlabel('frekvenser (Hz)')
ylabel('mV')
title('3.4')

%% 4.1
f = 250;
T = 1 / f

s = @(t) 1 * (cos(2 * pi * f * t) - 1 / 3 * cos(2 * pi * 3 * f * t));

t = 0 : T / 1000 : T * 3;

figure
plot(t, s(t))

function gain = Hsum(w, Hz, b, k)
    for s = 1 : length(w)
        w(s) = sum(Hz(b, w(s), k));
    end
    gain = w;
end

