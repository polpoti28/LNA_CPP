#!/bin/bash
source /u/sw/etc/bash.bashrc
module load gcc-glibc
module load lis

mpirun -n 4 ./etest5 L_s.mtx eigval_i.mtx eigvec_i.mtx hist_i.txt iters.txt -e si -ss 2 -etol 1.e-10 -emaxiter 3000