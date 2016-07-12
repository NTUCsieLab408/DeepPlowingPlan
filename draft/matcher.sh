#!/bin/bash

which=$1
compt=$2

cd $(dirname $0)

log_which=$1-3
let "log_which++"
data_file=matcher${log_which}.txt
log_file=log_m${log_which}.txt

./matcher $which $is $compt > $data_file 2> $log_file

