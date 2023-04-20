#!/bin/bash

SESSION="session"
PORT=1230

make clean
make


tmux new-session -d -s $SESSION

tmux split-window -v -p 75
tmux split-window -v -p 66
tmux split-window -v -p 50

tmux send-keys -t 0 "./lab5 $PORT" C-m
sleep 3

for i in 1 2 3
do
	tmux send-keys -t $i "nc localhost $PORT" C-m
done


tmux -2 attach-session -t $SESSION