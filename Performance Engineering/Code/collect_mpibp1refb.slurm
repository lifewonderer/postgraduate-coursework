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

for n in 1 2 4 8 16 32 64 128;
do
    n_size=$((n))
    runtime=$(mpiexec -n ${n} ./bpsraw -problem bp1 -ceed /cpu/self/ref/blocked -degree 16 -local 1000 ls -l | sed -n '23p' | cut -c 42-48)
    dofssec=$(mpiexec -n ${n} ./bpsraw -problem bp1 -ceed /cpu/self/ref/blocked -degree 16 -local 1000 ls -l | sed -n '24p' | cut -c 42-48)
    echo $n_size $runtime $dofssec
done

for n in 1 2 4 8 16 32 64 128;
do
    n_size=$((n))
    runtime=$(mpiexec -n ${n} ./bpsraw -problem bp1 -ceed /cpu/self/ref/blocked -degree 16 -local 10000 ls -l | sed -n '23p' | cut -c 42-48)
    dofssec=$(mpiexec -n ${n} ./bpsraw -problem bp1 -ceed /cpu/self/ref/blocked -degree 16 -local 10000 ls -l | sed -n '24p' | cut -c 42-48)
    echo $n_size $runtime $dofssec
done

for n in 1 2 4 8 16 32 64 128;
do
    n_size=$((n))
    runtime=$(mpiexec -n ${n} ./bpsraw -problem bp1 -ceed /cpu/self/ref/blocked -degree 16 -local 100000 ls -l | sed -n '23p' | cut -c 42-48)
    dofssec=$(mpiexec -n ${n} ./bpsraw -problem bp1 -ceed /cpu/self/ref/blocked -degree 16 -local 100000 ls -l | sed -n '24p' | cut -c 42-48)
    echo $n_size $runtime $dofssec
done