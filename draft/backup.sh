#!/bin/bash


cd $(dirname $0)

data_file=backup.txt
log_file=log_b.txt

./backup 5 > $data_file 2> $log_file

