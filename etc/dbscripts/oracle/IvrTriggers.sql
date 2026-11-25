CREATE SEQUENCE IVR_AB_SEQ_APP_ID
  START WITH 61
  MAXVALUE 9999999999999999999999999999
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;


CREATE SEQUENCE IVR_SEQ_APPL_ID
  START WITH 21
  MAXVALUE 9999999999999999999999999999
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;


CREATE SEQUENCE IVR_SEQ_CHARGE_ID
  START WITH 1
  MAXVALUE 9999999999999999999999999999
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;


CREATE SEQUENCE IVR_SEQ_COMMAND_ID
  START WITH 1
  MAXVALUE 9999999999999999999999999999
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;



CREATE SEQUENCE IVR_SEQ_NOTI_ID
  START WITH 1
  MAXVALUE 9999999999999999999999999999
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;


CREATE SEQUENCE IVR_SEQ_PROVIDER_ID
  START WITH 1
  MAXVALUE 9999999999999999999999999999
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;


CREATE SEQUENCE IVR_SEQ_ROUTINE_ID
  START WITH 41
  MAXVALUE 9999999999999999999999999999
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;


CREATE SEQUENCE IVR_SEQ_SCHEDULE_ID
  START WITH 1
  MAXVALUE 9999999999999999999999999999
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;


CREATE SEQUENCE IVR_SEQ_SPEAK_ID
  START WITH 41
  MAXVALUE 9999999999999999999999999999
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;


CREATE SEQUENCE IVR_SEQ_USER_ID
  START WITH 21
  MAXVALUE 9999999999999999999999999999
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;


CREATE SEQUENCE IVR_SEQ_USERTYPE_ID
  START WITH 21
  MAXVALUE 9999999999999999999999999999
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;

/*====================================Triggers========================================================*/

CREATE OR REPLACE TRIGGER "tib_app_builder_mast" before insert
on APP_BUILDER_MAST for each row
declare
    integrity_error  exception;
    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;

begin
    --  Column "APP_ID" uses sequence IVR.IVR_AB_SEQ_APP_ID
    select IVR_AB_SEQ_APP_ID.NEXTVAL INTO :new.APP_ID from dual;

--  Errors handling
exception
    when integrity_error then
       raise_application_error(errno, errmsg);
end;
/

CREATE OR REPLACE TRIGGER "tib_gui_user_details" before insert
on GUI_USER_DETAILS for each row
declare
    integrity_error  exception;
    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;

begin
    --  Column "USER_ID" uses sequence IVR_SEQ_USER_ID
    select IVR_SEQ_USER_ID.NEXTVAL INTO :new.USER_ID from dual;

--  Errors handling
exception
    when integrity_error then
       raise_application_error(errno, errmsg);
end;
/

CREATE OR REPLACE TRIGGER "tib_gui_user_type_mast" before insert
on GUI_USER_TYPE_MASTER for each row
declare
    integrity_error  exception;
    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;

begin
    --  Column "USER_ID" uses sequence IVR_SEQ_USER_ID
    select IVR_SEQ_USERTYPE_ID.NEXTVAL INTO :new.USER_TYPE_ID from dual;

--  Errors handling
exception
    when integrity_error then
       raise_application_error(errno, errmsg);
end;
/

CREATE OR REPLACE TRIGGER "tib_gui_user_type_master" before insert
on GUI_USER_TYPE_MASTER for each row
declare
    integrity_error  exception;
    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;

begin
    --  Column "USER_ID" uses sequence IVR_SEQ_USER_ID
    select IVR_SEQ_USERTYPE_ID.NEXTVAL INTO :new.USER_TYPE_ID from dual;

--  Errors handling
exception
    when integrity_error then
       raise_application_error(errno, errmsg);
end;
/

CREATE OR REPLACE TRIGGER "tib_ivr_app_provider" before insert
on IVR_APP_PROVIDER for each row
declare
    integrity_error  exception;
    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;

