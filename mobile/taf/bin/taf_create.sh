
APP=${1}
SRV=${2}

#�����ļ�
#�Ḳ�� ${APP}/${SRV}/${SRV}Servant.jce

/usr/local/taf/create_taf_server.sh  ${APP}  ${SRV}Server  ${SRV}Servant

#�༭jce�ļ�
#����jce�ļ������ӿ��ļ�
cd ${APP}/${SRV}Server/
vim ${SRV}Servant.jce
/usr/local/taf/bin/jce2cpp   ${SRV}Servant.jce
