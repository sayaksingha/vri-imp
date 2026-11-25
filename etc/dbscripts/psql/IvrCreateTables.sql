-- @(#) $Id: IvrCreateTables.sql,v 1.0 2025/06/12 12:00:00 rithesh Exp $

/*==============================================================*/
/* Table: IVR_LANG_MAST                                         */
/*==============================================================*/
CREATE TABLE ivr_lang_mast(
	lang_id int NOT NULL, 
	lang varchar(30) NOT NULL, 
	status smallint NOT NULL,
       	lang_dir varchar(10) NOT NULL, 
	lang_sort_order smallint NOT NULL, 
	PRIMARY KEY (lang_id)
);

/*==============================================================*/
/* Table: IVR_SUBSCRIBER                                        */
/*==============================================================*/
CREATE TABLE ivr_subscriber (
    subs_no        VARCHAR(20)  NOT NULL,
    imsi           VARCHAR(20),
    subs_name      VARCHAR(30),
    subs_type      SMALLINT,
    subs_dob       TIMESTAMP,
    email_id       VARCHAR(50),
    pref_lang_id   INT,
    sns_type       SMALLINT,
    login_pwd      VARCHAR(20),
    services       NUMERIC(38),
    status         SMALLINT     DEFAULT 1,
    entry_date     TIMESTAMP,
    PRIMARY KEY (subs_no)
);

/*==============================================================*/
/* Table: IVR_APPLICATION                                       */
/*==============================================================*/
CREATE TABLE ivr_application (
    appl_id           INT           NOT NULL,
    provider_id       INT,
    appl_name         VARCHAR(30)   NOT NULL,
    appl_type         SMALLINT,
    appl_desc         VARCHAR(150),
    appl_file         VARCHAR(200),
    upload_date       TIMESTAMP,
    last_update_date  TIMESTAMP,
    deleted_date      TIMESTAMP,
    deleted_flag      VARCHAR(1)    DEFAULT 'N',
    test_call_cli     VARCHAR(20),
    charge_id         INT,
    status            SMALLINT      NOT NULL,
    reason            VARCHAR(50),

    PRIMARY KEY (appl_id),
    UNIQUE (appl_name)
);

/*==============================================================*/
/* Table: IVR_SHORT_CODE_MAST                                    */
/*==============================================================*/
CREATE TABLE ivr_short_code_mast (
    short_code VARCHAR(20) NOT NULL,
    appl_id    INT,
    display    VARCHAR(100) NOT NULL,
    status     SMALLINT NOT NULL,
    PRIMARY KEY (short_code)
);

/*==============================================================*/
/* Table: IVR_APP_PROVIDER                                      */
/*==============================================================*/

CREATE TABLE ivr_app_provider (
    provider_id       INT             NOT NULL,
    revenue_share     DOUBLE PRECISION,
    status            INT,
    parent_provider_id INT,
    PRIMARY KEY (provider_id)
);

/*==============================================================*/
/* Table: IVR_CDR_DETAILS_DDMM                                  */
/*==============================================================*/

CREATE TABLE ivr_cdr_details_ddmm (
    subs_no        VARCHAR(20),
    subs_type      SMALLINT,
    call_ref_num   VARCHAR(20),
    service_type   SMALLINT,
    content        INT,
    duration       NUMERIC(38),
    units          NUMERIC(38),
    charge         NUMERIC(38),
    interface      SMALLINT,
    ref_id         INT,
    gen_time       TIMESTAMP
);

/*==============================================================*/
/* Table: IVR_CHANNEL_COMMANDS                                  */
/*==============================================================*/

CREATE TABLE ivr_channel_commands (
    command_id       INT NOT NULL,
    channel_id       INT NOT NULL,
    command          SMALLINT NOT NULL,
    "time"           TIMESTAMP,
    status           SMALLINT,
    patch_channel_id INT,
    PRIMARY KEY (command_id)
);

/*==============================================================*/
/* Table: IVR_CHANNEL_CONFIG                                    */
/*==============================================================*/

CREATE TABLE ivr_channel_config (
    channel_no   INT NOT NULL,
    channel_type SMALLINT NOT NULL,
    appl_id      INT,
    status       SMALLINT DEFAULT 0,
    PRIMARY KEY (channel_no)
);

/*==============================================================*/
/* Table: IVR_CHARGING_SLAB                                     */
/*==============================================================*/

CREATE TABLE ivr_charging_slab (
    charge_id         INT NOT NULL,
    slab_name         VARCHAR(30) NOT NULL,
    amount_per_minute NUMERIC(38) NOT NULL,
    service_key       INT NOT NULL,
    msc_address       VARCHAR(20),
    dest_number       VARCHAR(20),
    smsc_address      VARCHAR(20),
    vlr_number        VARCHAR(20),
    entry_time        TIMESTAMP NOT NULL,
    PRIMARY KEY (charge_id)
);

/*==============================================================*/
/* Table: IVR_CONFIG_DETAILS                                    */
/*==============================================================*/

CREATE TABLE ivr_config_details (
    config_key    VARCHAR(50),
    value         VARCHAR(50),
    display_name  VARCHAR(50)
);

/*==============================================================*/
/* Table: IVR_CONFIG_MAIN                                       */
/*==============================================================*/

CREATE TABLE ivr_config_main (
    config_key     VARCHAR(50) NOT NULL,
    config_type    VARCHAR(1),
    field_type     VARCHAR(1),
    min_val        VARCHAR(15),
    max_val        VARCHAR(15),
    def_val        VARCHAR(200),
    value          VARCHAR(200),
    gui_txt        VARCHAR(200),
    help_txt       VARCHAR(500),
    modified_date  TIMESTAMP,
    modified_user  VARCHAR(20),
    disp_order     SMALLINT,
    group_val      VARCHAR(5),
    PRIMARY KEY (config_key)
);

/*==============================================================*/
/* Table: IVR_EVR_CAUSE_MAST                                    */
/*==============================================================*/

CREATE TABLE ivr_evr_cause_mast (
    evr_cause   SMALLINT NOT NULL,
    description VARCHAR(40) NOT NULL,
    display     VARCHAR(30) NOT NULL,
    PRIMARY KEY (evr_cause)
);

/*==============================================================*/
/* Table: IVR_EVR_DETAILS_DDMM                                  */
/*==============================================================*/

CREATE TABLE ivr_evr_details_ddmm (
    evr_event      SMALLINT,
    evr_cause      SMALLINT,
    evr_reason     SMALLINT,
    ref_id        NUMERIC(38),
    short_code    VARCHAR(20) NOT NULL,
    request_mode  NUMERIC(38),
    call_ref_num  VARCHAR(20),
    cld_num       VARCHAR(20),
    clg_num       VARCHAR(20),
    content_id    INT,
    content       VARCHAR(100),
    duration      NUMERIC(38),
    charge_unit   SMALLINT DEFAULT 0,
    charge        NUMERIC(38),
    gen_time      TIMESTAMP,
    schedule_id   INT,
    camp_id       INT
);

/*==============================================================*/
/* Table: IVR_EVR_EVENT_MAST                                    */
/*==============================================================*/

CREATE TABLE ivr_evr_event_mast (
    evr_event   SMALLINT NOT NULL,
    description VARCHAR(40) NOT NULL,
    display     VARCHAR(30) NOT NULL,
    PRIMARY KEY (evr_event)
);

/*==============================================================*/
/* Table: IVR_EVR_REASON_MAST                                   */
/*==============================================================*/

CREATE TABLE ivr_evr_reason_mast (
    evr_reason  SMALLINT NOT NULL,
    description VARCHAR(40) NOT NULL,
    display     VARCHAR(40) NOT NULL,
    PRIMARY KEY (evr_reason)
);

/*==============================================================*/
/* Table: IVR_SCHEDULE                                          */
/*==============================================================*/

CREATE TABLE ivr_schedule (
    schedule_id   INT NOT NULL,
    schedule_type SMALLINT NOT NULL,
    call_ref_num  VARCHAR(20),
    ext_ref_num   VARCHAR(20),
    request_mode  SMALLINT,
    recipient_no  VARCHAR(20) NOT NULL,
    sender_no     VARCHAR(20) NOT NULL,
    appl_name     VARCHAR(20) NOT NULL,
    entry_time    TIMESTAMP NOT NULL,
    schedule_time TIMESTAMP,
    del_attempts  SMALLINT,
    status       SMALLINT,
    error_id      INT,
    end_time      TIMESTAMP,
    rsp_req       SMALLINT,
    feedback      VARCHAR(10),
    app_nop       SMALLINT,
    app_param1    VARCHAR(50),
    app_param2    VARCHAR(50),
    app_param3    VARCHAR(50),
    app_param4    VARCHAR(50),
    app_param5    VARCHAR(50),
    call_time     TIMESTAMP,
    duration      INT,
    PRIMARY KEY (schedule_id)
);

/*==============================================================*/
/* Table: IVR_NOTIFICATION                                      */
/*==============================================================*/

CREATE TABLE ivr_notification (
    noti_id     INT NOT NULL,
    noti_event  SMALLINT NOT NULL,
    call_ref_num VARCHAR(20),
    del_agent   SMALLINT NOT NULL,
    sub_agent   SMALLINT NOT NULL,
    user_id     VARCHAR(20) NOT NULL,
    text        VARCHAR(200),
    noti_time   TIMESTAMP NOT NULL,
    del_time    TIMESTAMP NOT NULL,
    status      SMALLINT,
    PRIMARY KEY (noti_id)
);

/*==============================================================*/
/* Table: IVR_NOTIFY_EVENTS                                     */
/*==============================================================*/

CREATE TABLE ivr_notify_events (
    noti_event   SMALLINT NOT NULL,
    text         VARCHAR(200) NOT NULL,
    severity     SMALLINT,
    description  VARCHAR(200),
    display      SMALLINT,
    PRIMARY KEY (noti_event)
);

ALTER TABLE ivr_application ADD CONSTRAINT fk_application_provider_id FOREIGN KEY (provider_id) REFERENCES ivr_app_provider (provider_id);

ALTER TABLE ivr_application ADD CONSTRAINT fk_appl_charge_id FOREIGN KEY (charge_id) REFERENCES ivr_charging_slab (charge_id);

ALTER TABLE ivr_channel_config ADD CONSTRAINT fk_channel_appl_id FOREIGN KEY (appl_id) REFERENCES ivr_application (appl_id);

ALTER TABLE ivr_config_details ADD CONSTRAINT fk_config_key FOREIGN KEY (config_key) REFERENCES ivr_config_main (config_key);


ALTER TABLE ivr_evr_details_ddmm ADD CONSTRAINT fk_evr_event FOREIGN KEY (evr_event) REFERENCES ivr_evr_event_mast (evr_event);

ALTER TABLE ivr_evr_details_ddmm ADD CONSTRAINT fk_evr_reason FOREIGN KEY (evr_reason) REFERENCES ivr_evr_reason_mast (evr_reason);

ALTER TABLE ivr_notification ADD CONSTRAINT fk_notification_event FOREIGN KEY (noti_event) REFERENCES ivr_notify_events (noti_event);

ALTER TABLE ivr_schedule ADD CONSTRAINT fk_schedule_appl_name FOREIGN KEY (appl_name) REFERENCES ivr_application (appl_name);

ALTER TABLE ivr_short_code_mast ADD CONSTRAINT fk_sc_appl_id FOREIGN KEY (appl_id) REFERENCES ivr_application (appl_id);

