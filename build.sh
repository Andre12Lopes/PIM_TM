#/bin/bash

if [[ $# != 4 ]]; then
	echo "Invalid number of parameters"
	echo "Usage: ./build.sh [backend {norec|rwlocks|tiny_wbctl|tiny_wbetl|tiny_wtetl}] [benchmark {bank|linkedlist|kmeans|labyrinth}] [contention {e.g. #bank accounts}] [#tasklets {1 .. 24}]"
	exit 1
fi

backend_folder=
backend_lib=
benchmark_folder=
tiny_mode=

case $1 in
	"norec" )
		backend_folder="NoREC"
		backend_lib="norec"
		;;
	"rwlocks" )
		backend_folder="RWLocksSTM"
		backend_lib="rwlocks"
		;;
	"tiny_wbctl" )
		backend_folder="TinySTM"
		backend_lib="tiny"
		tiny_mode="WRITE_BACK_CTL=1"
		;;
	"tiny_wbetl" )
		backend_folder="TinySTM"
		backend_lib="tiny"
		tiny_mode="WRITE_BACK_ETL=1"
		;;
	"tiny_wtetl" )
		backend_folder="TinySTM"
		backend_lib="tiny"
		tiny_mode="WRITE_THROUGH_ETL=1"
		;;
	* )
		echo ""
		echo "==================== ERROR UNKNOWN BACKEND $1 ===================="
		echo ""
		exit 1
		;;
esac

case $2 in
	"bank" )
		benchmark_folder="Bank"
		;;
	"linkedlist" )
		benchmark_folder="LinkedList"
		;;
	"kmeans" )
		benchmark_folder="Kmeans"
		;;
	"labyrinth" )
		benchmark_folder="Labyrinth"
		;;
	* )
		echo ""
		echo "==================== ERROR UNKNOWN BENCHMARK $2 ===================="
		echo ""
		exit 1
		;;
esac

if (( $4 < 1 || $4 > 24 )); then
	echo ""
	echo "==================== NUMBER OF TASKLETS MUST BE IN [1 .. 24] ===================="
	echo ""
	exit 1
fi

bash clean.sh

common_flags="TX_IN_MRAM= DATA_IN_MRAM=1 BACKOFF=1"

tm_flags="$tiny_mode"

benchmark_lib_flags="FOLDER=$backend_folder LIB=$backend_lib NR_TASKLETS=$4"

cd Backends/$backend_folder
make $tm_flags $common_flags
cd ../..

if [[ $benchmark_folder == "Bank" ]]; then
	bank_flags="N_ACCOUNTS=$3"

	cd $benchmark_folder
	make $common_flags $benchmark_lib_flags $bank_flags
	cd ..
fi

if [[ $benchmark_folder == "LinkedList" ]]; then
	linkedlist_flags="UPDATE_PERCENTAGE=$3"

	cd $benchmark_folder
	make $common_flags $benchmark_lib_flags $linkedlist_flags
	cd ..
fi

if [[ $benchmark_folder == "Kmeans" ]]; then
	kmeans_flags="N_CLUSTERS=$3"

	cd $benchmark_folder
	make $common_flags $benchmark_lib_flags $kmeans_flags
	cd ..
fi

if [[ $benchmark_folder == "Labyrinth" ]]; then
	# kmeans_flags="N_CLUSTERS=$3"

	cd $benchmark_folder
	make $common_flags $benchmark_lib_flags # $kmeans_flags
	cd ..
fi
