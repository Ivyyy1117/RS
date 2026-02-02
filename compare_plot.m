% Load processed data
load('E:\RS\emg_simulation\filtered_dataset\1-2.mat');

% Time vector
t = (0:length(emg_raw_segment)-1) / double(fs);

% Create figure
figure;

% Plot Raw
subplot(2,1,1);
plot(t, emg_raw_segment);
title('Raw EMG (Channel 1 )');
ylabel('Amplitude');
grid on;

% Plot Filtered
subplot(2,1,2);
plot(t, emg_filtered_segment);
title('Filtered EMG (CMSIS-DSP Biquad)');
ylabel('Amplitude');
xlabel('Time (s)');
grid on;

% Link axes for zooming
linkaxes(findall(gcf,'type','axes'),'x');
