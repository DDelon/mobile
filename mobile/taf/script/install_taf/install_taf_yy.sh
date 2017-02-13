TAFPATH="/data/app/taf/"

if [ $# -eq 1 ]; then
   IP=$1
elif [ $# -eq 0 ]; then
   IP=`/sbin/ifconfig | awk '/inet/{print $2}' | awk -F: '{print $2}'|awk '{if($1!="127.0.0.1") print}'`
fi
NUM=`echo "$IP" |wc -l`
if [ "$NUM" != "1" ];then
  echo "error2:parameter miss"
  echo "usage: install_ create_taf.sh IP"
  exit
fi


if [ -d ${TAFPATH}/tafnode ]
then
    echo "install erro!! ${TAFPATH}/tafnode is already exist!!"
    echo "exit"
    exit
fi

cp -r  taf/* $TAFPATH
cd  $TAFPATH;

if [ ! -d  /data/taf/app_log ]
then
mkdir -p /data/taf/app_log
fi


#if [ ! -d $TAFPATH/app_log ]
#then
#ln -s /data/taf/app_log/  app_log
#fi

CONF=`ls  */*/*config*  -1`
echo "$CONF"|while read line
do
	sed -e "s/172.19.103.14/${IP}/g" $line>tmp
	mv tmp $line 
	sed -e "s/node1/node/g" $line>tmp
	mv tmp $line 
done 

echo $IP

echo "over"
