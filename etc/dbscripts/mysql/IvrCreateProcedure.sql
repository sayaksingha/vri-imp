--  @(#) $Id: IvrCreateProcedure.sql,v 1.1.4.1 2016/04/12 21:59:09 shijuc Exp $
DELIMITER //

DROP PROCEDURE IF EXISTS gui_mis_snapshot_cumulative_proc;
//
CREATE PROCEDURE `gui_mis_snapshot_cumulative_proc`(
          IN mis_table VARCHAR(100),
          IN proc_exetime DATETIME,
          IN end_time DATETIME,
          OUT EXCEPTION_KEY SMALLINT(1))
BEGIN

   DECLARE lgen_date DATETIME;
   DECLARE levr_event SMALLINT(6) UNSIGNED;
   DECLARE levr_cause SMALLINT(6) UNSIGNED;
   DECLARE levr_reason SMALLINT(6) UNSIGNED;
   DECLARE ltot_dur_cnt INTEGER(11) UNSIGNED DEFAULT 0;
   DECLARE ltot_cnt INTEGER(11) UNSIGNED DEFAULT 0;
   DECLARE no_more_rows SMALLINT(1) UNSIGNED DEFAULT 0;
   DECLARE ext_from_proc SMALLINT(1) UNSIGNED DEFAULT 0;
   DECLARE getDetails CURSOR FOR SELECT DATE_FORMAT(GEN_TIME,'%Y/%m/%d'), EVR_EVENT, EVR_CAUSE, EVR_REASON, TOT_DURATION, TOT_CNT FROM MIS_TEMP_TABLE;

   DECLARE EXIT HANDLER FOR SQLEXCEPTION
      BEGIN
          SET EXCEPTION_KEY = 1;
          SELECT CONCAT('NOT FOUND - TERMINATING THE SUB PROCEDURE, TABLE NOT FOUND - ',mis_table);
      END;

   SET EXCEPTION_KEY = 0;
   BEGIN
      DECLARE EXIT HANDLER FOR NOT FOUND
         BEGIN
             SET ext_from_proc = 1;
             SELECT CONCAT('NOT FOUND - TERMINATING THE SUB PROCEDURE, NO DATA FOUND - ' ,mis_table);
         END;

      DECLARE EXIT HANDLER FOR 1329
          BEGIN
             SET ext_from_proc = 1;
             SELECT CONCAT('NOT FOUND - TERMINATING THE SUB PROCEDURE, EMPTY TABLE FOUND - ',mis_table);
          END;

      DECLARE EXIT HANDLER FOR 1146
          BEGIN
             SET ext_from_proc = 1;
             SELECT CONCAT('NOT FOUND - TERMINATING THE SUB PROCEDURE, TABLE NOT FOUND - ',mis_table);
          END;

      DECLARE EXIT HANDLER FOR SQLEXCEPTION
          BEGIN
              SET ext_from_proc = 1;
              SET EXCEPTION_KEY = 1;
              SELECT CONCAT('SQL EXCEPTION - TERMINATING THE SUB PROCEDURE - ',mis_table);
          END;

      SET @cnt_sqlquery = concat('SELECT EVR_EVENT INTO @EVENT FROM ',mis_table,' WHERE CAST(DATE_FORMAT(GEN_TIME,"%Y/%m/%d %T") AS DATETIME)>=CAST(DATE_FORMAT(\'',
      Proc_exetime,'\',"%Y/%m/%d %T") AS DATETIME) AND CAST(DATE_FORMAT(GEN_TIME,"%Y/%m/%d %T") AS DATETIME)<CAST(DATE_FORMAT(\'',end_time,
      '\',"%Y/%m/%d %T") AS DATETIME) LIMIT 1');
      PREPARE stmt FROM @cnt_sqlquery;
      EXECUTE stmt;
      DEALLOCATE PREPARE stmt;
   END;

   BEGIN
      DECLARE CONTINUE HANDLER FOR NOT FOUND
         BEGIN
             SET no_more_rows = 1;
             SELECT CONCAT('NOT FOUND - TERMINATING THE SUB PROCEDURE, END OF RESULT SET - ' ,mis_table);
         END;

      DECLARE EXIT HANDLER FOR SQLEXCEPTION
          BEGIN
              SET EXCEPTION_KEY = 1;
              SELECT CONCAT('SQL EXCEPTION - TERMINATING THE SUB PROCEDURE - ',mis_table);
          END;

      IF ext_from_proc = 0 THEN
         DROP TEMPORARY TABLE IF EXISTS MIS_TEMP_TABLE;
         CREATE TEMPORARY TABLE MIS_TEMP_TABLE (
                     GEN_TIME datetime DEFAULT '0000-00-00 00:00:00',
                     EVR_EVENT smallint(6) DEFAULT '0',
                     EVR_CAUSE smallint(6) DEFAULT '0',
                     EVR_REASON smallint(6) DEFAULT '0',
                     TOT_DURATION int(11) DEFAULT '0',
                     TOT_CNT int(11) DEFAULT '0');

         SET @insertQuery = concat('insert into MIS_TEMP_TABLE(GEN_TIME, EVR_EVENT, EVR_CAUSE, EVR_REASON, TOT_DURATION, TOT_CNT) SELECT DATE_FORMAT(GEN_TIME,"%Y/%m/%d"), EVR_EVENT, EVR_CAUSE, EVR_REASON, SUM(DURATION) TOT_DUR_CNT, COUNT(*) TOT_CNT FROM ',mis_table,' WHERE CAST(DATE_FORMAT(GEN_TIME,"%Y/%m/%d %T") AS DATETIME)>=CAST(DATE_FORMAT(\'',proc_exetime,'\',"%Y/%m/%d %T") AS DATETIME) AND CAST(DATE_FORMAT(GEN_TIME,"%Y/%m/%d %T") AS DATETIME)<CAST(DATE_FORMAT(\'',end_time,'\',"%Y/%m/%d %T") AS DATETIME) GROUP BY DATE_FORMAT(GEN_TIME,"%Y/%m/%d"), EVR_EVENT, EVR_CAUSE, EVR_REASON');
         PREPARE stmt FROM @insertQuery;
         EXECUTE stmt;
         DEALLOCATE PREPARE stmt;

         OPEN getDetails;
         LOOP_CURSOR : LOOP
         FETCH getDetails INTO lgen_date, levr_event, levr_cause, levr_reason, ltot_dur_cnt, ltot_cnt;

         IF no_more_rows = 1 THEN
            LEAVE LOOP_CURSOR;
         END IF;

         --  START OF INSERT AND UPDATE QUERY FOR MIS_CUMULATIVE TABLE
         BEGIN
             DECLARE CONTINUE HANDLER FOR 1062
                 BEGIN
                     --  START OF UPDATE QUERY FOR MIS_CUMULATIVE TABLE

                          SET @sqlquery1 = CONCAT('UPDATE IVR_SNAPSHOT_CUMULATIVE SET COUNT = COUNT + ',ltot_cnt,', EVR_DURATION = EVR_DURATION + ',ltot_dur_cnt,
                          ' WHERE CAST(DATE_FORMAT(GEN_TIME,"%Y/%m/%d %T") AS DATETIME) = CAST(DATE_FORMAT(''',lgen_date,''',"%Y/%m/%d %T") AS DATETIME) AND EVR_EVENT = ',levr_event,
                          ' AND EVR_CAUSE = ',levr_cause,' AND EVR_REASON = ',levr_reason);

                     --  END OF UPDATE QUERY FOR MIS_CUMULATIVE TABLE

                     PREPARE stmt1 FROM @sqlquery1;
                     EXECUTE stmt1;
                     DEALLOCATE PREPARE stmt1;
                 END;

             DECLARE EXIT HANDLER FOR SQLEXCEPTION
                 BEGIN
                     SET EXCEPTION_KEY = 1;
                     SELECT 'ERROR OCCURRED - TERMINATING THE SUB PROCEDURE';
                 END;

             --  START OF INSERT QUERY FOR MIS_CUMULATIVE TABLE
                  SET @sqlquery = CONCAT('INSERT INTO IVR_SNAPSHOT_CUMULATIVE(GEN_TIME, EVR_EVENT, EVR_CAUSE, EVR_REASON, EVR_DURATION, COUNT) VALUES (''',lgen_date,''',',levr_event,',',levr_cause,',',levr_reason,',',ltot_dur_cnt,',',ltot_cnt,')');
                  PREPARE stmt FROM @sqlquery;
                  EXECUTE stmt;
                  DEALLOCATE PREPARE stmt;
             --  END OF INSERT QUERY FOR MIS_CUMULATIVE TABLE

         END;
         --  END OF INSERT AND UPDATE QUERY FOR MIS_CUMULATIVE TABLE

         END LOOP LOOP_CURSOR;
         CLOSE getDetails;
      END IF;
   END;
END;
//


DROP PROCEDURE IF EXISTS ivr_mis_cumulative_main_proc;
//
CREATE PROCEDURE `ivr_mis_cumulative_main_proc`()
BEGIN

   DECLARE mis_table VARCHAR(100);
   DECLARE day_index SMALLINT(2) UNSIGNED DEFAULT 0;
   DECLARE start_time DATETIME;
   DECLARE proc_exetime DATETIME;
   DECLARE EXCEPTION_KEY SMALLINT(1) UNSIGNED DEFAULT 0;

   BEGIN
      DECLARE EXIT HANDLER FOR SQLEXCEPTION
      BEGIN
          SELECT 'ERROR OCCURRED - TERMINATING THE MAIN PROCEDURE';
      END;

      SELECT CAST(DATE_FORMAT(SYSDATE(),"%Y/%m/%d %T") AS DATETIME) - INTERVAL 1 MINUTE INTO start_time;

      START TRANSACTION;
      SET AUTOCOMMIT=0;

      -- START OF SUB PROCEDURE CALL
      SAVEPOINT start_transaction_snapshot;

      BEGIN
         DECLARE EXIT HANDLER FOR SQLEXCEPTION
         BEGIN
             SELECT 'ERROR OCCURRED - TERMINATING THE MAIN PROCEDURE';
             ROLLBACK TO SAVEPOINT start_transaction_snapshot;
         END;

         SELECT from_date
            INTO proc_exetime
            FROM GUI_PROCEDURE_EXC_TIME
         WHERE proc_name = 'IVR_SNAPSHOT_CUMULATIVE_PROC';

         loop_label: LOOP
              IF((CAST(DATE_FORMAT((proc_exetime + INTERVAL day_index DAY), "%Y/%m/%d %T") AS DATETIME)) > (CAST(DATE_FORMAT(start_time, "%Y/%m/%d %T") AS DATETIME))) THEN
                  LEAVE loop_label;
              ELSE
                  SET mis_table = concat('IVR_EVR_DETAILS_',DATE_FORMAT(proc_exetime + INTERVAL day_index DAY, '%d%m'));
                  CALL gui_mis_snapshot_cumulative_proc (mis_table, proc_exetime, start_time, EXCEPTION_KEY);
                  IF EXCEPTION_KEY = 1
                  THEN
                        ROLLBACK TO SAVEPOINT start_transaction_snapshot;
                        LEAVE loop_label;
                  END IF;
                  SET day_index = day_index + 1;
              END IF;
         END LOOP;

         IF EXCEPTION_KEY = 0
         THEN
              UPDATE GUI_PROCEDURE_EXC_TIME
              SET from_date = start_time
              WHERE proc_name = 'IVR_SNAPSHOT_CUMULATIVE_PROC';
         END IF;
      END;
      -- END OF SUB PROCEDURE CALL

   END;
   COMMIT;
END;
//

DELIMITER ;

