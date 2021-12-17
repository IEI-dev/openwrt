runFlag=true;
checkCount=0;
maxCheckCount=20;
while [[ "$runFlag" = true ]]; do
  if (($checkCount >= $maxCheckCount));then
      # echo "pass to run";
      checkCount=0
      sysupgrade -r /etc/backup-puzzle-m90x.tar.gz
      # sysupgrade -r /etc/back-fake-.tar.gz
      # sysupgrade -r /etc/back-fake-.tar.g
      if [ $? == 0 ]; then
	echo -n -e "\x40\x52\x32\x32\x12" > /dev/ttyS1
        echo -n -e "\x40\x43\x32\x31" > /dev/ttyS1
        runFlag=false;
      fi
  fi
  #statements
  ans=`devmem 0xf2440150`
  # echo $ans", "${#ans}
  if ((${#ans} == 10));then
      ans=${ans: 8: 1}
      ans=$((16#${ans}%2))
      if [[ $ans = 1 ]]; then
      checkCount=0;
      else
      # press reset button
      checkCount=$(( $checkCount + 1 ))
      fi
  fi
  usleep 500000
done

reboot
