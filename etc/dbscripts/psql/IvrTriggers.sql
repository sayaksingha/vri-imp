-- DROP SEQUENCE ivr_ab_seq_app_id;

CREATE SEQUENCE ivr_ab_seq_app_id
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 81
	CACHE 20
	NO CYCLE;

-- DROP SEQUENCE ivr_seq_appl_id;

CREATE SEQUENCE ivr_seq_appl_id
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 21
	CACHE 20
	NO CYCLE;

-- DROP SEQUENCE ivr_seq_charge_id;

CREATE SEQUENCE ivr_seq_charge_id
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 21
	CACHE 20
	NO CYCLE;

-- DROP SEQUENCE ivr_seq_command_id;

CREATE SEQUENCE ivr_seq_command_id
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 1
	CACHE 20
	NO CYCLE;

-- DROP SEQUENCE ivr_seq_noti_id;

CREATE SEQUENCE ivr_seq_noti_id
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 1
	CACHE 20
	NO CYCLE;


CREATE SEQUENCE ivr_seq_provider_id
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 21
	CACHE 20
	NO CYCLE;

CREATE SEQUENCE ivr_seq_routine_id
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 61
	CACHE 20
	NO CYCLE;


CREATE SEQUENCE ivr_seq_schedule_id
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 1
	CACHE 20
	NO CYCLE;


CREATE SEQUENCE ivr_seq_speak_id
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 81
	CACHE 20
	NO CYCLE;

CREATE SEQUENCE ivr_seq_user_id
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 41
	CACHE 20
	NO CYCLE;


CREATE SEQUENCE ivr_seq_usertype_id
	INCREMENT BY 1
	MINVALUE 1
	MAXVALUE 9223372036854775807
	START 41
	CACHE 20
	NO CYCLE;

/*====================================Triggers========================================================*/

-- DROP FUNCTION trigger_fct_tib_app_builder_mast();

CREATE OR REPLACE FUNCTION trigger_fct_tib_app_builder_mast()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
declare

    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;
BEGIN
  BEGIN
    --  Column "APP_ID" uses sequence IVR.IVR_AB_SEQ_APP_ID
    select nextval('ivr_ab_seq_app_id') INTO STRICT NEW.APP_ID;

--  Errors handling
exception
    when integrity_constraint_violation then
       RAISE EXCEPTION '%', errmsg USING ERRCODE = errno;
  END;
RETURN NEW;
end
$function$
;

-- DROP FUNCTION trigger_fct_tib_gui_user_details();

CREATE OR REPLACE FUNCTION trigger_fct_tib_gui_user_details()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
declare

    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;
BEGIN
  BEGIN
    --  Column "USER_ID" uses sequence IVR_SEQ_USER_ID
    select nextval('ivr_seq_user_id') INTO STRICT NEW.USER_ID;

--  Errors handling
exception
    when integrity_constraint_violation then
       RAISE EXCEPTION '%', errmsg USING ERRCODE = errno;
  END;
RETURN NEW;
end
$function$
;

-- DROP FUNCTION trigger_fct_tib_gui_user_type_mast();

CREATE OR REPLACE FUNCTION trigger_fct_tib_gui_user_type_mast()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
declare

    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;
BEGIN
  BEGIN
    --  Column "USER_ID" uses sequence IVR_SEQ_USER_ID
    select nextval('ivr_seq_usertype_id') INTO STRICT NEW.USER_TYPE_ID;

--  Errors handling
exception
    when integrity_constraint_violation then
       RAISE EXCEPTION '%', errmsg USING ERRCODE = errno;
  END;
RETURN NEW;
end
$function$
;

-- DROP FUNCTION trigger_fct_tib_gui_user_type_master();

CREATE OR REPLACE FUNCTION trigger_fct_tib_gui_user_type_master()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
declare

    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;
BEGIN
  BEGIN
    --  Column "USER_ID" uses sequence IVR_SEQ_USER_ID
    select nextval('ivr_seq_usertype_id') INTO STRICT NEW.USER_TYPE_ID;

--  Errors handling
exception
    when integrity_constraint_violation then
       RAISE EXCEPTION '%', errmsg USING ERRCODE = errno;
  END;
RETURN NEW;
end
$function$
;

-- DROP FUNCTION trigger_fct_tib_ivr_app_provider();

CREATE OR REPLACE FUNCTION trigger_fct_tib_ivr_app_provider()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
declare

    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;
BEGIN
  BEGIN
    --  Column "PROVIDER_ID" uses sequence IVR_SEQ_PROVIDER_ID
    select nextval('ivr_seq_provider_id') INTO STRICT NEW.PROVIDER_ID;

--  Errors handling
exception
    when integrity_constraint_violation then
       RAISE EXCEPTION '%', errmsg USING ERRCODE = errno;
  END;
