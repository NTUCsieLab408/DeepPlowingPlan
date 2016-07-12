#!/bin/bash

cd $(dirname $0)

G0=192.168.0.10
F0=192.168.0.20
M0=192.168.0.30
M1=192.168.0.40
R0=192.168.0.50
B0=192.168.0.60

data_url=data/$1

mkdir -p $data_url

scp -o ConnectTimeout=1 $G0:/tmp/generator.txt $data_url
#scp -o ConnectTimeout=1 $G0:/tmp/log_g.txt $data_url

scp -o ConnectTimeout=1 $F0:/tmp/forwarder.txt $data_url
scp -o ConnectTimeout=1 $F0:/tmp/log_f.txt $data_url

scp -o ConnectTimeout=1 $M0:/tmp/matcher1.txt $data_url
scp -o ConnectTimeout=1 $M0:/tmp/log_m1.txt $data_url

scp -o ConnectTimeout=1 $M1:/tmp/matcher2.txt $data_url
scp -o ConnectTimeout=1 $M1:/tmp/log_m2.txt $data_url

scp -o ConnectTimeout=1 $R0:/tmp/receiver.txt $data_url
#scp -o ConnectTimeout=1 $R0:/tmp/log_r.txt $data_url

scp -o ConnectTimeout=1 $B0:/tmp/backup.txt $data_url
#scp -o ConnectTimeout=1 $B0:/tmp/log_b.txt $data_url
#scp -o ConnectTimeout=1 $B0:/tmp/forwarder.txt $data_url/forwarder_new.txt
#scp -o ConnectTimeout=1 $B0:/tmp/log_f.txt $data_url/log_f_new.txt
#scp -o ConnectTimeout=1 $B0:/tmp/receiver.txt $data_url/receiver_new.txt
#scp -o ConnectTimeout=1 $B0:/tmp/log_r.txt $data_url/log_r_new.txt

