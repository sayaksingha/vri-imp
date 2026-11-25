#  @(#) $Id: garbageCollection.sh $
#!/bin/sh
#cd $3
audioList=$(ls $1/audio)
scriptList=$(ls $1/script)
count=0
audioIndex=1
scriptIndex=1
declare -a unusedAudioList
declare -a unusedScriptList

for file in $audioList
do
count=$(grep -c $file $1/xml/main.vxml)
if [ $count -eq 0 ]
then
#echo " $audioIndex --> $file"
 unusedAudioList[audioIndex]=$file
 audioIndex=$((audioIndex+1))  
fi
done


for file in $scriptList
do
count=$(grep -c $file $1/xml/main.vxml)
if [ $count -eq 0 ]
then
#echo " $scriptIndex --> $file"
 unusedScriptList[scriptIndex]=$file
 scriptIndex=$((scriptIndex+1))  
fi
done


echo "######## Unused Audio files #########"
for j in $(seq 1 $((audioIndex-1)))
do
  echo "$j --> ${unusedAudioList[$j]}"
done

echo "######## Unused Script files ########"
for j in $(seq 1 $((scriptIndex-1)))
do
  echo "$j --> ${unusedScriptList[$j]}"
done
#sed -e "s/$1/$2/ig" $file > /tmp/tempfile.tmp
#mv /tmp/tempfile.tmp $file

#cd -

