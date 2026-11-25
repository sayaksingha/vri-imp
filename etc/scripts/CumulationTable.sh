#  @(#) $Id: CumulationTable.sh $"
#-----------------------------------------------------------------------
# NAME       : CumulationTable.sh
#
# COPYRIGHT
# Tayana Software Solutions Ltd -- Copyright(C) 2013
# All rights reserved. No part of this computer program
# may be used or reproduced in any form by any
# means without prior written permission of
# Tayana Software Solutions Ltd
#
# DESCRIPTION
#
# Installed :                                              <DATE_VALUE>
#-----------------------------------------------------------------------
#!/bin/sh

#####################################################
##################### Variables #####################
#####################################################
#CUMULATION_PROCEDURE_FILE="/tmp/Cumulation_Oracle_Proc.sql"
#CUMULATION_JOB_FILE="/tmp/Cumulation_Oracle_Job.sh"
#source $PRODUCT_HOME/etc/dbscripts/Postgres/CumulationTable.env;
#*****************************************************************************
export SETCOLOR_SUCCESS="echo -en \\033[1;32m"
export SETCOLOR_CRITICAL="echo -en \\033[1;31m"
export SETCOLOR_NORMAL="echo -en \\033[0;39m"
export SETCOLOR_START="echo -en \\033[0;45m"
export SETCOLOR_MAGENTA="echo -en \\033[0;35m"

