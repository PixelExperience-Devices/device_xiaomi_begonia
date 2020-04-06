#! /system/bin/sh
IMEI=$(getprop persist.radio.imei)

if [ -z "$IMEI" ];
then
	echo "IMEI is empty, use default value"
	IMEI="000000"
fi

if [ ! -d /data/misc/tp_selftest_data ];
then
	mkdir -p /data/misc/tp_selftest_data
fi

FILE_NAME=/data/misc/tp_selftest_data/${IMEI}".csv"
echo $FILE_NAME

if [ -f /proc/nvt_test_data ]; then
	cat /proc/nvt_test_data > $FILE_NAME
elif [ -f /proc/fts_test_data ]; then
	cat /proc/fts_test_data > $FILE_NAME
else
	echo "ERROR: proc interface for tp testdata is not exist, can't collect testdata"
fi
