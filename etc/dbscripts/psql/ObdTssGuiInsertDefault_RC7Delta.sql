INSERT INTO cumulation_table
(cumulation_table_name, evr_table_name, cumulation_columns, evr_table_type, table_id, db_user, db_pswd, db_ip, db_name, db_srv, select_query)
VALUES('OBD_COMPLETE_MIS_CUMULATIVE', 'OBD_EVR_DETAILS', 'TOT_DURATION,TOT_CNT', 'D', 3, '<DB_USERNAME>', '<DB_PASSWORD>', '<DB_IP>', '<DB_NAME>', 'public', 'select  DATE_TRUNC(''hour'',      a.GEN_TIME) as GEN_TIME,        b.CLIENT_ID AS CLIENT_ID,       a.CAMP_ID,      a.EVR_EVENT,    a.EVR_CAUSE,    a.EVR_REASON,   SUM(a.DURATION) TOT_DURATION,   COUNT(*) TOT_CNT from   <:EVR_TABLE> a,         obd_campaign_mast b where  a.gen_time between TO_TIMESTAMP(<:startDate>, ''YYYY-MM-DD HH24:MI:SS'') and TO_TIMESTAMP(<:endDate>,   ''YYYY-MM-DD HH24:MI:SS'') AND a.CAMP_ID = b.camp_id  group by DATE_TRUNC(''hour'', a.GEN_TIME), b.client_id, a.CAMP_ID, a.EVR_EVENT, a.EVR_CAUSE, a.EVR_REASON');



COMMIT;
