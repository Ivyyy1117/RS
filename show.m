
% 1. 加载数据
load('Ninapro_dataset\s1\S1_A1_E1.mat')

% 2. 采样率
fs = 100;

% 3. 时间轴
t = (0:size(emg,1)-1) / fs;

% 4. 选择抓握动作（编号 2）
idx = restimulus == 8;
%{
% 5. 画图
figure
plot(t(idx), emg(idx,10))
xlabel('Time (s)')
ylabel('Amplitude')
title('Raw EMG during Grip')
grid on
%}
figure

for ch = 1:10
    subplot(10,1,ch)
    plot(t(idx), emg(idx,ch))
    ylabel(['Ch ' num2str(ch)])
    grid on
    
    if ch == 1
        title('Raw EMG (Grip)')
    end
    
    if ch < 10
        set(gca, 'XTickLabel', [])
    end
end

xlabel('Time (s)')
