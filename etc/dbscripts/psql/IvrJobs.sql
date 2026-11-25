GRANT EXECUTE ON PROCEDURE ivr.ivr_mis_cumulative_main_proc() TO ivr_new;
GRANT EXECUTE ON PROCEDURE ivr.ivr_mis_shortcode_main_proc() TO ivr_new;
GRANT EXECUTE ON PROCEDURE ivr.create_ivr_evr_details_proc(numeric) TO ivr_new;
GRANT EXECUTE ON PROCEDURE ivr.delete_ivr_evr_details_proc(numeric) TO ivr_new;


INSERT INTO cron.job (schedule, command, database, username)
VALUES (
  '*/30 * * * *',
  $$CALL ivr.ivr_mis_cumulative_main_proc();$$,
  current_database(),
  current_user
);

INSERT INTO cron.job (schedule, command, database, username)
VALUES (
  '*/30 * * * *',
  $$CALL ivr.ivr_mis_shortcode_main_proc();$$,
  current_database(),
  current_user
);

INSERT INTO cron.job (schedule, command, database, username)
VALUES (
  '*/30 * * * *',
  $$CALL ivr.create_ivr_evr_details_proc(1);$$,
  current_database(),
  current_user
);

INSERT INTO cron.job (schedule, command, database, username)
VALUES (
  '0 6 * * *',
  $$CALL ivr.delete_ivr_evr_details_proc(15);$$,
  current_database(),
  current_user
);


