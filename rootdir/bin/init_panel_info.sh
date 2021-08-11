#!/vendor/bin/sh

# Update the panel color property and Leds brightness
for i in $(seq 5); do
	#Focal
    if [ -f /sys/bus/platform/devices/ts_focal/panel_info/cg_color ]; then
		color=`cat /sys/bus/platform/devices/ts_focal/panel_info/cg_color`
		if [ -n "$color" ]; then
		    /vendor/bin/log -p i -t panel-info-sh Get panel_color successfully from sys node $color
		    break
		else
		    /vendor/bin/log -p i -t panel-info-sh Get panel_color unsuccessfully, try again...
		    sleep 1
		    continue
		fi
	#Novatek
    elif [ -f  /sys/bus/platform/devices/ts_novatek/panel_info/cg_color ]; then
        color=`cat /sys/bus/platform/devices/ts_novatek/panel_info/cg_color`
        if [ -n "$color" ]; then
            /vendor/bin/log -p i -t panel-info-sh Get panel_color successfully from sys node $color
            break
        else
            /vendor/bin/log -p i -t panel-info-sh Get panel_color unsuccessfully, try again...
            sleep 1
            continue
        fi
    else
	color="0"
	/vendor/bin/log -p i -t panel-info-sh Get panel_color unsuccessfully, try again...
	sleep 1
    fi
done

for i in $(seq 5); do
	#Focal
    if [ -f  /sys/bus/platform/devices/ts_focal/panel_info/cg_maker ]; then
	panel_vendor=`cat /sys/bus/platform/devices/ts_focal/panel_info/cg_maker`
		if [ -n "$panel_vendor" ]; then
		    /vendor/bin/log -p i -t panel-info-sh Get panel_vendor successfully from sys node $panel_vendor
		    break
		else
		    /vendor/bin/log -p i -t panel-info-sh Get panel_vendor unsuccessfully, try again...
		    sleep 1
		    continue
		fi
	#Novatek
    elif [ -f  /sys/bus/platform/devices/ts_novatek/panel_info/cg_maker ]; then
        panel_vendor=`cat /sys/bus/platform/devices/ts_novatek/panel_info/cg_maker`
        if [ -n "$panel_vendor" ]; then
            /vendor/bin/log -p i -t panel-info-sh Get panel_vendor successfully from sys node $panel_vendor
            break
        else
            /vendor/bin/log -p i -t panel-info-sh Get panel_vendor unsuccessfully, try again...
            sleep 1
            continue
        fi
    else
	panel_vendor="0"
	/vendor/bin/log -p i -t panel-info-sh Get panel_vendor unsuccessfully, try again...
	sleep 1
    fi
done

for i in $(seq 5); do
	#Focal
    if [ -f  /sys/bus/platform/devices/ts_focal/panel_info/display_maker ]; then
		panel_display=`cat /sys/bus/platform/devices/ts_focal/panel_info/display_maker`
		if [ -n "$panel_display" ]; then
		    /vendor/bin/log -p i -t panel-info-sh Get panel_display successfully from sys node $panel_display
		    break
		else
		    /vendor/bin/log -p i -t panel-info-sh Get panel_display unsuccessfully, try again...
		    sleep 1
		    continue
		fi
	#Novatek
    elif [ -f  /sys/bus/platform/devices/ts_novatek/panel_info/display_maker ]; then
        panel_display=`cat /sys/bus/platform/devices/ts_novatek/panel_info/display_maker`
        if [ -n "$panel_display" ]; then
            /vendor/bin/log -p i -t panel-info-sh Get panel_display successfully from sys node $panel_display
            break
        else
            /vendor/bin/log -p i -t panel-info-sh Get panel_display unsuccessfully, try again...
            sleep 1
            continue
        fi
    else
	panel_display=""
	/vendor/bin/log -p i -t panel-info-sh Get panel_display unsuccessfully, try again...
	sleep 1
    fi
done

case "$color" in
    "1")
        setprop sys.panel.color WHITE
        ;;
    "2")
        setprop sys.panel.color BLACK
        ;;
    "3")
        setprop sys.panel.color RED
        ;;
    "4")
        setprop sys.panel.color YELLOW
        ;;
    "5")
        setprop sys.panel.color GREEN
        ;;
    "6")
        setprop sys.panel.color PINK
        ;;
    "7")
        setprop sys.panel.color PURPLE
        ;;
    "8")
        setprop sys.panel.color GOLDEN
        ;;
    "9")
        setprop sys.panel.color SLIVER
        ;;
    "@")
        setprop sys.panel.color GRAY
        ;;
    "A")
        setprop sys.panel.color SLIVER_BLUE
        ;;
    "B")
        setprop sys.panel.color CORAL_BLUE
        ;;
    *)
        setprop sys.panel.color UNKNOWN
        ;;
esac
case "$panel_vendor" in
    "1")
        setprop sys.panel.vendor BIELTPB
        ;;
    "2")
        setprop sys.panel.vendor LENS
        ;;
    "3")
        setprop sys.panel.vendor WINTEK
        ;;
    "4")
        setprop sys.panel.vendor OFILM
        ;;
    "5")
        setprop sys.panel.vendor BIELD1
        ;;
    "6")
        setprop sys.panel.vendor TPK
        ;;
    "7")
        setprop sys.panel.vendor LAIBAO
        ;;
    "8")
        setprop sys.panel.vendor SHARP
        ;;
    "9")
        setprop sys.panel.vendor JDI
		;;
    "@")
        setprop sys.panel.vendor EELY
        ;;
    "A")
        setprop sys.panel.vendor GISEBBG
        ;;
    "B")
        setprop sys.panel.vendor LGD
        ;;
    "C")
        setprop sys.panel.vendor AUO
        ;;
    "D")
        setprop sys.panel.vendor BOE
        ;;
    "E")
        setprop sys.panel.vendor DSMUDONG
        ;;
    "F")
        setprop sys.panel.vendor TIANMA
        ;;
    "G")
        setprop sys.panel.vendor TRULY
        ;;
    "H")
        setprop sys.panel.vendor SDC
        ;;
    "I")
        setprop sys.panel.vendor PRIMAX
        ;;
    "P")
        setprop sys.panel.vendor SZZC
        ;;
    "Q")
        setprop sys.panel.vendor GVO
        ;;
    "R")
        setprop sys.panel.vendor VITALINK
        ;;
    *)
        setprop sys.panel.vendor UNKNOWN
        ;;
esac
case "$panel_display" in
    "1")
        setprop sys.panel.display JDI
        ;;
    "2")
        setprop sys.panel.display LGD
        ;;
    "3")
        setprop sys.panel.display SHARP
        ;;
    "4")
        setprop sys.panel.display AUO
        ;;
    "5")
        setprop sys.panel.display BOE
        ;;
    "6")
        setprop sys.panel.display TIANMA
        ;;
    "7")
        setprop sys.panel.display EBBG
        ;;
    "8")
        setprop sys.panel.display SDC
        ;;
    "9")
        setprop sys.panel.display EDO
		;;
    "0")
        setprop sys.panel.display OFILM
        ;;
    "B")
        setprop sys.panel.display CSOT
        ;;
    *)
        setprop sys.panel.display UNKNOWN
        ;;
esac
