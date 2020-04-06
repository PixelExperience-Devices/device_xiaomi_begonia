#! /system/bin/sh
if [ ! -d /data/misc/tp_selftest_data ];
then
	mkdir -p /data/misc/tp_selftest_data
fi

brightness=($(cat /sys/class/leds/lcd-backlight/brightness))

if [ $brightness -eq 0 ];
then
	echo "backlight is 0, please do tp_selftest when screen on"
	echo "TEST_FAIL"
	exit
fi

if [ -f /proc/nvt_selftest ]; then
	echo "NOVATEK"
	for i in $(seq 0 3); do
		cat /proc/nvt_selftest > /data/misc/tp_selftest_data/result.txt
		test_result=($(grep FAIL /data/misc/tp_selftest_data/result.txt | wc -l))
		if [ $test_result -eq 0 ];
		then
			echo "TEST_PASS"
			exit
		else
			if [ $i -eq 3 ];
			then
				echo "TEST_FAIL"
				exit
			fi
		fi
	done
elif [ -f /sys/bus/platform/devices/ts_focal/fts_test ]; then
	echo "FOCALTECH"
	for i in $(seq 0 3); do
		echo FT8719_G7_TestINI.ini > /sys/bus/platform/devices/ts_focal/fts_test
		cat /sys/bus/platform/devices/ts_focal/fts_test > /data/misc/tp_selftest_data/result.txt
		test_result=($(grep FAIL /data/misc/tp_selftest_data/result.txt | wc -l))
		if [ $test_result -eq 0 ];
		then
			echo "TEST_PASS"
			exit
		else
			if [ $i -eq 3 ];
			then
				echo "TEST_FAIL"
				exit
			fi
		fi
	done
else
	echo "Interface for test is not exist"
	echo "TEST_FAIL"
	exit
fi
