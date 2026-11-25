#!/bin/sh

source /opt/ivr/etc/scripts/ivr.env

IVR_MIS_FILE_PATH=/opt/ivr/var/EVRRecord/processing/
IVR_MIS_BACKUP_PATH=/opt/ivr/var/EVRRecord/processed/
#DDMMYYHH=$(date +"%d%m%y")

echo "Finding the files in $IVR_MIS_FILE_PATH"

cd $IVR_MIS_FILE_PATH

#list=$(find $IVR_MIS_FILE_PATH -name "*MIS*" -mmin +0)
#list=$(find -name "*MIS*" -mmin +0)
list=$(find -name "*IVR_EVR*" -mmin +0)

if [ "$list" == "" ]; then
	echo "No Files available to upload"
	exit;	
fi

echo "------------------------------------"
echo "File Found : $list"

for i in $list
do
      j=$(echo $i | cut -c 3-)
		DDMMYYHH=$(echo $j | cut -c 9-17)
		echo "Concatination file $IVR_MIS_FILE_PATH/$j to $IVR_MIS_FILE_PATH/upload_file_mis_${DDMMYYHH}"

		cat $IVR_MIS_FILE_PATH/$j >> $IVR_MIS_FILE_PATH/upload_file_mis_${DDMMYYHH}

	        #rm -f $IVR_MIS_FILE_PATH/$j
                mv -f $j $IVR_MIS_BACKUP_PATH
done

echo "Creating ${IVR_MIS_FILE_PATH}/UPLOAD_MIS.sh"

upLoadList=$(ls |grep "upload_file_mis_*")
echo -e "#!/bin/sh\n\n" > ${IVR_MIS_FILE_PATH}/UPLOAD_MIS.sh
echo -e "/usr/bin/mysql -uivr -pivr ivr << EOF\n" >> ${IVR_MIS_FILE_PATH}/UPLOAD_MIS.sh

for lst in $upLoadList
do
    DDMMYYHH=$(echo $lst | rev | cut -c 6-9| rev)
    echo -e "LOAD DATA LOCAL INFILE '${IVR_MIS_FILE_PATH}/$lst' INTO TABLE IVR_EVR_DETAILS_${DDMMYYHH} FIELDS TERMINATED BY ',' lines TERMINATED BY '\n';\n\n" >> ${IVR_MIS_FILE_PATH}/UPLOAD_MIS.sh
done
echo EOF >> ${IVR_MIS_FILE_PATH}/UPLOAD_MIS.sh

echo "Created ${IVR_MIS_FILE_PATH}/UPLOAD_MIS.sh"

echo "------------------------------------"
echo -n "Connecting to MySql MisData for uploading ${IVR_MIS_FILE_PATH}/UPLOAD_MIS.sh Uploading TIME:"
date +"%d-%m-%y-%H-%M-%S-%N"

sh -x ${IVR_MIS_FILE_PATH}/UPLOAD_MIS.sh

echo -n "Upload Completed Deleting ${IVR_MIS_FILE_PATH}/UPLOAD_MIS.sh TIME:"
date +"%d-%m-%y-%H-%M-%S-%N"
echo "------------------------------------"

rm -f ${IVR_MIS_FILE_PATH}/UPLOAD_MIS.sh

echo -n "Removing File : ${IVR_MIS_FILE_PATH}/upload_file_mis*"
rm -f ${IVR_MIS_FILE_PATH}/upload_file_mis*
echo ""

cd -
