#!/bin/bash
BOARD=`cat /sys/firmware/devicetree/base/model`

if [ "$BOARD" = "iEi Puzzle-M902" ]
then
/etc/init.d/network stop
nicslist="eth0 eth1 eth2 eth3 eth4 eth5 eth6 eth7 eth8"
      for nics in $nicslist
         do
         ifconfig $nics down
         echo down_nic=$nics
      done
#ifconfig -a | grep HW | awk '{ print $1" : "$5 }' > ${testfile}
      for nics in $nicslist
         do
         IFACE=$nics
         read MAC </sys/class/net/$IFACE/address
         #echo $IFACE $MAC
           case $nics in
             eth0) nameif eth10 $MAC;; ##10G
             eth1) echo "eth1" $MAC;;
             eth2) nameif eth0 $MAC;;
             eth3) nameif eth11 $MAC;; ##10G
             eth4) nameif eth3 $MAC;;
             eth5) nameif eth2 $MAC;;
             eth6) nameif eth12 $MAC;; ##10G
             eth7) nameif eth5 $MAC;;
             eth8) nameif eth4 $MAC;;
           esac
       done
sleep 1
/etc/init.d/network restart
fi
