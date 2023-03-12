#!/bin/sh

go run main.go &
python SEND_LOG_onPI.py &
wait
