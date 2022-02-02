data = textread('data.txt', '%u');
disp(min(data));
disp('\n');
disp(max(data));
disp('\n');
histogram(data); 