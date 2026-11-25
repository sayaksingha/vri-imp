#=======================================================#
# Script which removes all expired recorded audio files #
#=======================================================#
source /opt/ivr/etc/scripts/ivr.env


IVR_RECORDING_FILE_PATH=/opt/ivr/xml/recordings
IVR_MIS_FILE_LOG_PATH=/opt/ivr/var/log

echo "--------------------------------------------------------"
echo "Finding the files in $IVR_RECORDING_FILE_PATH"

cd $IVR_RECORDING_FILE_PATH


list=$(find $IVR_RECORDING_FILE_PATH -name "Record*" -mmin +30) #Removes 30 min older files


if [ "$list" == "" ]; then
        echo "No Files available to delete"
        exit;
fi

echo "------------------------------------"
echo "File Found : $list"
echo "------------------------------------"

for i in $list
do
   echo "Deleted file : "$i
   rm $i
done

echo "Deleted all expired recorded files"
date +"%d-%m-%y-%H-%M-%S-%N"
echo "------------------------------------------------------"

echo ""

cd -
