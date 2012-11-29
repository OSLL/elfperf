#!/bin/bash

./cpufreqmeter

cpufreq=`cat ./cpufreq.tmp`

export ELFPERF_CPU_FREQ=`cat ./cpufreq.tmp`
echo $"ENV_VAR VALUE: "`echo $ELFPERF_CPU_FREQ`

