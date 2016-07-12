#!/bin/bash

cd $(dirname $0)

G0=192.168.1.11
F0=192.168.1.21
M0=192.168.1.31
M1=192.168.1.41
R0=192.168.1.51
B0=192.168.1.61

make clean
if [ "$1" == 1 ]
then
  D_OPTIONS="-DDEVELOP" make
else
  make
fi

EG0=DPW_generator
EF0=DPW_forwarder
EM0=DPW_matcher
EM1=DPW_matcher
ER0=DPW_receiver
EB0=DPW_backup

scp -o ConnectTimeout=1 $EG0 ${EG0}.sh $G0:/tmp
scp -o ConnectTimeout=1 $EF0 ${EF0}.sh $F0:/tmp
scp -o ConnectTimeout=1 $EM0 ${EM0}.sh $M0:/tmp
scp -o ConnectTimeout=1 $EM1 ${EM1}.sh $M1:/tmp
scp -o ConnectTimeout=1 $ER0 ${ER0}.sh $R0:/tmp
scp -o ConnectTimeout=1 $EB0 ${EB0}.sh $EF0 ${EF0}.sh replace_forwarder.sh $ER0 ${ER0}.sh replace_receiver.sh $B0:/tmp

