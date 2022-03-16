clc
clear
close all

f = 1
fs = 8
Ts = 1 / fs;

N = 8
n = 0:N - 1;
samples = sin(2 * pi * f * Ts * n)

fft_it(samples)

function y = fft_it(x)
    n = length(x);
    x = x(bitrevorder(1:n));
    q = round(log(n) / log(2));
    for j = 1 : q
        m = 2^(j - 1);
        d = exp(-pi * i / m) .^ (0 : m - 1);
        for k = 1 : 2^(q - j);
            s = (k - 1) * 2 * m + 1;
            e = k * 2 * m;
            r = s + (e - s + 1) / 2
            even = x(s:(r - 1));
            odd  = x(r:e)
            z = d .* odd;
            y = [even + z, even - z];
            x(s:e) = y;
        end
    end
end
