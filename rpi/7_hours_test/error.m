clear;
format long;
%% find file size
fileID = fopen('bt_search_times.bin');
fseek(fileID, 0, 'eof');
filesize = ftell(fileID) / 8;
fclose(fileID);

%% read file
fileID = fopen('bt_search_times.bin');
bt_search_times = fread(fileID, filesize,'double');
fclose(fileID);

%% correct times
correct_times = [0: 10: (filesize - 1)*10]';

%% error
err = abs(bt_search_times - correct_times);
figure;
plot(err);
