

global A W C Q Xt_ Sigma_t_; 
global Fs Cn Max_v;
global u fSave fRaw fRaw_string feat_string Decode_Save;


%Number of Channels
Cn = 64;
%Frequency
Fs = 1000;
%Maximum Velocity
Max_v = 5;

%Kalman Filter Parameters Initialization
A = rand(5,5);
W = eye(5);
C = rand(Cn * 2,5);
Q = eye(Cn * 2);
Xt_ = rand(5,1);
Sigma_t_ = rand(5,5);






%Configure the recording of features and signals
fSave = fopen('D:\Data\features.txt','w');
%fRaw = fopen('D:\Data\raw_signals.txt','w');  record raw data
Decode_Save = fopen('D:\Data\Decode_Save.txt','w');

%Generate the fprintf string for the given number of channels
fRaw_string = '';
for i = 1:Cn
    fRaw_string = strcat(fRaw_string, ' %.2f');
end
fRaw_string = strcat(fRaw_string,'\r\n');


%Generate the fprintf string for the given number of channels
feat_string = '';
for i = 1:Cn * 2
    feat_string = strcat(feat_string,' %.2f');
end
feat_string = strcat(feat_string,'\r\n');





%Udp port setup
port=8051;
host='127.0.0.1';
u = udp(host,port, 'LocalPort',8052);
fopen(u);
