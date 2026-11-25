#  @(#) $Id: UsedFilesList.sh $
echo "########## Used Audio files ##########"
grep "\<audio expr=\"session.application.path" $1/xml/main.vxml |cut -d'/' -f3|cut -d"'" -f1 |sort -u
echo "######################################"
echo "########## Used Script files #########"
grep "\<script" $1/xml/main.vxml |cut -d'/' -f3|cut -d"'" -f1 |sort -u
echo "######################################"
sh garbageCollection.sh $1
