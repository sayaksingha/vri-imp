#!/bin/bash

source /opt/ivr/etc/scripts/ivr.env

# Connection details
VT_HOST=$(echo "$MIS_IVR_DB_NAME" | cut -d'@' -f1)
VT_DBNAME=$(echo "$MIS_IVR_DB_NAME" | cut -d'@' -f2)

# File paths
IVR_MIS_FILE_PATH=/opt/ivr/var/EVRRecord/processing/
IVR_MIS_BACKUP_PATH=/opt/ivr/var/EVRRecord/processed/
IVR_MIS_FILE_LOG_PATH=/opt/ivr/var/log

# Dates
DDMM=$(date +%d%m)
YESTERDAY_DDMM=$(date -d "yesterday" +%d%m)

# Schemas from env
IVR_SCHEMA=$MIS_IVR_DB_Schema
OBD_SCHEMA=$MIS_OBD_DB_Schema

# Source tables
SRC_TABLE_IVR="${IVR_SCHEMA}.evr_details_${DDMM}"
SRC_TABLE_OBD="${OBD_SCHEMA}.evr_details_${DDMM}"

# Destination tables
VT_TABLE="${IVR_SCHEMA}.ivr_evr_details_${DDMM}"
OBD_TABLE="${OBD_SCHEMA}.obd_evr_details_${DDMM}"

# 1️⃣ Drop yesterday’s staging tables
for SCHEMA in $IVR_SCHEMA $OBD_SCHEMA; do
/opt/vertica/bin/vsql -h "$VT_HOST" -d "$VT_DBNAME" \
  -U "$MIS_IVR_DB_USER" -w "$MIS_IVR_DB_PASS" <<EOF
DROP TABLE IF EXISTS ${SCHEMA}.evr_details_${YESTERDAY_DDMM} CASCADE;
EOF
done

# 2️⃣ Ensure final tables exist
/opt/vertica/bin/vsql -h "$VT_HOST" -d "$VT_DBNAME" \
  -U "$MIS_IVR_DB_USER" -w "$MIS_IVR_DB_PASS" <<EOF
CREATE TABLE IF NOT EXISTS $VT_TABLE (
    evr_event numeric(6),
    evr_cause numeric(6),
    evr_reason numeric(6),
    ref_id numeric(11),
    short_code varchar(20),
    request_mode numeric(11),
    call_ref_num varchar(20) NOT NULL,
    cld_num varchar(20),
    clg_num varchar(20),
    content_id numeric(11),
    content varchar(100),
    duration numeric(11),
    charge_unit numeric(6),
    charge numeric(11),
    gen_time timestamp,
    schedule_id numeric(11),
    camp_id numeric(11)
);

CREATE TABLE IF NOT EXISTS $OBD_TABLE (
    evr_event numeric(6),
    evr_cause numeric(6),
    evr_reason numeric(6),
    ref_id numeric(11),
    short_code varchar(20),
    request_mode numeric(11),
    call_ref_num varchar(20) NOT NULL,
    cld_num varchar(20),
    clg_num varchar(20),
    content_id numeric(11),
    content varchar(100),
    duration numeric(11),
    charge_unit numeric(6),
    charge numeric(11),
    gen_time timestamp,
    schedule_id numeric(11),
    camp_id numeric(11)
);
EOF

# 3️⃣ Load files into both staging tables
cd "$IVR_MIS_FILE_PATH" || exit 1
list=$(find . -name "IVR_EVR*" -mmin +0)

if [ -z "$list" ]; then
    echo "No files to load"
    exit 0
fi

for i in $list; do
    j=$(echo "$i" | cut -c 3-)
    FULL_PATH="${IVR_MIS_FILE_PATH}${j}"

    for SRC_TABLE in $SRC_TABLE_IVR $SRC_TABLE_OBD; do
        /opt/vertica/bin/vsql -h "$VT_HOST" -d "$VT_DBNAME" \
          -U "$MIS_IVR_DB_USER" -w "$MIS_IVR_DB_PASS" <<EOF
CREATE TABLE IF NOT EXISTS $SRC_TABLE (
    evr_event numeric(6),
    evr_cause numeric(6),
    evr_reason numeric(6),
    ref_id numeric(11),
    short_code varchar(20),
    request_mode numeric(11),
    call_ref_num varchar(20) NOT NULL,
    cld_num varchar(20),
    clg_num varchar(20),
    content_id numeric(11),
    content varchar(100),
    duration numeric(11),
    charge_unit numeric(6),
    charge numeric(11),
    gen_time timestamp,
    schedule_id numeric(11),
    camp_id numeric(11)
);
EOF

        /opt/vertica/bin/vsql -h "$VT_HOST" -d "$VT_DBNAME" \
          -U "$MIS_IVR_DB_USER" -w "$MIS_IVR_DB_PASS" -c \
          "COPY $SRC_TABLE FROM LOCAL '$FULL_PATH' DELIMITER ',' NULL '' DIRECT;"
    done

    if [ $? -eq 0 ]; then
        mv -f "$FULL_PATH" "$IVR_MIS_BACKUP_PATH"
        echo "Moved $j to backup."
    else
        echo "Load failed for $j"
    fi
done

# 4️⃣ Split into IVR / OBD
/opt/vertica/bin/vsql -h "$VT_HOST" -d "$VT_DBNAME" \
  -U "$MIS_IVR_DB_USER" -w "$MIS_IVR_DB_PASS" <<EOF
INSERT INTO $VT_TABLE
SELECT * FROM $SRC_TABLE_IVR WHERE evr_event BETWEEN 1 AND 13;

INSERT INTO $OBD_TABLE
SELECT * FROM $SRC_TABLE_OBD WHERE evr_event BETWEEN 51 AND 55;

COMMIT;
EOF

# 5️⃣ Cleanup staging (delete processed rows from both)
for SRC_TABLE in $SRC_TABLE_IVR $SRC_TABLE_OBD; do
LAST_GEN_TIME=$(/opt/vertica/bin/vsql -h "$VT_HOST" -d "$VT_DBNAME" \
  -U "$MIS_IVR_DB_USER" -w "$MIS_IVR_DB_PASS" -tAc \
  "SELECT MAX(gen_time) FROM $SRC_TABLE;")

if [ -n "$LAST_GEN_TIME" ]; then
  /opt/vertica/bin/vsql -h "$VT_HOST" -d "$VT_DBNAME" \
    -U "$MIS_IVR_DB_USER" -w "$MIS_IVR_DB_PASS" <<EOF
DELETE FROM $SRC_TABLE WHERE gen_time <= TIMESTAMP '$LAST_GEN_TIME';
COMMIT;
EOF
fi
done

# Final check
/opt/vertica/bin/vsql -h "$VT_HOST" -d "$VT_DBNAME" \
  -U "$MIS_IVR_DB_USER" -w "$MIS_IVR_DB_PASS" -c \
  "SELECT 'IVR Count=' || COUNT(*) FROM $VT_TABLE;
   SELECT 'OBD Count=' || COUNT(*) FROM $OBD_TABLE;"

