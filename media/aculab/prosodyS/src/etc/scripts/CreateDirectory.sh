# @(#) $Id: crtpro,v 1.1.4.1.6.1 2013/12/11 12:25:35 mrudulab Exp $
#----------------------------------------------------------------------
# NAME
#       CreateDirectory.sh
#
# COPYRIGHT
# Tayana Software Solutions Ltd -- Copyright(C) 2015
# All rights reserved. No part of this computer program
# may be used or reproduced in any form by any
# means without prior written permission of
# Tayana Software Solutions Ltd
#
# DESCRIPTION :
#
# AUTHOR         : Sip                                  23-Feb-2015
#
# 1.MODIFIED BY  : <Name>                                <Date>
#   MODIFICATION : <Details>
#----------------------------------------------------------------------

#! /bin/csh

source /opt/sip/etc/scripts/sip.env
#-----------------------------------------------------------------
echo Creating Directory structure...

echo " "
echo "/opt/sip/"
echo "         |--tmp"
echo "             |--DiskMonitor"
echo "             |--ProcessLock"
echo "         |--var"
echo "             |--log"
echo "             |--mis"
echo "             |--trace"
echo "             |--evrrecord"
echo "             |--cdrrecord"
echo "             |--db"
echo "                 |--oradata"
echo "                     |--BPLORA"
echo " "

#-----------------------------------------------------------------
# Creating Directories...

mkdir /tmp
mkdir /tmp/DiskMonitor
mkdir /tmp/ProcessLock
mkdir /var
mkdir /var/log
mkdir /var/mis
mkdir /var/trace
mkdir /var/audit
mkdir /var/evrrecord
mkdir /var/cdrrecord
mkdir /var/db
mkdir /var/db/oradata
mkdir /var/db/oradata/BPLORA

echo Creating Directories Success
echo  
