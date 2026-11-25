-- @(#) $Id: IVR_CREATE_EVR_DETAILS.sql,v 1.1.4.1.2.1 2025/06/12 16:49:11 mram Exp $
-- ==============================================================
-- DBMS name:       PostgreSQL Version 16.9
-- ==============================================================
-- Procedure:       create_ivr_evr_details_proc
-- ==============================================================

-- DROP PROCEDURE create_ivr_evr_details_proc(numeric);

CREATE OR REPLACE PROCEDURE create_ivr_evr_details_proc(IN i_days numeric)
 LANGUAGE plpgsql
 SECURITY DEFINER
AS $procedure$
DECLARE

lCount        integer;
lDate         varchar(20);
tmpInterval   integer := 1;
tablename     varchar(100);
query1        varchar(1000);
query2        varchar(1000);

BEGIN
tmpInterval := i_days;

--SELECT TO_CHAR( (clock_timestamp() + tmpInterval), 'DDMM') INTO STRICT lDate;
SELECT TO_CHAR(clock_timestamp() + (tmpInterval || ' days')::interval, 'DDMM') INTO STRICT lDate;

tablename := 'ivr_evr_details_' || lDate;


--query1 := 'SELECT count(*) FROM tab WHERE tname = ''' || tablename || '''';
query1 := 'SELECT count(*) FROM information_schema.tables WHERE table_name = ''' || tablename || '''';

EXECUTE query1 INTO STRICT lCount;

IF lCount = 0
THEN
query2 :=
'CREATE TABLE '
|| tablename
|| ' ( '
      || 'EVR_EVENT INTEGER ,'
      || 'EVR_CAUSE INTEGER ,'
      || 'EVR_REASON INTEGER ,'
      || 'REF_ID BIGINT, '
      || 'SHORT_CODE VARCHAR(20),'
      || 'REQUEST_MODE BIGINT,'
      || 'CALL_REF_NUM VARCHAR(20) NOT NULL ,'
      || 'CLD_NUM VARCHAR(20) ,'
      || 'CLG_NUM VARCHAR(20) ,'
      || 'CONTENT_ID BIGINT  ,'
      || 'CONTENT VARCHAR(100) ,'
      || 'DURATION BIGINT ,'
      || 'CHARGE_UNIT INTEGER ,'
      || 'CHARGE BIGINT  ,'
      || 'GEN_TIME DATE ,'
      || 'SCHEDULE_ID BIGINT  ,'
      || 'CAMP_ID BIGINT '
      || ')';

      -- comment on column REQUEST_MODE is '0 - Prepaid
-- 1 - Postpaid';
EXECUTE query2;
END IF;
EXCEPTION
WHEN OTHERS
THEN
RAISE EXCEPTION '%', 'Error Occurred in Procedure CREATE_IVR_EVR_DETAILS_PROC '
      || SQLERRM
      || SQLSTATE USING ERRCODE = '45001';
END;
$procedure$
;

