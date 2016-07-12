#!/bin/bash

num=$1

cd $(dirname $0)

data_file=generator.txt
log_file=log_g.txt

./generator 0 $num 1 > $data_file 2> $log_file

