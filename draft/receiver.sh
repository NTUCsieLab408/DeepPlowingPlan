#!/bin/bash

cd $(dirname $0)

data_file=receiver.txt
log_file=log_r.txt

./receiver 4 > $data_file 2> $log_file

