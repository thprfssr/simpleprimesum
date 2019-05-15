#!/bin/bash
# This script checks the run time of the primesum program for a random n
# between a and b, which are the two required arguments.

datafile=data.tsv

function timing () {
local START=$(date +%s%N)
./primesum $1
local FINISH=$(date +%s%N)
delta=$(($FINISH-$START))

return $delta
}

# usage: (min, max)
function run {
while true; do
	n=$(shuf -i $1-$2 -n 1)
	timing $n
	t=$delta
	echo -e "$n\t$t" | tee -a $datafile
done
}

function help {
cat <<-EOF
$0 - Collect data on the efficiency of Albert's primesum algorithm on your machine

usage: $0 [upper bound]
usage: $0 [lower bound] [upper bound]

* This script chooses random integers within an interval, and runs the
primesum program with each of the generated integers in order to record the
time it takes primesum to complete.
* The output file is set to $datafile
* The default lower bound is 1.
EOF
}

function get_bounds {
# If the number of passed arguments is 1, then we interpret that as
# a lower bound
if [ $# -eq 1 ]; then
	case $1 in
		''|*[!0-9]*)
			help
			exit
			;;
		*)
			max=$1
			run 1 $max
			exit
			;;
	esac

# If the number of passed arguments is 2, then we interpret them as lower and
# upper bounds, respectively.
elif [ $# -eq 2 ]; then
	case $1$2 in
		''|*[!0-9]*)
			help
			exit
			;;
		*)
			if (( $1 >= $2 )); then
				help
				exit
			else
				min=$1
				max=$2
				run $1 $2
				exit
			fi
			exit
			;;
	esac

# Else, then we simply print the help menu.
else
	help
	exit
fi
}


get_bounds "$@" # We pass all the command line arguments to the get_bounds function.
