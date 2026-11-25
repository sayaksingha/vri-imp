--  @(#) $Id: IVR_DELETE_IVR_EVR_DETAILS.sql,v 1.1.4.1 2016/04/12 21:59:09 shijuc Exp $
#/*==============================================================*/
#/* DBMS name:       MYSQL Version 5.5.8-log                     */
#/*==============================================================*/

#/*==============================================================*/
#/* Procedure:       DELETE_IVR_EVR_DETAILS_PROC                 */
#/*==============================================================*/

DELIMITER |

DROP PROCEDURE IF EXISTS DELETE_IVR_EVR_DETAILS_PROC;

CREATE PROCEDURE `DELETE_IVR_EVR_DETAILS_PROC`(in i_days INTEGER)

BEGIN
   DECLARE count INTEGER;
   DECLARE lDate VARCHAR (20);
   DECLARE tmpInterval INTEGER default 1;

   DECLARE EXIT HANDLER FOR SQLEXCEPTION
      SELECT 'Error Occurred in Procedure DELETE_IVR_EVR_DETAILS_PROC'
      as Error;

   SET @tmpInterval = i_days;
   SELECT DATE_FORMAT(DATE_SUB(SYSDATE()
      , INTERVAL @tmpInterval DAY),"%d%m")
   INTO
      lDate;

   SET @tablename:= CONCAT ("IVR_EVR_DETAILS_", lDate);
   SET @query1:= CONCAT("SELECT COUNT(*) INTO @count ",
                        "FROM information_schema.tables ",
                        "WHERE table_schema = 'IVR' ",
                        "AND table_name = '",@tablename, "'");

   PREPARE STMT1 FROM @query1;
   EXECUTE STMT1;
   DEALLOCATE PREPARE STMT1;

   IF @count >= 1 AND @tmpInterval > 0
   THEN
      SET @query2 := CONCAT ("DROP TABLE ", @tablename, " CASCADE");

      PREPARE STMT2 FROM @query2;
      EXECUTE STMT2;
      DEALLOCATE PREPARE STMT2;

   END IF;

END |
