#!/bin/bash

back="norec rwlocks_wbctl rwlocks_wbetl rwlocks_wtetl tiny_wbctl tiny_wbetl tiny_wtetl"

for b in $back; do
	./test.sh $b bank 10000
done
