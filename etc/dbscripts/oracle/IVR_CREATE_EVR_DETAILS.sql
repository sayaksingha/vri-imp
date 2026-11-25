--  @(#) $Id: IVR_CREATE_EVR_DETAILS.sql,v 1.1.4.1.2.1 2017/10/27 16:49:11 mram Exp $
#/*==============================================================*/
#/* DBMS name:       ORACLE Version 5.5.8-log                     */
#/*==============================================================*/

#/*==============================================================*/
#/* Procedure:      CREATE_IVR_EVR_DETAILS_PROC                  */
#/*==============================================================*/
/* Formatted on 1/20/2014 5:15:34 PM (QP5 v5.163.1008.3004) */

CREATE OR REPLACE PROCEDURE CREATE_IVR_EVR_DETAILS_PROC (
      i_days IN NUMBER)
AS
lCount        INTEGER;
lDate         VARCHAR2 (20);
tmpInterval   INTEGER DEFAULT 1;
tablename     VARCHAR2 (100);
query1        VARCHAR2 (1000);
query2        VARCHAR2 (1000);
BEGIN
tmpInterval := i_days;

SELECT TO_CHAR ( (SYSDATE + tmpInterval), 'DDMM') INTO lDate FROM DUAL;

tablename := 'IVR_EVR_DETAILS_' || lDate;

query1 := 'SELECT count(*) FROM tab WHERE tname = ''' || tablename || '''';

EXECUTE IMMEDIATE query1 INTO lCount;

IF lCount = 0
THEN
query2 :=
'CREATE TABLE '
|| tablename
|| ' ( '
      || 'EVR_EVENT NUMBER(6) ,'
      || 'EVR_CAUSE NUMBER(6) ,'
      || 'EVR_REASON NUMBER(6) ,'
      || 'REF_ID NUMBER(11), '
      || 'SHORT_CODE VARCHAR2(20 BYTE),'
      || 'REQUEST_MODE NUMBER(11) ,'
      || 'CALL_REF_NUM VARCHAR2(20 BYTE) NOT NULL ,'
      || 'CLD_NUM VARCHAR2(20 BYTE) ,'
      || 'CLG_NUM VARCHAR2(20 BYTE) ,'
      || 'CONTENT_ID NUMBER(11)  ,'
      || 'CONTENT VARCHAR2(100 BYTE) ,'
      || 'DURATION NUMBER(11) ,'
      || 'CHARGE_UNIT NUMBER(6) ,'
      || 'CHARGE NUMBER(11)  ,'
      || 'GEN_TIME DATE ,'
      || 'SCHEDULE_ID NUMBER(11)  ,'
      || 'CAMP_ID NUMBER(11)  '
      || ')';

      -- comment on column REQUEST_MODE is '0 - Prepaid
-- 1 - Postpaid';

EXECUTE IMMEDIATE query2;
END IF;
EXCEPTION
WHEN OTHERS
THEN
raise_application_error (
      -20001,
      'Error Occurred in Procedure CREATE_IVR_EVR_DETAILS_PROC '
      || SQLERRM
      || SQLCODE);
END;
/
