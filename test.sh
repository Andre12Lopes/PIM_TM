#/bin/bash

if [[ $# != 3 ]]; then
	echo "Invalid number of parameters"
	echo "Usage: ./test.sh [backend {norec|rwlocks|tiny_wbctl|tiny_wbetl|tiny_wtetl}] [benchmark {bank|linkedlist|kmeans}] [contention {e.g. #bank accounts}]"
	exit 1
fi

if [[ ! -d "Results/${2^}/$1" ]]; then
	mkdir -p "./Results/${2^}/$1"
fi

echo -e "N_THREADS\tN_TRANSACTIONS\tTIME\tN_ABORTS\tPROCESS_READ_TIME\tPROCESS_WRITE_TIME\tPROCESS_VALIDATION_TIME\tPROCESS_OTHER_TIME\tCOMMIT_VALIDATION_TIME\tCOMMIT_OTHER_TIME\tWASTED_TIME" > Results/${2^}/$1/results_$3.txt

for (( i = 1; i < 13; i++ )); do
	bash build.sh $1 $2 $3 $i
	for (( j = 0; j < 1; j++ )); do
		./${2^}/bin/host >> Results/${2^}/$1/results_$3.txt
	done
done