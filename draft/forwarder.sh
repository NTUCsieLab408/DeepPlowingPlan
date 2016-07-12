#!/bin/bash

infly_high=$1

cd $(dirname $0)

data_file=forwarder.txt
log_file=log_f.txt

./forwarder 1 $infly_high > $data_file 2> $log_file

