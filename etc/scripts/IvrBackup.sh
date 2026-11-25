# @(#) $Id: $
#!/bin/bash
#########################################################################################
#       Script for taking IVR system back up for IVR system                             #
#       IvrBackup.sh Ver 1.1                                                           #
#       This Back up includes the following backup only                                 #
#               1.      IVR Core                                                       #
#               2.      OAM                                                             #
#               3.      IVR and OAM web GUI                                            #
#########################################################################################
# Set variables and Source the environment

TE=`date +%a`
DT=`date +%b`
BACKUP_DIR=/backup/SystemBackup/
mkdir -p ${BACKUP_DIR}/${TE}
chmod -R 755 ${BACKUP_DIR}/${TE}
source /opt/ivr/etc/scripts/ivr.env

echo ""
echo "IVR Core Backup is in Progress. Please wait....."
echo ""
sleep 2
tar -Pzcf ${BACKUP_DIR}/IvrBackup.tar.gz /opt/ivr/bin/ /opt/ivr/etc/ /opt/ivr/ivrgui/ /opt/ivr/www/ /opt/ivr/xml/ /opt/ivr/audio/ /home/ivr/.bashrc
echo ""
sleep 2
tar -Pzcf ${BACKUP_DIR}/CoamBackup.tar.gz /opt/oam/etc/ /opt/oam/bin/ /opt/oam/coam/ /opt/jakarta-tomcat-4.1.12/

echo "IVR Core Backup Completed .......... "

mv ${BACKUP_DIR}/IvrBackup.tar.gz ${BACKUP_DIR}/${TE}/IvrBackup.${TE}.tar.gz
mv ${BACKUP_DIR}/CoamBackup.tar.gz ${BACKUP_DIR}/${TE}/CoamBackup.${TE}.tar.gz

cd ${BACKUP_DIR}/${TE}
tar -zcf  Ivr_${TE}.tar.gz IvrBackup.${TE}.tar.gz CoamBackup.${TE}.tar.gz

HOST='xxx.xxx.xxx.xxx'
USER='ivr'
PASSWD='ivr'
FILE=Ivr_${TE}.tar.gz
cd ${BACKUP_DIR}/${TE}

ftp -n $HOST <<END_SCRIPT
quote USER $USER
quote PASS $PASSWD
cd /backup/SystemBackup/
binary
put $FILE $FILE
quit
END_SCRIPT
