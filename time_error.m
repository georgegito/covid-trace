clear;
close all;
format long;

%% find file size
fileID = fopen( 'out/bt_search_times.bin' );
fseek( fileID, 0, 'eof' );
filesize = ftell( fileID ) / 8;
fclose( fileID );

%% read file
fileID = fopen( 'out/bt_search_times.bin' );
bt_search_times = fread( fileID, filesize,'double' );
fclose( fileID );

%% iteration error
it_err = zeros( filesize - 1, 1 );
for i = 2: filesize
    it_err( i - 1 ) = abs( ( bt_search_times( i ) - bt_search_times( i - 1 ) ) - 10 );
end
figure( 1 );
plot( it_err, 'Linewidth', 0.5 );
title( 'Real-Time Error per Iteration', 'interpreter', 'latex', 'FontWeight', 'bold' );
ylabel( 'Real-Time Error (s)', 'interpreter', 'latex' );
xlabel( 'Runtime (s)', 'interpreter', 'latex' );
xlim([0 filesize]);

%% correct times
correct_times = [ 0: 10: ( filesize - 1 ) * 10 ]';

%% accumulated error
acc_err = abs( bt_search_times - correct_times );
figure( 2 );
plot( acc_err, 'Linewidth', 0.8 );
title( 'Accumulated Real-Time Error', 'interpreter', 'latex' );
ylabel( 'Real-Time Error (s)', 'interpreter', 'latex', 'FontWeight', 'bold' );
xlabel( 'Runtime (s)', 'interpreter', 'latex' );
xlim([0 filesize]);