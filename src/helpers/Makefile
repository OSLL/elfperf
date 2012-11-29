all: bin

bin:
	gcc cpufreqmeter.c ../timers/rdtsc_cntrs.c -o cpufreqmeter -m32 -lrt

clean:
	rm -f ./cpufreqmeter ./cpufreq.tmp

run:
	./export_cpufreq.sh
