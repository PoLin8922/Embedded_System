#!/bin/bash

SESSION="session"
PORT=4449

make clean
make


tmux new-session -d -s $SESSION

tmux split-window -v -p 75
tmux split-window -v -p 66
tmux split-window -v -p 50

tmux send-keys -t 0 "./lab5 $PORT" C-m
sleep 3

tmux send-keys -t 1 "nc localhost $PORT" C-m
tmux send-keys -t 2 "nc localhost $PORT" C-m
tmux send-keys -t 3 "nc localhost $PORT" C-m


tmux -2 attach-session -t $SESSION