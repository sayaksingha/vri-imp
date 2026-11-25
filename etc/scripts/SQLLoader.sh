#!/bin/bash

# Load environment variables
source /opt/ivr/etc/scripts/ivr.env

# Extract host and DB from MIS_DB_NAME
VT_HOST=$(echo "$MIS_DB_NAME" | cut -d'@' -f1)
VT_DBNAME=$(echo "$MIS_DB_NAME" | cut -d'@' -f2)

# Get today's date in DDMM format
DDMM=$(date +%d%m)

# Construct dynamic table names
SRC_TABLE="evr_details_${DDMM}"
VT_TABLE="public.ivr_evr_details_${DDMM}"
OBD_TABLE="public.obd_evr_details_${DDMM}"

echo "Starting data transfer for date: $DDMM"
echo "Source Table: $SRC_TABLE"
echo "Target Table (Valid Data): $VT_TABLE"
echo "Target Table (Invalid Data): $OBD_TABLE"

# Check if source table exists
SRC_EXISTS=$(vsql -h "$VT_HOST" -d "$VT_DBNAME" -U "$MIS_DB_USER" -w "$MIS_DB_PASS" -tAc \
  "SELECT table_name FROM v_catalog.tables WHERE table_schema = 'public' AND table_name = '${SRC_TABLE}';")

if [ -z "$SRC_EXISTS" ]; then
  echo "❌ Source table $SRC_TABLE does not exist. Skipping data transfer."
  exit 1
fi

# Create target VT_TABLE if not exists
VT_EXISTS=$(vsql -h "$VT_HOST" -d "$VT_DBNAME" -U "$MIS_DB_USER" -w "$MIS_DB_PASS" -tAc \
  "SELECT table_name FROM v_catalog.tables WHERE table_schema = 'public' AND table_name = 'ivr_evr_details_${DDMM}';")

if [ -z "$VT_EXISTS" ]; then
    echo "ℹ️ Target table does not exist. Creating table: $VT_TABLE"
    vsql -h "$VT_HOST" -d "$VT_DBNAME" -U "$MIS_DB_USER" -w "$MIS_DB_PASS" <<EOF
CREATE TABLE $VT_TABLE (
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
fi

# Create OBD_TABLE if not exists
OBD_EXISTS=$(vsql -h "$VT_HOST" -d "$VT_DBNAME" -U "$MIS_DB_USER" -w "$MIS_DB_PASS" -tAc \
  "SELECT table_name FROM v_catalog.tables WHERE table_schema = 'public' AND table_name = 'obd_evr_details_${DDMM}';")

if [ -z "$OBD_EXISTS" ]; then
    echo "ℹ️ Target table does not exist. Creating table: $OBD_TABLE"
    vsql -h "$VT_HOST" -d "$VT_DBNAME" -U "$MIS_DB_USER" -w "$MIS_DB_PASS" <<EOF
CREATE TABLE $OBD_TABLE (
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
fi

echo "➡️ Getting latest gen_time from $SRC_TABLE..."
LAST_GEN_TIME=$(vsql -h "$VT_HOST" -d "$VT_DBNAME" -U "$MIS_DB_USER" -w "$MIS_DB_PASS" -tAc \
  "SELECT MAX(gen_time) FROM $SRC_TABLE;")

echo "ℹ️ Latest gen_time in $SRC_TABLE: $LAST_GEN_TIME"

# Transfer valid data
#echo "➡️ Transferring valid data from $SRC_TABLE to $VT_TABLE (excluding NULL or empty short_code)..."
#vsql -h "$VT_HOST" -d "$VT_DBNAME" -U "$MIS_DB_USER" -w "$MIS_DB_PASS" <<EOF
#INSERT INTO $VT_TABLE
#SELECT * FROM $SRC_TABLE
#COMMIT;
#EOF

echo "➡️ Transferring valid data from $SRC_TABLE to $VT_TABLE (evr_event between 1 and 13)..."

vsql -h "$VT_HOST" -d "$VT_DBNAME" -U "$MIS_DB_USER" -w "$MIS_DB_PASS" <<EOF
INSERT INTO $VT_TABLE
SELECT *
FROM $SRC_TABLE
WHERE evr_event BETWEEN 1 AND 13;
COMMIT;
EOF

# Transfer invalid (null/empty short_code) to OBD table
#echo "➡️ Transferring invalid short_code data to $OBD_TABLE and cleaning source..."
#vsql -h "$VT_HOST" -d "$VT_DBNAME" -U "$MIS_DB_USER" -w "$MIS_DB_PASS" <<EOF
#INSERT INTO $OBD_TABLE
#SELECT * FROM $SRC_TABLE
#WHERE short_code IS NULL OR TRIM(short_code) = '';
#COMMIT;

echo "➡️ Transferring valid data from $SRC_TABLE to $OBD_TABLE (evr_event between 51 and 55)..."

vsql -h "$VT_HOST" -d "$VT_DBNAME" -U "$MIS_DB_USER" -w "$MIS_DB_PASS" <<EOF
INSERT INTO $OBD_TABLE
SELECT *
FROM $SRC_TABLE
WHERE evr_event BETWEEN 51 AND 55;
COMMIT;
EOF


echo "➡️ Deleting processed rows from $SRC_TABLE (gen_time <= $LAST_GEN_TIME)..."
vsql -h "$VT_HOST" -d "$VT_DBNAME" -U "$MIS_DB_USER" -w "$MIS_DB_PASS" <<EOF
DELETE FROM $SRC_TABLE
WHERE gen_time <= TIMESTAMP '$LAST_GEN_TIME';
COMMIT;
EOF


#DELETE FROM $SRC_TABLE
#WHERE short_code IS NOT NULL AND TRIM(short_code) <> '';
#COMMIT;
#EOF

# Final row count confirmation
echo "✅ Final row count in $VT_TABLE:"
vsql -h "$VT_HOST" -d "$VT_DBNAME" -U "$MIS_DB_USER" -w "$MIS_DB_PASS" -c \
  "SELECT COUNT(*) FROM $VT_TABLE;"

echo "✅ Data transfer completed successfully for date $DDMM."

