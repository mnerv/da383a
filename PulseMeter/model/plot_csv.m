clc
clear
close all

filename = 'plot_data.csv';
f_id   = fopen(filename, 'r');
header = strsplit(fgetl(f_id), ',');
header(1) = [];
fclose(f_id);

data = csvread(filename, 1);
n = data(:,1);

figure
plot(n, data(:,2))
hold on
plot(n, data(:,3), '.:','MarkerSize', 5)
plot(n, data(:,4), '.-', 'MarkerSize', 6)

grid on
title('sample data')
xlabel('sample [n]')
ylabel('amplitude')
legend(header)

