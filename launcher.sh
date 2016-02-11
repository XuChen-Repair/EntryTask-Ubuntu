#!/usr/bin/env bash
SECONDS=0
for (( i = 0; i < 40; i++ )); do
	python3 benchmark.py &
done
wait
duration=$SECONDS
echo "$(($duration / 60)) minutes and $(($duration % 60)) seconds elapsed."