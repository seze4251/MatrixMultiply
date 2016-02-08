#!/bin/bash
# Seths First Shell Script to give pretty graphs for my report
counter="400"
while [ $counter -lt 4000 ]
do
	./matrixmult $counter
	((counter+=100))
done