begin
    --  Column "PROVIDER_ID" uses sequence IVR_SEQ_PROVIDER_ID
    select IVR_SEQ_PROVIDER_ID.NEXTVAL INTO :new.PROVIDER_ID from dual;

--  Errors handling
exception
    when integrity_error then
       raise_application_error(errno, errmsg);
end;
/

CREATE OR REPLACE TRIGGER "tib_ivr_application" before insert
on IVR_APPLICATION for each row
declare
    integrity_error  exception;
    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;

begin
    --  Column "APPL_ID" uses sequence IVR_SEQ_APPL_ID
    select IVR_SEQ_APPL_ID.NEXTVAL INTO :new.APPL_ID from dual;

--  Errors handling
exception
    when integrity_error then
       raise_application_error(errno, errmsg);
end;
/

CREATE OR REPLACE TRIGGER "tib_ivr_channel_commands" before insert
on IVR_CHANNEL_COMMANDS for each row
declare
    integrity_error  exception;
    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;

begin
    --  Column "COMMAND_ID" uses sequence IVR_SEQ_COMMAND_ID
    select IVR_SEQ_COMMAND_ID.NEXTVAL INTO :new.COMMAND_ID from dual;

--  Errors handling
exception
    when integrity_error then
       raise_application_error(errno, errmsg);
end;
/

CREATE OR REPLACE TRIGGER "tib_ivr_charging_slab" before insert
on IVR_CHARGING_SLAB for each row
declare
    integrity_error  exception;
    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;

begin
    --  Column "CHARGE_ID" uses sequence IVR_SEQ_CHARGE_ID
    select IVR_SEQ_CHARGE_ID.NEXTVAL INTO :new.CHARGE_ID from dual;

--  Errors handling
exception
    when integrity_error then
       raise_application_error(errno, errmsg);
end;
/

CREATE OR REPLACE TRIGGER "tib_ivr_notification" before insert
on IVR_NOTIFICATION for each row
declare
    integrity_error  exception;
    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;

begin
    --  Column "NOTI_ID" uses sequence IVR_SEQ_NOTI_ID
    select IVR_SEQ_NOTI_ID.NEXTVAL INTO :new.NOTI_ID from dual;

--  Errors handling
exception
    when integrity_error then
       raise_application_error(errno, errmsg);
end;
/

CREATE OR REPLACE TRIGGER "tib_ivr_schedule" before insert
on IVR_SCHEDULE for each row
declare
    integrity_error  exception;
    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;

begin
    --  Column "SCHEDULE_ID" uses sequence IVR_SEQ_SCHEDULE_ID
    select IVR_SEQ_SCHEDULE_ID.NEXTVAL INTO :new.SCHEDULE_ID from dual;

--  Errors handling
exception
    when integrity_error then
       raise_application_error(errno, errmsg);
end;
/

CREATE OR REPLACE TRIGGER "tib_library_routines" before insert
on LIBRARY_ROUTINES for each row
declare
    integrity_error  exception;
    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;

begin
    --  Column "APP_ID" uses sequence IVR.IVR_AB_SEQ_APP_ID
    select IVR_SEQ_ROUTINE_ID.NEXTVAL INTO :new.ROUTINE_ID from dual;

--  Errors handling
exception
    when integrity_error then
       raise_application_error(errno, errmsg);
end;
/

CREATE OR REPLACE TRIGGER "tib_speak_format_display" before insert
on SPEAK_FORMAT_DISPLAY for each row
declare
    integrity_error  exception;
    errno            integer;
    errmsg           char(200);
    dummy            integer;
    found            boolean;

begin
    --  Column "APP_ID" uses sequence IVR.IVR_AB_SEQ_APP_ID
    select IVR_SEQ_SPEAK_ID.NEXTVAL INTO :new.SPEAK_ID from dual;

--  Errors handling
exception
    when integrity_error then
       raise_application_error(errno, errmsg);
end;
/
