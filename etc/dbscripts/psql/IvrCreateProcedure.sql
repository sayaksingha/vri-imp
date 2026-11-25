-- DROP PROCEDURE obd.ivr_mis_snapshot_proc();

CREATE OR REPLACE PROCEDURE ivr_mis_snapshot_proc(
    mis_table TEXT,
    proc_exetime TIMESTAMP,
    end_time TIMESTAMP
)
LANGUAGE plpgsql
AS $body$
DECLARE
    cm1 REFCURSOR;
    sqlquery TEXT;
    lgen_date TIMESTAMP;
    levr_event INTEGER;
    levr_cause INTEGER;
    levr_reason INTEGER;
    ltot_dur_cnt BIGINT := 0;
    ltot_cnt BIGINT := 0;
BEGIN
    sqlquery := format(
        'SELECT DATE_TRUNC(''day'', GEN_TIME), EVR_EVENT, EVR_CAUSE, EVR_REASON, SUM(DURATION), COUNT(*) 
         FROM %I 
         WHERE GEN_TIME >= $1 AND GEN_TIME < $2 
         GROUP BY DATE_TRUNC(''day'', GEN_TIME), EVR_EVENT, EVR_CAUSE, EVR_REASON',
        mis_table
    );

    RAISE NOTICE 'Query: %', sqlquery;

    OPEN cm1 FOR EXECUTE sqlquery USING proc_exetime, end_time;

    LOOP
        BEGIN
            FETCH cm1 INTO lgen_date, levr_event, levr_cause, levr_reason, ltot_dur_cnt, ltot_cnt;
            EXIT WHEN NOT FOUND;

            BEGIN
                INSERT INTO ivr_snapshot_cumulative (
                    gen_time, evr_event, evr_cause, evr_reason, evr_duration, COUNT
                )
                VALUES (
                    lgen_date, levr_event, levr_cause, levr_reason, ltot_dur_cnt, ltot_cnt
                );
            EXCEPTION
                WHEN unique_violation THEN
                    UPDATE ivr_snapshot_cumulative
                    SET
                        COUNT = COUNT + ltot_cnt,
                        evr_duration = evr_duration + ltot_dur_cnt
                    WHERE
                        gen_time = lgen_date
                        AND evr_event = levr_event
                        AND evr_cause = levr_cause
                        AND evr_reason = levr_reason;
            END;
        EXCEPTION
            WHEN OTHERS THEN
                RAISE NOTICE 'Exception occurred: %', SQLERRM;
                RAISE;
        END;
    END LOOP;

    CLOSE cm1;
END;
$body$;


-- DROP PROCEDURE obd.ivr_mis_cumulative_main_proc();

CREATE OR REPLACE PROCEDURE ivr_mis_cumulative_main_proc()
 LANGUAGE plpgsql
AS $procedure$
DECLARE
    mis_table     TEXT;
    day_index     INTEGER := 0;
    start_time    TIMESTAMP;
    proc_exetime  TIMESTAMP;
    tab_name      TEXT;
    table_count   INTEGER;
    target_date   DATE;
BEGIN
    start_time := clock_timestamp() - INTERVAL '1 day';

    SELECT from_date
    INTO STRICT proc_exetime
    FROM gui_procedure_exc_time
    WHERE proc_name = 'IVR_SNAPSHOT_CUMULATIVE_PROC';

    LOOP
        target_date := DATE_TRUNC('day', proc_exetime + (day_index || ' days')::interval);
        EXIT WHEN target_date > clock_timestamp();

        mis_table := 'ivr_evr_details_' || TO_CHAR(target_date, 'DDMM');

        SELECT COUNT(*)
        INTO table_count
        FROM information_schema.tables
        WHERE table_name = lower(mis_table)
        AND table_schema = 'public';  

        IF table_count = 1 THEN
            CALL ivr_mis_snapshot_proc(mis_table, target_date, start_time);
        END IF;

        day_index := day_index + 1;
    END LOOP;

    UPDATE gui_procedure_exc_time
    SET from_date = start_time
    WHERE proc_name = 'IVR_SNAPSHOT_CUMULATIVE_PROC';

