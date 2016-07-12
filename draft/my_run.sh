#!/bin/bash

if [ "$#" -ne 5 ]
then
  echo "Usage: $0 num_task m_compt infly_high exp_dir" >&2
  exit 1
fi

num_task=$1
m_compt=$2
infly_high=$3
exp_dir=$4

generator_program=generator
forwarder_program=forwarder
matcher_program=matcher
receiver_program=receiver
backup_program=backup

G0=192.168.0.10
F0=192.168.0.20
M0=192.168.0.30
M1=192.168.0.40
R0=192.168.0.50
B0=root@192.168.0.60

cd $(dirname $0)

data_url=data/$exp_dir
mkdir -p $data_url
echo "num=$num_task compt=$m_compt in-flight=$infly_high" > $data_url/description.txt

./my_copy.sh

ssh -o ConnectTimeout=1 $R0 "cd /tmp; screen -d -m ./receiver.sh"
sleep 1
ssh -o ConnectTimeout=1 $B0 "cd /tmp; screen -d -m ./backup.sh"
ssh -o ConnectTimeout=1 $M1 "cd /tmp; screen -d -m ./matcher.sh 2 $m_compt"
sleep 1
ssh -o ConnectTimeout=1 $M0 "cd /tmp; screen -d -m ./matcher.sh 3 $m_compt"
sleep 1
ssh -o ConnectTimeout=1 $F0 "cd /tmp; screen -d -m ./forwarder.sh $infly_high"
sleep 3
ssh -o ConnectTimeout=1 $G0 "cd /tmp; ./generator.sh $num_task"

sleep 1
./my_paste.sh $exp_dir

