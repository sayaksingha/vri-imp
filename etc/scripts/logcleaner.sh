# @(#) $Id: $
#!/bin/bash
################################################################################
# NAME
# logcleaner
#
# COPYRIGHT
# BITL -- Copyright (C) 2002 Wirefree Technologies Ltd.,
# All rights reserved. No part of this computer program
# may be used or reproduced in any form by any
# means without prior written permission of
# BPL Innovision Technologies Ltd.
#
# CLASS TYPE
# concrete
#
# DESCRIPTION
# description text
#
# Originated : 23-Jan-2007 Shajahan.S Tech Support
#################################################################################

######################################################################################
######################################################################################
##This script can be use for clearing old logs with respect to logcleaner.conf entry##
##WRITEN BY SHAJAHAN.S                                                              ##
######################################################################################
######################################################################################
PWD_CONF=/opt/ivr/etc/config/

###############################
##Checking configuration file##
###############################

if [ -f $PWD_CONF/logcleaner.conf ]; then
echo "Configuration file found $PWD_CONF/logcleaner.conf"
else
echo "No configuration file found $PWD_CONF/logcleaner.conf"
exit
fi

########################################
##Taking total number of configuration##
########################################

COUNT=`cat $PWD_CONF/logcleaner.conf |grep -v "#"|grep -v "//"|grep "/"|wc -l`
COUNT=`echo $COUNT|cut -d " " -f1`


while [ $COUNT != 0 ]; do

##################################################
##Collecting information from configuration file##
##################################################

LINE=(`cat $PWD_CONF/logcleaner.conf |grep -v "#"|grep -v "//"|grep "/"|tail -$COUNT|head -1`)
LOG_PATH=`echo ${LINE[0]}`
DAYS=`echo ${LINE[1]}`
BACKUP=`echo ${LINE[2]}`
BACKUP_PATH=`echo ${LINE[3]}`
BACKUP_NAME=`echo ${LINE[4]}`

DATE=`date -d "$DAYS days ago" +%e`
MONTH=`date -d "$DAYS days ago" +%b`
DATE1=`date -d "$DAYS days ago" +%d%m%Y`

#############################################
##Collecting file list as per configuration##
#############################################

if [ -d $LOG_PATH ]; then
cd $LOG_PATH
FILE=`ls -lrth|grep "$MONTH $DATE"|tr -s " "|cut -d " " -f9`

if [ "$FILE" != "" ]; then
echo "File list collected for $MONTH $DATE from $LOG_PATH"
echo "***************************************************"
echo "$FILE"
###################################################
##Checking backup status and backup folder status##
###################################################

if [ "$BACKUP" = "y" ] || [ "$BACKUP" = "Y" ] ; then
if [ -d $BACKUP_PATH ] ; then
echo "Backup folder found $BACKUP_PATH"
else
echo "Backup folder not found, Creating Backup folder $BACKUP_PATH"
mkdir -p $BACKUP_PATH
fi

############################################
##Taking backup and removing backuped file##
############################################

cd $LOG_PATH
tar zcvf $BACKUP_PATH/$BACKUP_NAME_$DATE1.tar.gz $FILE
echo "Backup over ($BACKUP_PATH/$BACKUP_NAME_$DATE1.tar.gz)"
rm -f $FILE
echo "Removed listed files after Backup"
elif [ "$BACKUP" = "n" ] || [ "$BACKUP" = "N" ]; then
rm -f $FILE
echo "Removed listed files without taking Backup"
fi
else
echo "No files for configured date $MONTH $DATE from $LOG_PATH"
fi
else
echo "$LOG_PATH path not found"
fi
let COUNT=$COUNT-1

echo "***************************************************"
done

echo "Drive Cleaning done on `date`"
exit