RETURN NEW;
end
$function$
;


-- DROP FUNCTION trigger_fct_tib_ivr_application();

CREATE OR REPLACE FUNCTION trigger_fct_tib_ivr_application()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
declare

    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;
BEGIN
  BEGIN
    --  Column "APPL_ID" uses sequence IVR_SEQ_APPL_ID
    select nextval('ivr_seq_appl_id') INTO STRICT NEW.APPL_ID;

--  Errors handling
exception
    when integrity_constraint_violation then
       RAISE EXCEPTION '%', errmsg USING ERRCODE = errno;
  END;
RETURN NEW;
end
$function$
;


-- DROP FUNCTION trigger_fct_tib_ivr_channel_commands();

CREATE OR REPLACE FUNCTION trigger_fct_tib_ivr_channel_commands()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
declare

    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;
BEGIN
  BEGIN
    --  Column "COMMAND_ID" uses sequence IVR_SEQ_COMMAND_ID
    select nextval('ivr_seq_command_id') INTO STRICT NEW.COMMAND_ID;

--  Errors handling
exception
    when integrity_constraint_violation then
       RAISE EXCEPTION '%', errmsg USING ERRCODE = errno;
  END;
RETURN NEW;
end
$function$
;


-- DROP FUNCTION trigger_fct_tib_ivr_charging_slab();

CREATE OR REPLACE FUNCTION trigger_fct_tib_ivr_charging_slab()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
declare

    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;
BEGIN
  BEGIN
    --  Column "CHARGE_ID" uses sequence IVR_SEQ_CHARGE_ID
    select nextval('ivr_seq_charge_id') INTO STRICT NEW.CHARGE_ID;

--  Errors handling
exception
    when integrity_constraint_violation then
       RAISE EXCEPTION '%', errmsg USING ERRCODE = errno;
  END;
RETURN NEW;
end
$function$
;

-- DROP FUNCTION trigger_fct_tib_ivr_notification();

CREATE OR REPLACE FUNCTION trigger_fct_tib_ivr_notification()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
declare

    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;
BEGIN
  BEGIN
    --  Column "NOTI_ID" uses sequence IVR_SEQ_NOTI_ID
    select nextval('ivr_seq_noti_id') INTO STRICT NEW.NOTI_ID;

--  Errors handling
exception
    when integrity_constraint_violation then
       RAISE EXCEPTION '%', errmsg USING ERRCODE = errno;
  END;
RETURN NEW;
end
$function$
;

-- DROP FUNCTION trigger_fct_tib_ivr_schedule();

CREATE OR REPLACE FUNCTION trigger_fct_tib_ivr_schedule()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
declare

    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;
BEGIN
  BEGIN
    --  Column "SCHEDULE_ID" uses sequence IVR_SEQ_SCHEDULE_ID
    select nextval('ivr_seq_schedule_id') INTO STRICT NEW.SCHEDULE_ID;

--  Errors handling
exception
    when integrity_constraint_violation then
       RAISE EXCEPTION '%', errmsg USING ERRCODE = errno;
  END;
RETURN NEW;
end
$function$
;

-- DROP FUNCTION trigger_fct_tib_library_routines();

CREATE OR REPLACE FUNCTION trigger_fct_tib_library_routines()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
declare

    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;
BEGIN
  BEGIN
    --  Column "APP_ID" uses sequence IVR.IVR_AB_SEQ_APP_ID
    select nextval('ivr_seq_routine_id') INTO STRICT NEW.ROUTINE_ID;

--  Errors handling
exception
    when integrity_constraint_violation then
       RAISE EXCEPTION '%', errmsg USING ERRCODE = errno;
  END;
RETURN NEW;
end
$function$
;

create trigger tib_library_routines before
insert
    on
    library_routines for each row execute function trigger_fct_tib_library_routines();
-- DROP FUNCTION trigger_fct_tib_speak_format_display();

CREATE OR REPLACE FUNCTION trigger_fct_tib_speak_format_display()
 RETURNS trigger
 LANGUAGE plpgsql
AS $function$
declare

    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;
BEGIN
  BEGIN
    --  Column "APP_ID" uses sequence IVR.IVR_AB_SEQ_APP_ID
    select nextval('ivr_seq_speak_id') INTO STRICT NEW.SPEAK_ID;

--  Errors handling
exception
    when integrity_constraint_violation then
       RAISE EXCEPTION '%', errmsg USING ERRCODE = errno;
  END;
RETURN NEW;
end
$function$
;

CREATE TRIGGER tib_speak_format_display
BEFORE INSERT
ON speak_format_display
FOR EACH ROW
EXECUTE FUNCTION trigger_fct_tib_speak_format_display();

