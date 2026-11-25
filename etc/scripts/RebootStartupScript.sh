#  @(#) $Id: RebootStartupScript.sh,v 1.1.1.1.2.3.2.9 2013/09/06 06:11:13 kkothari Exp $
#!/bin/sh

#*****************************************************************************
su - ivr<<END_SCRIPT

#Set the environment
source /opt/ivr/etc/scripts/ivr.env
source /opt/ivr/etc/scripts/oam.env
source /opt/ivr/etc/scripts/ivr.env

/opt/ivr/etc/scripts/IvrStart

/opt/ivr/etc/scripts/OamStart

/opt/ivr/etc/scripts/StartSynway &

quit
END_SCRIPT
