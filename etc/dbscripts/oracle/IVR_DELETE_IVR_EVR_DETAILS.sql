--  @(#) $Id: IVR_DELETE_IVR_EVR_DETAILS.sql,v 1.1.4.1 2016/04/12 21:59:10 shijuc Exp $
#/*==============================================================*/
#/* DBMS name:       ORACLE                                     */
#/*==============================================================*/

#/*==============================================================*/
#/* Procedure:       DELETE_IVR_EVR_DETAILS_PROC                 */
#/*==============================================================*/

CREATE OR REPLACE PROCEDURE DELETE_IVR_EVR_DETAILS_PROC (i_days IN NUMBER)
   AS
   lCount        INTEGER;
   lDate         VARCHAR2 (20);
   tmpInterval   INTEGER DEFAULT 1;
   tablename     VARCHAR2 (100);
   query1        VARCHAR2 (1000);
   query2        VARCHAR2 (1000);
   BEGIN
   --DECLARE EXIT HANDLER FOR SQLEXCEPTION
   -- SELECT 'Error Occurred in Procedure DELETE_IVR_EVR_DETAILS_PROC'
   -- as Error;

tmpInterval := i_days;

SELECT TO_CHAR ( (SYSDATE + tmpInterval), 'DDMM') INTO lDate FROM DUAL;

tablename := 'IVR_EVR_DETAILS_' || lDate;

query1 := 'SELECT count(*) FROM tab WHERE tname = ''' || tablename || '''';

EXECUTE IMMEDIATE query1 INTO lCount;

IF lCount >= 1 AND tmpInterval > 0
THEN
query2 := 'DROP TABLE ' || tablename || ' CASCADE';

EXECUTE IMMEDIATE query2;
END IF;
END;
/
