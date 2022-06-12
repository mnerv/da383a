clc
clear
close all

% Prepare header name for legend texts
filename = 'plot_data.csv';
f_id   = fopen(filename, 'r');
header = strsplit(fgetl(f_id), ',');
header(1) = [];
fclose(f_id);

% Read data columns skip the first row
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

%saveas(gcf, './simulate.eps', 'epsc')
%saveas(gcf, './simulate.png', 'png')
%saveas(gcf, './simulated.svg', 'svg')