if [ $# != "7" ]; then
 echo "Mandatory Arguments are missing, Ex sh CumulationTable.sh <TABLE_ID> <DB_TYPE 1-Oracle, 2-MYSQL, 3-OMNISCI, 4-Vertica> <DB_USERNAME> <DB_PASSWORD> <DB_HOST> <DB_NAME> <DB_SCHEMA>"
 exit 0;
fi

TABLE_ID=$1
DB_TYPE=$2
USERNAME_TSSGUI=$3
PASSWORD_TSSGUI=$4
HOST_TSSGUI=$5
SID_TSSGUI=$6
SCHEMA_TSSGUI=$7


#####################################################
##################### Functions #####################
#####################################################

#------------------------- Read Cumulation Table Details ----------------------#
read_table_details () {
CUMULATIVE_TABLE_ARR=$(PGOPTIONS="--search_path=$SCHEMA_TSSGUI" PGPASSWORD=$PASSWORD_TSSGUI psql -h $HOST_TSSGUI -U $USERNAME_TSSGUI -d $SID_TSSGUI -t -A -c "
SELECT
  COALESCE(CUMULATION_TABLE_NAME, '') || '|' ||
  COALESCE(EVR_TABLE_NAME, '') || '|' ||
  COALESCE(CUMULATION_COLUMNS, '') || '|' ||
  COALESCE(EVR_TABLE_TYPE, '') || '|' ||
  COALESCE(DB_USER, '') || '|' ||
  COALESCE(DB_PSWD, '') || '|' ||
  COALESCE(DB_IP, '') || '|' ||
  COALESCE(DB_NAME, '') || '|' ||
  COALESCE(DB_SRV, '') || '|' ||
  COALESCE(SELECT_QUERY, '')
FROM CUMULATION_TABLE
WHERE TABLE_ID = $TABLE_ID;

")

if [[ $CUMULATIVE_TABLE_ARR == ERROR* ]]; then
 $SETCOLOR_CRITICAL
 echo "$CUMULATIVE_TABLE_ARR" > $CUMULATION_LOG_PATH/"Cumulation_${TABLE_ID}_Exceptions.txt"
 $SETCOLOR_NORMAL
else

CUMULATIVE_TABLE=`echo $CUMULATIVE_TABLE_ARR | cut -f 1 -d '|'`
EVR_TABLE=`echo $CUMULATIVE_TABLE_ARR | cut -f 2 -d '|'`
CUMULATIVE_COLUMN_NAME=`echo $CUMULATIVE_TABLE_ARR | cut -f 3 -d '|'`
EVR_TABLE_TYPE=`echo $CUMULATIVE_TABLE_ARR | cut -f 4 -d '|'`
DB_USER=`echo $CUMULATIVE_TABLE_ARR | cut -f 5 -d '|'`
DB_PSWD=`echo $CUMULATIVE_TABLE_ARR | cut -f 6 -d '|'`
DB_IP=`echo $CUMULATIVE_TABLE_ARR | cut -f 7 -d '|'`
DB_NAME=`echo $CUMULATIVE_TABLE_ARR | cut -f 8 -d '|'`

if [ "$DB_TYPE" = "3" ]; then
   DB_FILE_PATH=`echo $CUMULATIVE_TABLE_ARR | cut -f 9 -d '|'`
else
   DB_SID=`echo $CUMULATIVE_TABLE_ARR | cut -f 9 -d '|'`
fi

QUERY=`echo $CUMULATIVE_TABLE_ARR | cut -f 10 -d '|'`

if [[ $QUERY == *"<:startDate>"*"<:endDate>"* ]]; then
   TIME_ARR=$(PGOPTIONS="--search_path=$SCHEMA_TSSGUI" PGPASSWORD=$PASSWORD_TSSGUI psql -h $HOST_TSSGUI -U $USERNAME_TSSGUI -d $SID_TSSGUI -t -A -c "
   SELECT TO_CHAR(TIME, 'YYYY-MM-DD HH24:MI:SS') FROM CUMULATIVE_EXEC_TIME WHERE TABLE_ID = $TABLE_ID;")
   START_TIME=`echo $TIME_ARR | cut -f 1 -d '|'`
fi

EVR_TABLE+="_"
if [ "$EVR_TABLE_TYPE" = "D" ]; then
  EVR_TABLE+=$(date +"%d%m")
elif [ "$EVR_TABLE_TYPE" = "H" ]; then
  EVR_TABLE+=$(date +"%d%m%y")
elif [ "$EVR_TABLE_TYPE" = "M" ]; then
  EVR_TABLE+=$(date +"%m%Y")
elif [ "$EVR_TABLE_TYPE" = "Y" ]; then
  EVR_TABLE+=$(date +"%Y")
fi
fi
}

#------------------------- Update Exec Time In TSSGUI Db
updateTime () {
  echo "`date "+%d-%b-%y %H:%M:%S"` Executing -----------------> UPDATE CUMULATIVE_EXEC_TIME SET TIME = TO_TIMESTAMP('$1','YYYY-MM-DD HH24:MI:SS') WHERE TABLE_ID = $TABLE_ID;" >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log

  PGOPTIONS="--search_path=$SCHEMA_TSSGUI" PGPASSWORD=$PASSWORD_TSSGUI psql -h $HOST_TSSGUI -U $USERNAME_TSSGUI -d $SID_TSSGUI -c "
  UPDATE CUMULATIVE_EXEC_TIME
  SET TIME = TO_TIMESTAMP('$1','YYYY-MM-DD HH24:MI:SS')
  WHERE TABLE_ID = $TABLE_ID;
  " >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log 2>&1
}

oracle_db () {
   #*****************************************************************************
   # Read Cumulative Table Details
   read_table_details
   if [ -s $CUMULATION_LOG_PATH/"Cumulation_${TABLE_ID}_Exceptions.txt" ]
   then
      $SETCOLOR_CRITICAL
      cat $CUMULATION_LOG_PATH/"Cumulation_${TABLE_ID}_Exceptions.txt" >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
      $SETCOLOR_NORMAL
      exit;
   else
      DATE=`date "+%Y-%m-%d %H:%M:%S"`
      QUERY=$(echo $QUERY | sed -e "s/<:startDate>/'$START_TIME'/g" -e "s/<:endDate>/'${DATE}'/g" -e "s/<:EVR_TABLE>/${EVR_TABLE}/g")

if [ "$DB_SID" = "SID" ]; then
  ORACLE_CONN_STR="$DB_USER/$DB_PSWD@(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)(HOST=$DB_IP)(PORT=1521))(CONNECT_DATA=(SID=$DB_NAME)))"
else
  ORACLE_CONN_STR="$DB_USER/$DB_PSWD@(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)(HOST=$DB_IP)(PORT=1521))(CONNECT_DATA=(SERVICE_NAME=$DB_NAME)))"
fi

TABLE_EXISTS=$(sqlplus -s "$ORACLE_CONN_STR" <<EOF
whenever sqlerror exit sql.sqlcode
SET LINESIZE 32767 PAGESIZE 0 FEEDBACK OFF VERIFY OFF HEADING OFF ECHO OFF
DESC $CUMULATIVE_TABLE;
EXIT;
EOF
)


      if [[ $TABLE_EXISTS == *"ERROR"* ]]; then
         echo "`date "+%d-%b-%y %H:%M:%S"` $CUMULATIVE_TABLE Table does not exists " >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
         echo "`date "+%d-%b-%y %H:%M:%S"` Executing -----------------> CREATE TABLE $CUMULATIVE_TABLE AS ($QUERY);" >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log

TABLE_CREATE=$(sqlplus -s "$ORACLE_CONN_STR" <<EOF
whenever sqlerror exit sql.sqlcode
SET LINESIZE 32767 PAGESIZE 0 FEEDBACK OFF VERIFY OFF HEADING OFF ECHO OFF
CREATE TABLE $CUMULATIVE_TABLE AS ($QUERY);
EXIT;
EOF
)

         if [[ $TABLE_CREATE == *"ERROR"* ]]; then
            echo "`date "+%d-%b-%y %H:%M:%S"` Error in Table Creation ---> $TABLE_CREATE --> Exiting Script" >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
            exit;
         fi
      else
         echo "`date "+%d-%b-%y %H:%M:%S"` $CUMULATIVE_TABLE Table exists " >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
# Get HEADER from Oracle
# Run SQL and get CSV output


HEADER=$(sqlplus -s "$ORACLE_CONN_STR" <<EOF
SET MARKUP CSV ON DELIMITER ',' QUOTE OFF
SET FEEDBACK OFF
SET PAGESIZE 0
$QUERY;
EOF
)

# Extract first non-empty line (the header), remove carriage returns
HEADER=$(echo "$HEADER" | tr -d '\r' | sed '/^\s*$/d')

if [ -z  "$HEADER" ]; then
  echo "`date "+%d-%b-%y %H:%M:%S"` No data available in EVR table for $QUERY. Exiting."  >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
  exit 1
fi

# Parse header CSV into array
IFS=',' read -r -a COLS <<< "$HEADER"

# Your aggregate columns string, comma separated
# Example: "TOTAL_COUNT,SOME_OTHER_AGG"
IFS=',' read -r -a AGG_COLS <<< "$CUMULATIVE_COLUMN_NAME"

# Prepare arrays for aggregate set clauses and key columns
AGG_SET=()
KEY_COLS=()
AGG_CNT=0

# Separate keys and aggregate columns
for col in "${COLS[@]}"; do
  IS_AGG=0
  for agg in "${AGG_COLS[@]}"; do
    if [[ "$col" == "$agg" ]]; then
      IS_AGG=1
       ((AGG_CNT++))
      break
    fi
  done

  if [ "$IS_AGG" -eq 1 ]; then
    AGG_SET+=("t.${col} = t.${col} + s.${col}")
  else
    KEY_COLS+=("$col")
  fi
done

if [ ${#AGG_COLS[@]} -ne $AGG_CNT ]; then
  echo "`date "+%d-%b-%y %H:%M:%S"` Error: cumulative column not found in query output columns"
  exit 1
fi


# Build ON clause from key columns
ON_CLAUSE=""
for col in "${KEY_COLS[@]}"; do
  ON_CLAUSE+="t.${col} = s.${col} AND "
done
ON_CLAUSE=${ON_CLAUSE%" AND "}


	 UPDATE_CLAUSE=$(IFS=, ; echo "${AGG_SET[*]}")
  	 INSERT_COLS=$(IFS=, ; echo "${COLS[*]}")
	 INSERT_VALUES=$(printf "s.%s," "${COLS[@]}")
	 INSERT_VALUES=${INSERT_VALUES%,}

	 STAGING_TABLE="${CUMULATIVE_TABLE}_STAGING"
	 # Oracle MERGE using staging
	 sqlplus -s "$ORACLE_CONN_STR" <<EOF >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log 2>&1
		WHENEVER SQLERROR EXIT SQL.SQLCODE
		SET FEEDBACK OFF HEADING OFF ECHO OFF
		DECLARE
		  dummy NUMBER;
		BEGIN
		  EXECUTE IMMEDIATE 'DROP TABLE $STAGING_TABLE PURGE';
		EXCEPTION
		  WHEN OTHERS THEN NULL;
		END;
		/

		CREATE GLOBAL TEMPORARY TABLE $STAGING_TABLE ON COMMIT PRESERVE ROWS AS SELECT * FROM (${QUERY}) WHERE 1=0;
		TRUNCATE TABLE $STAGING_TABLE;
		INSERT INTO $STAGING_TABLE ${QUERY};

		MERGE INTO $CUMULATIVE_TABLE t	
		USING $STAGING_TABLE s
		ON (${ON_CLAUSE})
		WHEN MATCHED THEN UPDATE SET ${UPDATE_CLAUSE}
		WHEN NOT MATCHED THEN INSERT (${INSERT_COLS}) VALUES (${INSERT_VALUES});	

		COMMIT;
EOF
      fi

      if [ $? -eq 0 ]; then
         updateTime "$DATE"
         exit;
      else
         $SETCOLOR_CRITICAL_Exceptions.txt
         echo "`date "+%d-%b-%y %H:%M:%S"` Error: Oracle DB operation failed. Skipping updateTime." >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
         $SETCOLOR_NORMAL
      fi
   fi
}

omnisci_db() {
  read_table_details

  DATE=$(date "+%Y-%m-%d %H:%M:%S")
  QUERY=$(echo "$QUERY" | sed -e "s/<:startDate>/'$START_TIME'/g" -e "s/<:endDate>/'$DATE'/g" -e "s/<:EVR_TABLE>/${EVR_TABLE}/g")
  STAGING_TABLE="${CUMULATIVE_TABLE}_STAGING"

  # Extract header from SELECT part of query
  HEADER=$(echo "$QUERY" | sed -E 's/^SELECT (.+) FROM .+$/\1/I' | sed 's/ AS / as /Ig' | awk 'BEGIN{RS=",";ORS=","} {print $NF}' | sed 's/,$//')


# Parse header CSV into array
IFS=',' read -r -a COLS <<< "$HEADER"

# Your aggregate columns string, comma separated
# Example: "TOTAL_COUNT,SOME_OTHER_AGG"
IFS=',' read -r -a AGG_COLS <<< "$CUMULATIVE_COLUMN_NAME"

# Prepare arrays for aggregate set clauses and key columns
AGG_SET=()
KEY_COLS=()
AGG_CNT=0

# Separate keys and aggregate columns
for col in "${COLS[@]}"; do
  IS_AGG=0
  for agg in "${AGG_COLS[@]}"; do
    if [[ "$col" == "$agg" ]]; then
      IS_AGG=1
       ((AGG_CNT++))
      break
    fi
  done

  if [ "$IS_AGG" -eq 1 ]; then
    AGG_SET+=("t.${col} = t.${col} + s.${col}")
  else
    KEY_COLS+=("$col")
  fi
done

if [ ${#AGG_COLS[@]} -ne $AGG_CNT ]; then
  echo "`date "+%d-%b-%y %H:%M:%S"` Error: cumulative column not found in query output columns"
  exit 1
fi


# Build ON clause from key columns
ON_CLAUSE=""
for col in "${KEY_COLS[@]}"; do
  ON_CLAUSE+="t.${col} = s.${col} AND "
done
ON_CLAUSE=${ON_CLAUSE%" AND "}


         UPDATE_CLAUSE=$(IFS=, ; echo "${AGG_SET[*]}")
         INSERT_COLS=$(IFS=, ; echo "${COLS[*]}")
         INSERT_VALUES=$(printf "s.%s," "${COLS[@]}")
         INSERT_VALUES=${INSERT_VALUES%,}

  # Run everything in a single DB call
  $DB_FILE_PATH $DB_NAME -u $DB_USER -p $DB_PSWD -s $DB_IP <<EOF >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log 2>&1
DROP TABLE IF EXISTS $STAGING_TABLE;
CREATE TABLE $STAGING_TABLE AS $QUERY;

-- Try to create the cumulative table if it doesn't exist
CREATE TABLE IF NOT EXISTS $CUMULATIVE_TABLE AS SELECT * FROM $STAGING_TABLE;

-- Try update; if table exists, this will work
UPDATE $CUMULATIVE_TABLE t
SET $UPDATE_CLAUSE
FROM $STAGING_TABLE s
WHERE $ON_CLAUSE;

-- Insert new rows
INSERT INTO $CUMULATIVE_TABLE ($INSERT_COLS)
SELECT $INSERT_VALUES FROM $STAGING_TABLE s
LEFT JOIN $CUMULATIVE_TABLE t ON $ON_CLAUSE
WHERE t.${COLS[0]} IS NULL;
EOF

  if [ $? -eq 0 ]; then
    updateTime "$DATE"
  else
    $SETCOLOR_CRITICAL
    echo "`date '+%d-%b-%y %H:%M:%S'` Error executing Omnisci operation for $CUMULATIVE_TABLE" >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
    $SETCOLOR_NORMAL
    exit 1
  fi
}


mysql_db () {
  #*****************************************************************************
  # Read Cumulative Table Details
  read_table_details

  if [ -s "$CUMULATION_LOG_PATH/Cumulation_${TABLE_ID}_Exceptions.txt" ]; then
    $SETCOLOR_CRITICAL
    cat "$CUMULATION_LOG_PATH/Cumulation_${TABLE_ID}_Exceptions.txt" >> "$CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log"
    $SETCOLOR_NORMAL
    exit
  else
    DATE=$(date "+%Y-%m-%d %H:%M:%S")

    QUERY=$(echo "$QUERY" | sed -e "s/<:startDate>/'$START_TIME'/g" -e "s/<:endDate>/'${DATE}'/g" -e "s/<:EVR_TABLE>/${EVR_TABLE}/g")

    STAGING_TABLE="${CUMULATIVE_TABLE}_STAGING"

    # Check if main table exists
    echo "SHOW TABLES LIKE '$CUMULATIVE_TABLE';" > "$CUMULATION_LOG_PATH/Cumulation_${TABLE_ID}.txt"
    TABLE_EXISTS=$(mysql -h "$DB_IP" -u "$DB_USER" -p"$DB_PSWD" "$DB_NAME" < "$CUMULATION_LOG_PATH/Cumulation_${TABLE_ID}.txt" 2>/dev/null)

    if [ -z "$TABLE_EXISTS" ]; then
      echo "Table is absent"
      echo "`date "+%d-%b-%y %H:%M:%S"` Creating $CUMULATIVE_TABLE from query..." >> "$CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log"

      mysql -h "$DB_IP" -u "$DB_USER" -p"$DB_PSWD" "$DB_NAME" <<EOF
CREATE TABLE $CUMULATIVE_TABLE AS $QUERY;
EOF

    else
      echo "`date "+%d-%b-%y %H:%M:%S"` $CUMULATIVE_TABLE Table is present"  >> "$CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log"

      # Get column names
      HEADER=$(mysql -h "$DB_IP" -u "$DB_USER" -p"$DB_PSWD" -D "$DB_NAME" --batch -e "$QUERY"   | head -n 1 | sed 's/\t/,/g' | tr -d '\r')

if [ -z  "$HEADER" ]; then
  echo "`date "+%d-%b-%y %H:%M:%S"` No data available in EVR table for $QUERY. Exiting."  >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
  exit 1
fi


      IFS=',' read -r -a COLS <<< "$HEADER"

IFS=',' read -r -a AGG_COLS <<< "$CUMULATIVE_COLUMN_NAME"

# Prepare arrays for aggregate set clauses and key columns
AGG_SET=()
KEY_COLS=()
AGG_CNT=0

# Separate keys and aggregate columns
for col in "${COLS[@]}"; do
  IS_AGG=0
  for agg in "${AGG_COLS[@]}"; do
    if [[ "$col" == "$agg" ]]; then
      IS_AGG=1
       ((AGG_CNT++))
      break
    fi
  done

  if [ "$IS_AGG" -eq 1 ]; then
    AGG_SET+=("t.${col} = t.${col} + s.${col}")
  else
    KEY_COLS+=("$col")
  fi
done

if [ ${#AGG_COLS[@]} -ne $AGG_CNT ]; then
  echo "`date "+%d-%b-%y %H:%M:%S"` Error: cumulative column not found in query output columns"
  exit 1
fi

# Build ON clause from key columns
ON_CLAUSE=""
for col in "${KEY_COLS[@]}"; do
  ON_CLAUSE+="t.${col} = s.${col} AND "
done
ON_CLAUSE=${ON_CLAUSE%" AND "}


         UPDATE_CLAUSE=$(IFS=, ; echo "${AGG_SET[*]}")
         INSERT_COLS=$(IFS=, ; echo "${COLS[*]}")
         INSERT_VALUES=$(printf "s.%s," "${COLS[@]}")
         INSERT_VALUES=${INSERT_VALUES%,}

      # Perform the UPDATE
      mysql -h "$DB_IP" -u "$DB_USER" -p"$DB_PSWD" "$DB_NAME" <<EOF
DROP TEMPORARY TABLE IF EXISTS $STAGING_TABLE;
CREATE TEMPORARY TABLE $STAGING_TABLE AS $QUERY;

UPDATE $CUMULATIVE_TABLE t
JOIN $STAGING_TABLE s
ON $ON_CLAUSE
SET $UPDATE_CLAUSE;

INSERT INTO $CUMULATIVE_TABLE (${INSERT_COLS})
SELECT ${INSERT_VALUES}
FROM $STAGING_TABLE s
LEFT JOIN $CUMULATIVE_TABLE t
ON $ON_CLAUSE
WHERE t.\`${COLS[0]}\` IS NULL;
EOF

    fi

    # Log errors if any
if [ $? -eq 0 ]; then
  updateTime "$DATE"
  exit
else
  $SETCOLOR_CRITICAL
  echo "`date "+%d-%b-%y %H:%M:%S"` Error: MySQL DB operation failed. Skipping updateTime." >> "$CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log"
  $SETCOLOR_NORMAL
fi
  fi
}

vertica_db() {
  read_table_details   # sets QUERY and CUMULATIVE_TABLE dynamically
   if [ -s $CUMULATION_LOG_PATH/"Cumulation_${TABLE_ID}_Exceptions.txt" ]
   then
      $SETCOLOR_CRITICAL
      cat $CUMULATION_LOG_PATH/"Cumulation_${TABLE_ID}_Exceptions.txt" >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
      $SETCOLOR_NORMAL
      exit;
   else
      DATE=`date "+%Y-%m-%d %H:%M:%S"`
      QUERY=$(echo $QUERY | sed -e "s/<:startDate>/'$START_TIME'/g" -e "s/<:endDate>/'${DATE}'/g" -e "s/<:EVR_TABLE>/${EVR_TABLE}/g")

  STAGING_TABLE="${CUMULATIVE_TABLE}_STAGING"

  TABLE_EXISTS=$($vsql -h $DB_IP -d $DB_NAME -U $DB_USER -w $DB_PSWD -Atc "
	SELECT table_name
	FROM v_catalog.tables
	WHERE table_schema = '$DB_SID' AND table_name = '$CUMULATIVE_TABLE';
	")

if [[ -z "$TABLE_EXISTS" ]]; then
  echo "$(date "+%d-%b-%y %H:%M:%S") $CUMULATIVE_TABLE Table does not exist" >> "$CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log"
  echo "$(date "+%d-%b-%y %H:%M:%S") Executing -----------------> CREATE TABLE $DB_SID.$CUMULATIVE_TABLE AS ($QUERY);" >> "$CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log"

  TABLE_CREATE=$($vsql -h $DB_IP -d $DB_NAME -U $DB_USER -w $DB_PSWD -Atc "
  CREATE TABLE $DB_SID.$CUMULATIVE_TABLE AS $QUERY;
  ")

  if [[ $? -ne 0 ]]; then
    echo "$(date "+%d-%b-%y %H:%M:%S") Error in Table Creation ---> $TABLE_CREATE --> Exiting Script" >> "$CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log"
    exit 1
  fi
else
  echo "`date "+%d-%b-%y %H:%M:%S"` $CUMULATIVE_TABLE Table exists " >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log

  # Get column names dynamically (comma-separated)
  HEADER=$($vsql -h "$DB_IP" -U "$DB_USER" -w "$DB_PSWD" -d "$DB_NAME" \
  -A -F $'\t' -P footer=off -c "$QUERY" | head -n 1 | sed 's/\t/,/g' | tr -d '\r')

  if [ -z "$HEADER" ]; then
        echo "$(date "+%d-%b-%y %H:%M:%S") No data available in EVR table for $QUERY. Exiting." >> "$CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log"
        exit 1
  fi

  # Parse header into array of columns
  IFS=',' read -r -a COLS <<< "$HEADER"

  # Decide keys and aggregates: 
  # For generic, assume *all columns except last one* are keys, and last is aggregate (adjust if needed)
  # Or better, detect numeric columns as aggregates - here simplified as last column aggregate
  IFS=',' read -r -a AGG_COLS <<< "$CUMULATIVE_COLUMN_NAME"

AGG_SET=()
KEY_COLS=()
AGG_CNT=0

# Separate keys and aggregate columns
for col in "${COLS[@]}"; do
  IS_AGG=0
  for agg in "${AGG_COLS[@]}"; do
    if [[ "$col" == "$agg" ]]; then
      IS_AGG=1
      ((AGG_CNT++))
      break
    fi
  done

  if [ "$IS_AGG" -eq 1 ]; then
    AGG_SET+=("${col} = t.${col} + s.${col}")
  else
    KEY_COLS+=("$col")
  fi
done

if [ ${#AGG_COLS[@]} -ne $AGG_CNT ]; then
  echo "`date "+%d-%b-%y %H:%M:%S"` Error: cumulative column not found in query output columns"
  exit 1
fi

  # Build ON condition dynamically
  ON_CLAUSE=""
  for col in "${KEY_COLS[@]}"; do
    ON_CLAUSE+="t.${col} = s.${col} AND "
  done
  ON_CLAUSE=${ON_CLAUSE%" AND "}

  # Build update clause - aggregate column updated by addition
  #UPDATE_CLAUSE="${AGG_COL} = t.${AGG_COL} + s.${AGG_COL}"

  # Build insert column list and values from all columns
  #INSERT_COLS=$(IFS=, ; echo "${COLS[*]}")
  #INSERT_VALUES=$(IFS=, ; echo "s.${COLS[*]}")


         UPDATE_CLAUSE=$(IFS=, ; echo "${AGG_SET[*]}")
         INSERT_COLS=$(IFS=, ; echo "${COLS[*]}")
         INSERT_VALUES=$(printf "s.%s," "${COLS[@]}")
         INSERT_VALUES=${INSERT_VALUES%,}

echo "CREATE LOCAL TEMP TABLE $STAGING_TABLE ON COMMIT PRESERVE ROWS AS SELECT * FROM (${QUERY}) AS sub WHERE false;INSERT INTO $STAGING_TABLE ${QUERY};"
	 echo "UPDATE ${DB_SID}.${CUMULATIVE_TABLE} SET ${UPDATE_CLAUSE} FROM $STAGING_TABLE  WHERE ${ON_CLAUSE};"
  export VSQL_SEARCH_PATH=$DB_SID

  # Create or truncate staging table
  $vsql -h "$DB_IP" -d "$DB_NAME" -U "$DB_USER" -w "$DB_PSWD" <<EOF >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log 2>&1
\set AUTOCOMMIT on
CREATE LOCAL TEMP TABLE $STAGING_TABLE ON COMMIT PRESERVE ROWS AS SELECT * FROM (${QUERY}) AS sub WHERE false;
TRUNCATE TABLE $STAGING_TABLE;
INSERT INTO $STAGING_TABLE ${QUERY};

UPDATE ${DB_SID}.${CUMULATIVE_TABLE} t
SET ${UPDATE_CLAUSE}
FROM $STAGING_TABLE s
WHERE ${ON_CLAUSE};

INSERT INTO ${DB_SID}.${CUMULATIVE_TABLE} (${INSERT_COLS})
SELECT ${INSERT_VALUES} FROM $STAGING_TABLE s
WHERE NOT EXISTS (
  SELECT 1 FROM ${DB_SID}.${CUMULATIVE_TABLE} t
  WHERE ${ON_CLAUSE}
);
EOF
fi

      if [ $? -eq 0 ]; then
         updateTime "$DATE"
         exit;
      else
         $SETCOLOR_CRITICAL_Exceptions.txt
         echo "`date "+%d-%b-%y %H:%M:%S"` Error: Vertica DB operation failed. Skipping updateTime." >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
         $SETCOLOR_NORMAL
      fi
   fi
}

#*****************************************************************************
# Check whether we have sufficient privileges
if [ $(id -u) -eq 0 ]; then
echo "==================================================="
$SETCOLOR_CRITICAL
echo "ERROR: This script needs to be run as product user."
$SETCOLOR_NORMAL
echo
exit 1
fi

#*****************************************************************************
# Check whether PRODUCT_HOME env is set
if [ ! $PRODUCT_HOME ]; then
echo "==================================================="
$SETCOLOR_CRITICAL
echo "ERROR: PRODUCT_HOME env not set."
$SETCOLOR_NORMAL
echo
exit 1
fi

#*****************************************************************************
pushd $PRODUCT_HOME >/dev/null
#*****************************************************************************


#****************************** Functions ************************************
if [ "$DB_TYPE" = "1" ]; then
$SETCOLOR_SUCCESS
echo "`date "+%d-%b-%y %H:%M:%S"` Table Id : $TABLE_ID, DB Type : Oracle" >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
$SETCOLOR_NORMAL
oracle_db
break
elif [ "$DB_TYPE" = "2" ]; then
$SETCOLOR_SUCCESS
echo "`date "+%d-%b-%y %H:%M:%S"` Table Id : $TABLE_ID, DB Type : MySQL" >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
$SETCOLOR_NORMAL
mysql_db
break
elif [ "$DB_TYPE" = "3" ]; then
$SETCOLOR_SUCCESS
echo "`date "+%d-%b-%y %H:%M:%S"` Table Id : $TABLE_ID, DB Type : OMNISCI" >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
$SETCOLOR_NORMAL
omnisci_db
break
elif [ "$DB_TYPE" = "4" ]; then
$SETCOLOR_SUCCESS
echo "`date "+%d-%b-%y %H:%M:%S"` Table Id : $TABLE_ID, DB Type : Vertica" >> $CUMULATION_LOG_PATH/CumulationScript_$(date +%Y%m%d).log
$SETCOLOR_NORMAL
vertica_db
break
else
$SETCOLOR_CRITICAL
echo "Invalid Database Type.[Arg 1 : 1-Oracle,2-Mysql,3-Omnisci,4-Vertica] "
$SETCOLOR_NORMAL
fi

if [ -f "$CUMULATION_LOG_PATH/Cumulation_${TABLE_ID}.txt" ]; then
   rm $CUMULATION_LOG_PATH/"Cumulation_${TABLE_ID}.txt"
fi
if [ -f "$CUMULATION_LOG_PATH/Cumulation_${TABLE_ID}_Exceptions.txt" ]; then
   rm $CUMULATION_LOG_PATH/"Cumulation_${TABLE_ID}_Exceptions.txt"
fi

#sh $CUMULATION_JOB_FILE

#####################################################
#--------------- END OF SCRIPT ---------------------#
#####################################################

