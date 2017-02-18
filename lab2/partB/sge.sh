#!/bin/bash
module load sge
module load openmpi

for job in sge2.sh sge4.sh sge8.sh sge16.sh
do
    qsub $job
done

