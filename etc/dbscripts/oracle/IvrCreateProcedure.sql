--  @(#) $Id: IvrCreateProcedure.sql,v 1.1.4.1.2.1 2017/11/14 02:54:47 mram Exp $

CREATE OR REPLACE PROCEDURE ivr_mis_snapshot_proc (
   mis_table      IN   VARCHAR2,
   proc_exetime   IN   DATE,
   end_time       IN   DATE
)
AS
   TYPE cur1 IS REF CURSOR;

   cm1            cur1;
   sqlquery       VARCHAR2 (2000);
   lgen_date      DATE;
   levr_event     NUMBER (6);
   levr_cause     NUMBER (6);
   levr_reason    NUMBER (6);
   ltot_dur_cnt   NUMBER (11)     DEFAULT 0;
   ltot_cnt       NUMBER (11)     DEFAULT 0;
BEGIN
   SAVEPOINT start_transaction_trans;
   sqlquery :=
         'SELECT TRUNC(GEN_TIME), EVR_EVENT, EVR_CAUSE, EVR_REASON, SUM(DURATION) TOT_DUR_CNT, COUNT(*) TOT_CNT FROM '
      || mis_table
      || ' WHERE GEN_TIME >= :1 AND GEN_TIME < :2 GROUP BY TRUNC(GEN_TIME), EVR_EVENT, EVR_CAUSE, EVR_REASON';
   DBMS_OUTPUT.put_line ('Query: ' || sqlquery);

   OPEN cm1 FOR sqlquery USING proc_exetime, end_time;

   LOOP
      BEGIN
         FETCH cm1
          INTO lgen_date, levr_event, levr_cause, levr_reason, ltot_dur_cnt,
               ltot_cnt;

         EXIT WHEN cm1%NOTFOUND;

         INSERT INTO ivr_snapshot_cumulative
                     (gen_time, evr_event, evr_cause, evr_reason,
                      evr_duration, COUNT
                     )
              VALUES (lgen_date, levr_event, levr_cause, levr_reason,
                      ltot_dur_cnt, ltot_cnt
                     );
      EXCEPTION
         WHEN DUP_VAL_ON_INDEX
         THEN
            UPDATE ivr_snapshot_cumulative
               SET COUNT = COUNT + ltot_cnt,
                   evr_duration = evr_duration + ltot_dur_cnt
             WHERE gen_time = lgen_date
               AND evr_event = levr_event
               AND evr_cause = levr_cause
               AND evr_reason = levr_reason;
         WHEN OTHERS
         THEN
            DBMS_OUTPUT.put_line ('Exception in execution of procedure: ');
            ROLLBACK TO start_transaction_trans;
            RAISE;
      END;
   END LOOP;

   CLOSE cm1;
END;
/


CREATE OR REPLACE PROCEDURE ivr_mis_cumulative_main_proc
AS
   mis_table      VARCHAR2 (100);
   day_index      NUMBER;
   start_time     DATE;
   proc_exetime   DATE;
   tab_name       VARCHAR2 (100);
   table_count    NUMBER;
BEGIN
   SAVEPOINT start_transaction_snapshot;

   SELECT (SYSDATE - 60 / 86400)
     INTO start_time
     FROM DUAL;

   day_index := 0;

   SELECT from_date
     INTO proc_exetime
     FROM gui_procedure_exc_time
    WHERE proc_name = 'IVR_SNAPSHOT_CUMULATIVE_PROC';

   LOOP
      EXIT WHEN TRUNC (proc_exetime + day_index) > SYSDATE;
      mis_table :=
             'IVR_EVR_DETAILS_' || TO_CHAR (proc_exetime + day_index, 'ddmm');
      table_count := 0;

      SELECT COUNT (*)
        INTO table_count
        FROM tab
       WHERE tname = mis_table;

      IF table_count = 1
      THEN
         ivr_mis_snapshot_proc (mis_table, proc_exetime, start_time);
      END IF;

      day_index := day_index + 1;
   END LOOP;

   UPDATE gui_procedure_exc_time
      SET from_date = start_time
    WHERE proc_name = 'IVR_SNAPSHOT_CUMULATIVE_PROC';

   COMMIT;
EXCEPTION
   WHEN NO_DATA_FOUND
   THEN
      ROLLBACK TO start_transaction_snapshot;
   WHEN OTHERS
   THEN
      ROLLBACK TO start_transaction_snapshot;
END;
/

CREATE OR REPLACE PROCEDURE ivr_mis_shortcode_proc (
   mis_table      IN   VARCHAR2,
   proc_exetime   IN   DATE,
   end_time       IN   DATE
)
AS
   TYPE cur1 IS REF CURSOR;

   cm1            cur1;
   sqlquery       VARCHAR2 (2000);
   lgen_date      DATE;
   levr_event     NUMBER (6);
   levr_cause     NUMBER (6);
   levr_reason    NUMBER (6);
   lshortcode     VARCHAR2 (21)   DEFAULT NULL;
   ltot_cnt       NUMBER (11)     DEFAULT 0;
   lduration      NUMBER (11)     DEFAULT 0;
BEGIN
   SAVEPOINT start_transaction_trans;
   sqlquery :=
         'SELECT TRUNC(GEN_TIME), EVR_EVENT, EVR_CAUSE, EVR_REASON, SHORT_CODE, COUNT(*) TOT_CNT,DURATION FROM '
      || mis_table
      || ' WHERE GEN_TIME >= :1 AND GEN_TIME < :2 GROUP BY TRUNC(GEN_TIME), EVR_EVENT, EVR_CAUSE, EVR_REASON,SHORT_CODE,DURATION';
   DBMS_OUTPUT.put_line ('Query: ' || sqlquery);

   OPEN cm1 FOR sqlquery USING proc_exetime, end_time;

   LOOP
      BEGIN
         FETCH cm1
          INTO lgen_date, levr_event, levr_cause, levr_reason, lshortcode,
               ltot_cnt, lduration;

         EXIT WHEN cm1%NOTFOUND;

         INSERT INTO ivr_mis_cumulative
                     (gen_time, evr_event, evr_cause, evr_reason,
                     short_code , COUNT ,DURATION
                     )
              VALUES (lgen_date, levr_event, levr_cause, levr_reason,
                      lshortcode, ltot_cnt,lduration
                     );
      EXCEPTION
         WHEN DUP_VAL_ON_INDEX
         THEN
            UPDATE ivr_mis_cumulative
               SET COUNT = COUNT + ltot_cnt
                    WHERE gen_time = lgen_date
               AND evr_event = levr_event
               AND evr_cause = levr_cause
               AND evr_reason = levr_reason
               AND short_code = lshortcode
               AND lduration = lduration;
         WHEN OTHERS
         THEN
            DBMS_OUTPUT.put_line ('Exception in execution of procedure: ');
            ROLLBACK TO start_transaction_trans;
            RAISE;
      END;
   END LOOP;

   CLOSE cm1;
END;
/

CREATE OR REPLACE PROCEDURE IVR_MIS_SHORTCODE_MAIN_PROC
AS
   mis_table      VARCHAR2 (100);
   day_index      NUMBER;
   start_time     DATE;
   proc_exetime   DATE;
   tname          VARCHAR2 (100);
   table_count    NUMBER;
BEGIN
   SAVEPOINT start_transaction_snapshot;

   SELECT (SYSDATE - 60 / 86400)
     INTO start_time
     FROM DUAL;

   day_index := 0;

   SELECT from_date
     INTO proc_exetime
     FROM gui_procedure_exc_time
    WHERE proc_name = 'IVR_SHORTCODE_CUMULATIVE_PROC';

   LOOP
      EXIT WHEN TRUNC (proc_exetime + day_index) > SYSDATE;
      mis_table :=
             'IVR_EVR_DETAILS_' || TO_CHAR (proc_exetime + day_index, 'ddmm');
      table_count := 0;

      SELECT COUNT (*)
        INTO table_count
        FROM tab
       WHERE tname = mis_table;

      IF table_count = 1
      THEN
         IVR_MIS_SHORTCODE_PROC (mis_table, proc_exetime, start_time);
      END IF;

      day_index := day_index + 1;
   END LOOP;

   UPDATE gui_procedure_exc_time
      SET from_date = start_time
    WHERE proc_name = 'IVR_SHORTCODE_CUMULATIVE_PROC';

   COMMIT;
EXCEPTION
   WHEN NO_DATA_FOUND
   THEN
      ROLLBACK TO start_transaction_snapshot;
   WHEN OTHERS
   THEN
      ROLLBACK TO start_transaction_snapshot;
END;
/

