#!/bin/bash

rm -f elfperf_results_*

make -C .. disable_console_output 1>/dev/null 2>/dev/null
make -C .. lib_32 1>/dev/null 2>/dev/null
make test_functional_32

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../libelfperf.so:/tools/lib/libdl.so.2 ELFPERF_ENABLE=true \
ELFPERF_PROFILE_FUNCTION=\
testFunction1\
:testFunction2\
:testFunction3\
:testFunction4\
:testFunction6\
:testFunction8 /tools/lib/ld-2.11.3.so ./test_functional' 1>/dev/null 2>/dev/null

count=6
testFunctions=(testFunction1 testFunction2 testFunction3 testFunction4 testFunction6 testFunction8)
stats=\
(\
 `cat elfperf_results_* | grep 'Statistic for testFunction1' | sed 's/.*total calls number = //' | sed 's/, total ticks .* /,/' | sed 's/,.*/ /'`\
 `cat elfperf_results_* | grep 'Statistic for testFunction2' | sed 's/.*total calls number = //' | sed 's/, total ticks .* /,/' | sed 's/,.*/ /'`\
 `cat elfperf_results_* | grep 'Statistic for testFunction3' | sed 's/.*total calls number = //' | sed 's/, total ticks .* /,/' | sed 's/,.*/ /'`\
 `cat elfperf_results_* | grep 'Statistic for testFunction4' | sed 's/.*total calls number = //' | sed 's/, total ticks .* /,/' | sed 's/,.*/ /'`\
 `cat elfperf_results_* | grep 'Statistic for testFunction6' | sed 's/.*total calls number = //' | sed 's/, total ticks .* /,/' | sed 's/,.*/ /'`\
 `cat elfperf_results_* | grep 'Statistic for testFunction8' | sed 's/.*total calls number = //' | sed 's/, total ticks .* /,/' | sed 's/,.*/ /'`
)

correct_stat=100

for (( i=0; i<$count; i++))
do
    if [ "${stats[$i]}" -eq $correct_stat ]
    then
        echo "Statistic for ${testFunctions[$i]} is CORRECT!"
    else
        echo "Statistic for ${testFunctions[$i]} is WRONG! Test Failed"
        exit
    fi
done

echo "Functional test for x86 SUCCEEDED!"
