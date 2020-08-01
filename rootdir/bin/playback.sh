set -x
# $1: device for output
#     spk: speaker
#     rcv: receiver
#     spk_hp: speaker high power

# tinyplay file.wav [-D card] [-d device] [-p period_size] [-n n_periods]
# sample usage: playback.sh spk
# rcv.wav:-4.5dbfs   spk: -4.8dbfs  ultra: -4.5dbfs  spk_hp:-1.8dbfs

function enable_receiver
{
    echo "enabling receiver"
    tinymix 'ADDA_DL_CH1 DL1_CH1' 1
    tinymix 'ADDA_DL_CH2 DL1_CH2' 1
    tinymix 'DAC In Mux' 'Normal Path'
    tinymix 'RCV Mux' 'Voice Playback'
}

function disable_receiver
{
    echo "disabling receiver"
    tinymix 'ADDA_DL_CH1 DL1_CH1' 0
    tinymix 'ADDA_DL_CH2 DL1_CH2' 0
    tinymix 'RCV Mux'  'Open'
}

function enable_speaker
{
    echo "enabling speaker"
    tinymix 'I2S3_CH1 DL1_CH1' 1
    tinymix 'I2S3_CH2 DL1_CH2' 1
    tinymix 'I2S3_HD_Mux' 'Low_Jitter'

    sleep 1
}

function disable_speaker
{
    echo "disabling speaker"
    tinymix 'I2S3_CH1 DL1_CH1' 0
    tinymix 'I2S3_CH2 DL1_CH2' 0
    tinymix 'I2S3_HD_Mux' 'Normal'
}

if [ "$1" = "spk" ]; then
    enable_speaker
    filename=/vendor/etc/spk.wav
elif [ "$1" = "spk_hp" ]; then
    enable_speaker
    filename=/vendor/etc/spk_hp.wav
elif [ "$1" = "rcv" ]; then
    enable_receiver
    filename=/vendor/etc/rcv.wav
else
    echo "Usage: playback.sh device; device: spk or spk_hp or rcv"
fi

echo "start playing"
tinyplay $filename

if [ "$1" = "spk" ]; then
    disable_speaker
elif [ "$1" = "spk_hp" ]; then
    disable_speaker
elif [ "$1" = "rcv" ]; then
    disable_receiver
fi

exit 0
