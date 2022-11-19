#!/bin/bash

# 1 core
#SBATCH -n 1
#SBATCH --job-name="collect"
#SBATCH -o collect.%J.out
#SBATCH -e collect.%J.err
#SBATCH -p par7.q

source /etc/profile.d/modules.sh

module load intel/2019.6
module load gcc/9.3.0

for n in 1000 2000 4000 8000 10000 20000 40000 80000 100000 200000 400000 500000;
do
    for p in $(seq 1 16)
    do
        n_size=$((n))
        p_size=$((p))
        runtime=$(./bpsraw -problem bp1 -ceed /cpu/self/avx/serial -degree ${p} -local ${n} ls -l | sed -n '23p' | cut -c 42-48)
        dofssec=$(./bpsraw -problem bp1 -ceed /cpu/self/avx/serial -degree ${p} -local ${n} ls -l | sed -n '24p' | cut -c 42-48)
        echo $n_size $p_size $runtime $dofssec
    done
done