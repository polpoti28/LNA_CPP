#!/bin/bash
source /u/sw/etc/profile
module load gcc-glibc
module load lis

mpirun -n 4 ./eigen1 L_s.mtx eigvec.txt hist.txt -e pi -shift 29.5 -etol 1.e-8 -emaxiter 3000
