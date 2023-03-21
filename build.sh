#/bin/bash

bash clean.sh

backend_folder=
backend_lib=
benchmark_folder=

case $1 in
	"norec" )
		backend_folder="NoREC"
		backend_lib="norec"
		;;
	"rwlocks" )
		backend_folder="RWLocksSTM"
		backend_lib="rwlocks"
		;;
	"tiny" )
		backend_folder="TinySTM"
		backend_lib="tiny"
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
	* )
		echo ""
        echo "==================== ERROR UNKNOWN BENCHMARK $2 ===================="
		echo ""
        exit 1
		;;
esac

common_flags="TX_IN_MRAM= DATA_IN_MRAM=1 BACKOFF=1"

tm_flags="WRITE_BACK_CTL= WRITE_BACK_ETL= WRITE_THROUGH_ETL=1 R_SET_SIZE=10 W_SET_SIZE=10 LOCK_ARRAY_LOG_SIZE=10"

benchmark_lib_flags="FOLDER=$backend_folder LIB=$backend_lib"

cd Backends/$backend_folder
make $tm_flags $common_flags
cd ../..

if [[ $benchmark_folder == "Bank" ]]; then
	bank_flags="N_ACCOUNTS=800 NR_TASKLETS=$3"

	cd $benchmark_folder
	make $common_flags $benchmark_lib_flags $bank_flags
	cd ..
fi


# if [[ $backend == "norec" ]]; then
# 	echo "HEREEEEE"
# elif [[ condition ]]; then
# 		#statements
# elif [[ condition ]]; then
# 	#statements
# fi