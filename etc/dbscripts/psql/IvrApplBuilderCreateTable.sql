-- @(#) $Id: IvrApplBuilderCreateTable.sql,v 1.0 2025/06/12 12:00:00 rithesh Exp $

CREATE TABLE app_builder_mast (
  app_id     INTEGER NOT NULL,
  app_name   VARCHAR(25) DEFAULT NULL,
  app_desc   VARCHAR(200) DEFAULT NULL,
  type       SMALLINT,
  user_id    INTEGER DEFAULT NULL,
  CONSTRAINT pk_app_builder_mast PRIMARY KEY (app_id)
);

CREATE TABLE ivr_vxml_element_master (
  element_id   INT NOT NULL,
  parent       SMALLINT,
  child        SMALLINT,
  display      VARCHAR(50) NOT NULL,
  description  VARCHAR(200),
  PRIMARY KEY (element_id)
);

CREATE TABLE ivr_vxml_element_parent (
  element_id INT,
  parent_id INT DEFAULT NULL
);

CREATE TABLE ivr_vxml_element_child (
  element_id INT,
  child_id   INT DEFAULT NULL
);

CREATE TABLE ivr_vxml_attr_datatype_master (
  datatype_id INT NOT NULL,
  value       SMALLINT,
  display     VARCHAR(50),
  PRIMARY KEY (datatype_id)
);

CREATE TABLE ivr_vxml_attr_datatype_values (
  datatype_id INT,
  value       VARCHAR(50) DEFAULT NULL
);

CREATE TABLE ivr_vxml_attribute_master (
  attribute_id INT NOT NULL,
  datatype_id  INT,
  display      VARCHAR(50),
  description  VARCHAR(250),
  PRIMARY KEY (attribute_id)
);

CREATE TABLE ivr_vxml_element_details (
  element_id    INT,
  attribute_id  INT,
  status        SMALLINT DEFAULT NULL
);

CREATE TABLE ivr_vxml_suprt_parent_element (
  parent_element_id INT NOT NULL,
  element_id        INT NOT NULL,
  display           VARCHAR(30) NOT NULL,
  description       VARCHAR(100) NOT NULL,
  image_name        VARCHAR(30) NOT NULL,
  display_order     SMALLINT NOT NULL DEFAULT 0,
  PRIMARY KEY (parent_element_id)
);


CREATE TABLE library_routines (
	routine_id bigserial NOT NULL,
	provider_id int8 DEFAULT 0 NOT NULL,
	"name" varchar(50) NOT NULL,
	description varchar(200) NULL,
	in_params varchar(200) NULL,
	out_params varchar(200) NULL,
	active varchar(1) DEFAULT 'Y'::character varying NULL,
	in_param_type varchar(50) NULL,
	in_value_list varchar(200) NULL,
	CONSTRAINT library_routines_pkey PRIMARY KEY (routine_id)
);

CREATE TABLE speak_format_display (
	speak_id bigserial NOT NULL,
	"type" varchar(20) NULL,
	"format" text NULL,
	display_order int8 NULL,
	CONSTRAINT speak_format_display_pkey PRIMARY KEY (speak_id)
);


CREATE TABLE property (
  name             VARCHAR(50) NOT NULL,
  type             VARCHAR(1) NOT NULL,
  value            VARCHAR(250),
  description      VARCHAR(200),
  min              INT,
  max              INT,
  validation_expr  VARCHAR(200),
  PRIMARY KEY (name)
);

CREATE TABLE ivr_snapshot_cumulative (
  gen_time     TIMESTAMP NOT NULL,
  evr_event    SMALLINT NOT NULL,
  evr_cause    SMALLINT NOT NULL,
  evr_reason   SMALLINT NOT NULL DEFAULT 0,
  count        INT DEFAULT 0,
  evr_duration INT,
  PRIMARY KEY (gen_time, evr_event, evr_cause, evr_reason)
);

CREATE TABLE ivr_mis_content_cumulative (
  gen_date              TIMESTAMP NOT NULL,
  gen_hour              SMALLINT NOT NULL,
  provider_id           SMALLINT NOT NULL,
  no_content_upload     SMALLINT DEFAULT 0,
  no_content_delete     SMALLINT DEFAULT 0,
  no_content_available  SMALLINT DEFAULT 0,
  no_content_blocked    SMALLINT DEFAULT 0,
  no_content_preview    SMALLINT DEFAULT 0,
  no_content_rejected   SMALLINT DEFAULT 0,
  PRIMARY KEY (gen_date, gen_hour, provider_id)
);

CREATE TABLE ivr_key_subs_mis_cumulative (
  gen_time     TIMESTAMP NOT NULL,
  evr_event    SMALLINT NOT NULL,
  evr_cause    SMALLINT NOT NULL,
  request_mode SMALLINT NOT NULL,
  pkg_id       SMALLINT NOT NULL,
  cnt          INT DEFAULT 0,
  amt          SMALLINT DEFAULT 0,
  evr_reason   NUMERIC NOT NULL,
  gen_hour     NUMERIC NOT NULL,
  PRIMARY KEY (
    gen_time,
    evr_event,
    evr_cause,
    request_mode,
    pkg_id,
    gen_hour,
    evr_reason
  )
);

CREATE TABLE ivr_content_mis_cumulative (
  gen_time     TIMESTAMP NOT NULL,
  evr_event    SMALLINT NOT NULL,
  evr_cause    SMALLINT NOT NULL,
  content_id   VARCHAR(20) NOT NULL,
  charge_unit  SMALLINT NOT NULL,
  cnt          INT DEFAULT 0,
  cnt_dur      INT DEFAULT 0,
  amt          INT DEFAULT 0,
  PRIMARY KEY (
    gen_time,
    evr_event,
    evr_cause,
    content_id,
    charge_unit
  )
);

CREATE TABLE ivr_appl_mis_cumulative (
  gen_time     TIMESTAMP NOT NULL,
  evr_event    SMALLINT NOT NULL,
  evr_cause    SMALLINT NOT NULL,
  app_id       VARCHAR(20) NOT NULL,
  charge_unit  SMALLINT NOT NULL,
  cnt          INT DEFAULT 0,
  amt          INT DEFAULT 0,
  PRIMARY KEY (
    gen_time,
    evr_event,
    evr_cause,
    app_id,
    charge_unit
  )
);

CREATE TABLE ivr_mis_cumulative (
  gen_time    TIMESTAMP,
  evr_event   INT,
  evr_cause   INT,
  evr_reason  INT,
  short_code  VARCHAR(20),
  count       NUMERIC(38) DEFAULT NULL,
  duration    NUMERIC,
  UNIQUE (
    gen_time,
    short_code,
    evr_cause,
    evr_event,
    evr_reason
  )
);



