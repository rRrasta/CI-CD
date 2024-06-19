#!/bin/bash

scp src/cat/s21_cat rrasta@192.168.56.102:~/
scp src/grep/s21_grep rrasta@192.168.56.102:~/
ssh rrasta@192.168.56.102 "echo "h080HM102" | sudo -S mv s21_cat s21_grep /usr/local/bin"
