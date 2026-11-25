--  @(#) $Id: IVR_CREATE_EVR_DETAILS.sql,v 1.1.4.1 2016/04/12 21:59:08 shijuc Exp $
#/*==============================================================*/
#/* DBMS name:       MYSQL Version 5.5.8-log                     */
#/*==============================================================*/

#/*==============================================================*/
#/* Procedure:      CREATE_IVR_EVR_DETAILS_PROC                  */
#/*==============================================================*/

DELIMITER |

DROP PROCEDURE IF EXISTS CREATE_IVR_EVR_DETAILS_PROC;

CREATE PROCEDURE `CREATE_IVR_EVR_DETAILS_PROC`(in i_days INTEGER)
BEGIN
   DECLARE count INTEGER;
   DECLARE lDate VARCHAR (20);
   DECLARE tmpInterval INTEGER default 1;

   #DECLARE EXIT HANDLER FOR SQLEXCEPTION
   #SELECT 'Error Occurred in Procedure CREATE_IVR_EVR_DETAILS_PROC'
   #as Error;

   SET @tmpInterval = i_days;
   SELECT DATE_FORMAT(DATE_ADD(SYSDATE(), INTERVAL @tmpInterval DAY),"%d%m") INTO lDate;

   SET @tablename:= CONCAT ("IVR_EVR_DETAILS_", lDate);

   SET @query1:= CONCAT("SELECT COUNT(*) INTO @count ",
         "FROM information_schema.tables ",
         "WHERE table_schema = 'IVR' ",
         "AND table_name = '",@tablename, "'");

   PREPARE STMT1 FROM @query1;
   EXECUTE STMT1;
   DEALLOCATE PREPARE STMT1;

   IF @count = 0
   THEN
      SET @query2 := CONCAT ("CREATE TABLE ",@tablename, " ( ",
            "EVR_EVENT smallint(6) DEFAULT NULL,",
            "EVR_CAUSE smallint(6) DEFAULT NULL,",
            "EVR_REASON smallint(6) DEFAULT NULL,",
            "REF_ID int(11) DEFAULT NULL COMMENT 'Channel Id for call, Package Id for Package, Gift, Renew',",
            "SHORT_CODE varchar(20) NOT NULL,",
            "REQUEST_MODE int(11) DEFAULT NULL COMMENT 'Mode of Request - 0: IVR, 1: SMS, 2:USSD, 3: GUI',",
            "CALL_REF_NUM varchar(20) DEFAULT NULL COMMENT 'Call Reference Number to track entire call events',",
            "CLD_NUM varchar(20) DEFAULT NULL COMMENT 'Called party Number or DNIS',",
            "CLG_NUM varchar(20) DEFAULT NULL COMMENT 'Calling party number or CLI',",
            "CONTENT_ID int(11) DEFAULT NULL COMMENT 'holds Content_Id for Play/Pause/Stop event',",
            "CONTENT varchar(100) DEFAULT NULL COMMENT 'holds either Content_file for Play/Pause/Stop event or CCXML/VXML document name used to answer the call',",
            "DURATION int(11) DEFAULT NULL COMMENT 'Duration for which the contents were played or call duration for call completion event',",
            "CHARGE_UNIT smallint(6) DEFAULT '0' COMMENT 'Charge Units: Amount-1, Min-2, Content-3, Subscription-4, Dedication-5, Other-6\n            ',",
            "CHARGE int(11) DEFAULT NULL,",
            "GEN_TIME datetime DEFAULT NULL COMMENT 'EVR generation time',",
            "SCHEDULE_ID int(11) DEFAULT NULL,",
            "CAMP_ID int(11) DEFAULT NULL ",
            ") ENGINE=InnoDB DEFAULT CHARSET=latin1;");

      PREPARE STMT2 FROM @query2;
      EXECUTE STMT2;
      DEALLOCATE PREPARE STMT2;

END IF;

END |
