runFlag=true;
link_yes="yes"
link_no="no"
while [[ "$runFlag" = true ]]; do
  #statements
  ans=`ethtool eth0 | grep -i 'Link det'`
  #echo $ans", "${#ans}
  result=$(echo $ans | grep "${link_yes}")
  if [[ "$result" != "" ]]
    then
    #echo "yes"
    echo -n -e "\x40\x52\x31\x30\x13" > /dev/ttyS1
  else
    #echo "no"
    echo -n -e "\x40\x52\x31\x31\x12" > /dev/ttyS1
  fi
  usleep 1000000
done
