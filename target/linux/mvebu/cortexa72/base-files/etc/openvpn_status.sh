runFlag=true;
service_status="running"
while [[ "$runFlag" = true ]]; do
  #statements
  ans=`/etc/init.d/openvpn status`
  #echo $ans", "${#ans}
  result=$(echo $ans | grep "${service_status}")
  if [[ "$result" != "" ]]
    then
    #echo "yes"
    echo -n -e "\x40\x52\x30\x30\x12" > /dev/ttyS1
  else
    #echo "no"
    echo -n -e "\x40\x52\x30\x31\x13" > /dev/ttyS1
  fi
  usleep 1000000
done
