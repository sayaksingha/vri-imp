#  @(#) $Id: IvrChannelDetails.sh $
# ------------------------------------------------------------------------------ #
#         Invoke IVR_ChannelDetails binary to populate Channel Details           #
# ------------------------------------------------------------------------------ #

source /opt/ivr/etc/scripts/ivr.env
/opt/ivr/bin/IVR_ChannelDetails $1 $2 $3
