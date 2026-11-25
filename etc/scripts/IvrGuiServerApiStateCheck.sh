source ~/.bashrc
PID=`ps -eaf | grep '<PRODUCT_HOME>/restapi/ivrServer/IVR-1.0-SNAPSHOT.jar' | grep -v grep | awk '{print $2}'`

if [[ "" ==  "$PID" ]]; then
   echo "$now IVR Server is not running, Starting the API"
   java -jar <PRODUCT_HOME>/restapi/ivrServer/IVR-1.0-SNAPSHOT.jar --spring.config.location=<PRODUCT_HOME>/restapi/ivrServer/config/application.properties &
fi