EXCEPTION
    WHEN no_data_found THEN
        RAISE NOTICE 'No data found for gui_procedure_exc_time';
    WHEN OTHERS THEN
        RAISE NOTICE 'Unhandled exception: %', SQLERRM;
        RAISE;
END;
$procedure$
;


CREATE OR REPLACE PROCEDURE ivr_mis_shortcode_main_proc()
LANGUAGE plpgsql
AS $body$
DECLARE
    mis_table     TEXT;
    day_index     INTEGER := 0;
    start_time    TIMESTAMP;
    proc_exetime  TIMESTAMP;
    table_count   INTEGER;
    target_date   DATE;
BEGIN
    start_time := clock_timestamp() - INTERVAL '1 day';

    SELECT from_date
    INTO STRICT proc_exetime
    FROM gui_procedure_exc_time
    WHERE proc_name = 'IVR_SNAPSHOT_CUMULATIVE_PROC';

    LOOP
        target_date := DATE_TRUNC('day', proc_exetime + (day_index || ' days')::interval);
        EXIT WHEN target_date > clock_timestamp();

        mis_table := 'ivr_evr_details_' || TO_CHAR(target_date, 'DDMM');

        SELECT COUNT(*)
        INTO table_count
        FROM information_schema.tables
        WHERE table_name = LOWER(mis_table)
          AND table_schema = 'public';  

        IF table_count = 1 THEN
            CALL ivr_mis_shortcode_proc(mis_table, target_date, start_time);
        END IF;

        day_index := day_index + 1;
    END LOOP;

    UPDATE gui_procedure_exc_time
    SET from_date = start_time
    WHERE proc_name = 'IVR_SNAPSHOT_CUMULATIVE_PROC'; 

EXCEPTION
    WHEN no_data_found THEN
        RAISE NOTICE 'No data found in gui_procedure_exc_time for ivr_shortcode_cumulative_proc';
    WHEN OTHERS THEN
        RAISE NOTICE 'Unhandled exception: %', SQLERRM;
        RAISE;
END;
$body$;



-- DROP PROCEDURE obd.ivr_mis_shortcode_proc();

create or replace procedure ivr_mis_shortcode_proc (
    mis_table text,
    proc_exetime timestamp,
    end_time timestamp
)
language plpgsql
as $body$
declare
    cm1 refcursor;
    sqlquery text;
    lgen_date timestamp;
    levr_event integer;
    levr_cause integer;
    levr_reason integer;
    lshortcode varchar(21);
    ltot_cnt bigint := 0;
    lduration bigint := 0;
begin
    sqlquery := format(
        $$select date_trunc('day', gen_time), evr_event, evr_cause, evr_reason, short_code, count(*) as tot_cnt, duration
          from %I
         where gen_time >= $1 and gen_time < $2
         group by date_trunc('day', gen_time), evr_event, evr_cause, evr_reason, short_code, duration$$,
        mis_table
    );

    raise notice 'Query: %', sqlquery;

    open cm1 for execute sqlquery using proc_exetime, end_time;

    loop
        begin
            fetch cm1 into lgen_date, levr_event, levr_cause, levr_reason, lshortcode, ltot_cnt, lduration;
            exit when not found;

            begin
                insert into ivr_mis_cumulative (
                    gen_time, evr_event, evr_cause, evr_reason, short_code, count, duration
                )
                values (
                    lgen_date, levr_event, levr_cause, levr_reason, lshortcode, ltot_cnt, lduration
                );
            exception
                when unique_violation then
                    update ivr_mis_cumulative
                    set count = count + ltot_cnt
                    where gen_time = lgen_date
                      and evr_event = levr_event
                      and evr_cause = levr_cause
                      and evr_reason = levr_reason
                      and short_code = lshortcode
                      and duration = lduration;
            end;
        exception
            when others then
                raise notice 'Exception in execution: %', sqlerrm;
                raise;
        end;
    end loop;

    close cm1;
end;
$body$;
;


