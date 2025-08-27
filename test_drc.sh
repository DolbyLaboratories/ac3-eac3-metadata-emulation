#!/bin/bash

# 5.1 Line Mode
#./test_case51.sh test/sources/6ch_Skip_51CM.wav "-p0 -s0 -a7 -dn1 -c2" "-a7 -dn1" "-k2"
# 5.1 RF Mode
#./test_case51.sh test/sources/6ch_Skip_51CM.wav "-p0 -s0 -a7 -dn11 -c3" "-a7 -dn11" "-k3"

if [ ! -f "build_release/MdEmu" ]; then
        echo "Executable does not exist, rebuilding"
        if [ ! -d "build_release" ]; then
  			conan install . --output-folder=build_release --build=missing -s build_type=Release
  		fi
		cd build_release
		if [ ! -f "Makefile" ]; then
			cmake -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
		fi
		make
		cd ..
fi

export gc=./build_release/graph_check

rm -fR test/output test/tmp
mkdir -p test/output
mkdir -p test/tmp

export source_file=test/sources/6_comp.wav

comp_no=2
pass_num=0
fail_num=0

for comp in line rf; do

	prof_no=1

	for prof in fs fl ms ml sp; do
		./test/test_case51_emu.sh $source_file 7 31 $prof_no $comp_no 
		((prof_no++))
		for chan in 0 1 2 4 5; do
			$gc ./test/output/6_comp.emu."$chan".res ./test/reference/"$comp"_"$prof"_"$chan".res -max 1.6 -dev 0.6
			if [ $? -eq "0" ]; then
				((pass_num++))
			else
				((fail_num++))
			fi
		done
	done
	((comp_no++))
done

echo "Number of passes: " $pass_num
echo "Number of failures: " $fail_num
if [ $fail_num -eq "0" ]; then
	echo "Overall Result: Pass"
	exit 0
else
	echo "Overall Result: Fail"
	exit 1
fi
