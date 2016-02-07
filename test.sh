# Seths First Shell Script to give pretty graphs for my report
#!/bin/bash
counter=100
while [ $counter -lt 1500];do
	./matrixmult $counter
	((counter+=100))
done

