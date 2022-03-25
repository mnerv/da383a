clc
clear
close all

d = csvread('data.csv');
d_n = csvread('data_noise.csv');
out = csvread('data_out.csv');

x = d(:,1);
y = d(:,2);

figure
%stem(x, y)
plot(x, y)
hold on

x = d_n(:,1);
y = d_n(:,2);
plot(x, y, '.','MarkerSize', 1)

x = out(:,1);
y = out(:,2);
plot(x, y, '-.', 'LineWidth', 2)


grid on
title('sample data')
xlabel('sample [n]')
ylabel('amplitude')

