DECLARE
X NUMBER;
BEGIN
SYS.DBMS_JOB.SUBMIT(job        => x,
        what       => 'IVR.ivr_mis_cumulative_main_proc();',
        next_date  => to_date('20/01/2014 21:19:13','dd/mm/yyyy hh24:mi:ss'),
        interval   => 'SYSDATE+30/1440 ',
        no_parse   => TRUE
        );
END;
/

DECLARE
X NUMBER;
BEGIN
SYS.DBMS_JOB.SUBMIT(job        => x,
        what       => 'IVR.IVR_MIS_SHORTCODE_MAIN_PROC();',
        next_date  => sysdate,
        interval   => 'SYSDATE+30/1440 ',
        no_parse   => TRUE
        );
END;
/

DECLARE
X NUMBER;
BEGIN
SYS.DBMS_JOB.SUBMIT
( job       => X
  ,what      => 'IVR.CREATE_IVR_EVR_DETAILS_PROC (1 /* days */);'
  ,next_date => sysdate
  ,interval  => 'SYSDATE+30/1440 '
  ,no_parse  => FALSE
  );
END;
/



DECLARE
X NUMBER;
BEGIN
SYS.DBMS_JOB.SUBMIT
( job       => X
  ,what      => 'IVR.DELETE_IVR_EVR_DETAILS_PROC (15);'
  ,next_date => sysdate
  ,interval  => 'TRUNC(SYSDATE+1)+6/24'
  ,no_parse  => FALSE
  );
END;
/

