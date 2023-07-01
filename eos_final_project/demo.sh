#!/bin/bash

server_ip="192.168.0.206"
server_port=8888


numbers=($(shuf -i 1-12))

tmux new-session -d -s mysession

for i in {1..8}; do
    indices=($(shuf -i 0-11 -n 5))
    random_number1=${numbers[indices[0]]}
    random_number2=${numbers[indices[1]]}
    random_number3=${numbers[indices[2]]}
    #random_number4=${numbers[indices[3]]}
    #random_number5=${numbers[indices[4]]}
    
    random_number_x=$((RANDOM % 13 - 6))
    random_number_y=$((RANDOM % 13 - 6))
    
    tmux new-window -t mysession -n "Client $i" "nc $server_ip $server_port <<< \"$random_number1 $random_number2 $random_number3 ( $random_number_x $random_number_y )\""
    #tmux new-window -t mysession -n "Client $i" "nc $server_ip $server_port <<< \"$random_number1 $random_number2 $random_number3 $random_number4 $random_number5 ( $random_number_x $random_number_y )\""
done

tmux attach-session -t mysession
