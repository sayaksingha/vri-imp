-- @(#) $Id: IVR_DELETE_IVR_EVR_DETAILS.sql,v 1.1.4.1.2.1 2025/06/12 16:49:11 mram Exp $
-- ==============================================================
-- DBMS name:       PostgreSQL Version 16.9
-- ==============================================================
-- Procedure:       delete_ivr_evr_details_proc
-- ==============================================================

-- DROP PROCEDURE delete_ivr_evr_details_proc(numeric);

CREATE OR REPLACE PROCEDURE delete_ivr_evr_details_proc(IN i_days numeric)
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
   --DECLARE EXIT HANDLER FOR SQLEXCEPTION
   -- SELECT 'Error Occurred in Procedure DELETE_IVR_EVR_DETAILS_PROC'
   -- as Error;
tmpInterval := i_days;

--SELECT TO_CHAR( (clock_timestamp() + tmpInterval), 'DDMM') INTO STRICT lDate;

SELECT TO_CHAR(clock_timestamp() + (tmpInterval || ' days')::interval, 'DDMM') INTO STRICT lDate;

tablename := 'ivr_evr_details_' || lDate;

--query1 := 'SELECT count(*) FROM tab WHERE tname = ''' || tablename || '''';
query1 := 'SELECT count(*) FROM information_schema.tables WHERE table_name = ''' || tablename || '''';

EXECUTE query1 INTO STRICT lCount;

IF lCount >= 1 AND tmpInterval > 0
THEN
query2 := 'DROP TABLE ' || tablename || ' CASCADE';

EXECUTE query2;
END IF;
END;
$procedure$
;


