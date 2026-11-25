--  @(#) $Id: IvrCreateTables.sql,v 1.1.4.1 2016/04/12 21:59:11 shijuc Exp $

/*==============================================================*/
/* Table: IVR_LANG_MAST                                         */
/*==============================================================*/
create table IVR_LANG_MAST  (
      LANG_ID              NUMBER(6)                       not null,
      LANG                 varchar(30)                     not null,
      STATUS               NUMBER(3)                       not null,
      LANG_DIR             varchar(10)                     not null,
      LANG_SORT_ORDER      NUMBER(3)                       not null,
      constraint PK_IVR_LANG_MAST primary key (LANG_ID)
   );

/*==============================================================*/
/* Table: IVR_SUBSCRIBER                                        */
/*==============================================================*/
create table IVR_SUBSCRIBER  (
      SUBS_NO              varchar(20)                     not null,
      IMSI                 varchar(20),
      SUBS_NAME            varchar(30),
      SUBS_TYPE            NUMBER(3),
      SUBS_DOB             date,
      EMAIL_ID             varchar(50),
      PREF_LANG_ID         NUMBER(6),
      SNS_TYPE             NUMBER(3),
      LOGIN_PWD            varchar(20),
      SERVICES             INTEGER,
      STATUS               NUMBER(3)                      default 1,
      ENTRY_DATE           TIMESTAMP,
      constraint PK_IVR_SUBSCRIBER primary key (SUBS_NO)
      );


/*==============================================================*/
/* Table: IVR_APPLICATION                                       */
/*==============================================================*/
create table IVR_APPLICATION  (
      APPL_ID              NUMBER(6)                       not null,
      PROVIDER_ID          NUMBER(6),
      APPL_NAME            varchar(30)                     not null,
      APPL_TYPE            NUMBER(3),
      APPL_DESC            varchar(150),
      APPL_FILE            varchar(200),
      UPLOAD_DATE          TIMESTAMP,
      LAST_UPDATE_DATE     TIMESTAMP,
      DELETED_DATE         TIMESTAMP,
      DELETED_FLAG         VARCHAR(1)                     default 'N',
      TEST_CALL_CLI        varchar(20),
      CHARGE_ID            NUMBER(6),
      STATUS               NUMBER(3)                       not null,
      REASON               varchar(50),
      constraint PK_IVR_APPLICATION primary key (APPL_ID),
      constraint AK_KEY_2_IVR_APPL unique (APPL_NAME)
      );


/*==============================================================*/
/* Table: IVR_SHOT_CODE_MAST                                    */
/*==============================================================*/
create table IVR_SHORT_CODE_MAST  (
      SHORT_CODE           varchar(20)                     not null,
      APPL_ID              NUMBER(6),
      DISPLAY              varchar(100)                    not null,
      STATUS               NUMBER(3)                       not null,
      constraint PK_IVR_SHORT_CODE_MAST primary key (SHORT_CODE)
      );

/*==============================================================*/
/* Table: IVR_APP_PROVIDER                                      */
/*==============================================================*/
create table IVR_APP_PROVIDER  (
      PROVIDER_ID          NUMBER(6)                       not null,
      REVENUE_SHARE        float,
      STATUS               NUMBER(6),
      PARENT_PROVIDER_ID   NUMBER(6),
      constraint PK_IVR_APP_PROVIDER primary key (PROVIDER_ID)
      );


/*==============================================================*/
/* Table: IVR_CDR_DETAILS_DDMM                                  */
/*==============================================================*/
create table IVR_CDR_DETAILS_DDMM  (
      SUBS_NO              varchar(20),
      SUBS_TYPE            NUMBER(3),
      CALL_REF_NUM         varchar(20),
      SERVICE_TYPE         NUMBER(3),
      CONTENT              NUMBER(6),
      DURATION             INTEGER,
      UNITS                INTEGER,
      CHARGE               INTEGER,
      INTERFACE            NUMBER(1),
      REF_ID               NUMBER(6),
      GEN_TIME             TIMESTAMP
      );


/*==============================================================*/
/* Table: IVR_CHANNEL_COMMANDS                                  */
/*==============================================================*/
create table IVR_CHANNEL_COMMANDS  (
      COMMAND_ID           NUMBER(6)                       not null,
      CHANNEL_ID           NUMBER(6)                       not null,
      COMMAND              NUMBER(2)                       not null,
      TIME                 TIMESTAMP,
      STATUS               NUMBER(3),
      PATCH_CHANNEL_ID     NUMBER(6),
      constraint PK_IVR_CHANNEL_COMMANDS primary key (COMMAND_ID)
      );

/*==============================================================*/
/* Table: IVR_CHANNEL_CONFIG                                    */
/*==============================================================*/
create table IVR_CHANNEL_CONFIG  (
      CHANNEL_NO           NUMBER(6)                       not null,
      CHANNEL_TYPE         NUMBER(1)                       not null,
      APPL_ID              NUMBER(6),
      STATUS               NUMBER(3)                      default 0,
      constraint PK_IVR_CHANNEL_CONFIG primary key (CHANNEL_NO)
      );


/*==============================================================*/
/* Table: IVR_CHARGING_SLAB                                     */
/*==============================================================*/
create table IVR_CHARGING_SLAB  (
      CHARGE_ID            NUMBER(6)                       not null,
      SLAB_NAME            varchar(30)                     not null,
      AMOUNT_PER_MINUTE    numeric                         not null,
      SERVICE_KEY          NUMBER(6)                       not null,
      MSC_ADDRESS          varchar(20),
      DEST_NUMBER          varchar(20),
      SMSC_ADDRESS         varchar(20),
      VLR_NUMBER           varchar(20),
      ENTRY_TIME           TIMESTAMP                       not null,
      constraint PK_IVR_CHARGING_SLAB primary key (CHARGE_ID)
      );

/*==============================================================*/
/* Table: IVR_CONFIG_DETAILS                                    */
/*==============================================================*/
create table IVR_CONFIG_DETAILS  (
      CONFIG_KEY           varchar(50),
      VALUE                varchar(50),
      DISPLAY_NAME         varchar(50)
      );

/*==============================================================*/
/* Table: IVR_CONFIG_MAIN                                       */
/*==============================================================*/
create table IVR_CONFIG_MAIN  (
      CONFIG_KEY           varchar(50)                     not null,
      CONFIG_TYPE          VARCHAR(1),
      FIELD_TYPE           VARCHAR(1),
      MIN_VAL              varchar(15),
      MAX_VAL              varchar(15),
      DEF_VAL              varchar(200),
      VALUE                varchar(200),
      GUI_TXT              varchar(200),
      HELP_TXT             varchar(500),
      MODIFIED_DATE        TIMESTAMP,
      MODIFIED_USER        varchar(20),
      DISP_ORDER           NUMBER(3),
      GROUP_VAL            varchar(5),
      constraint PK_IVR_CONFIG_MAIN primary key (CONFIG_KEY)
      );

/*==============================================================*/
/* Table: IVR_EVR_CAUSE_MAST                                    */
/*==============================================================*/
create table IVR_EVR_CAUSE_MAST  (
      EVR_CAUSE            NUMBER(3)                       not null,
      DESCRIPTION          varchar(40)                     not null,
      DISPLAY              varchar(30)                     not null,
      constraint PK_IVR_EVR_CAUSE_MAST primary key (EVR_CAUSE)
      );

/*==============================================================*/
/* Table: IVR_EVR_DETAILS_DDMM                                  */
/*==============================================================*/
create table IVR_EVR_DETAILS_DDMM  (
      EVR_EVENT            NUMBER(3),
      EVR_CAUSE            NUMBER(3),
      EVR_REASON           NUMBER(3),
      REF_ID               INTEGER,
      SHORT_CODE           varchar(20)                     not null,
      REQUEST_MODE         INTEGER,
      CALL_REF_NUM         varchar(20),
      CLD_NUM              varchar(20),
      CLG_NUM              varchar(20),
      CONTENT_ID           NUMBER(6),
      CONTENT              varchar(100),
      DURATION             INTEGER,
      CHARGE_UNIT          NUMBER(3)                      default 0,
      CHARGE               INTEGER,
      GEN_TIME             TIMESTAMP,
      SCHEDULE_ID          NUMBER(6),
      CAMP_ID              NUMBER(6)
      );
commit;

comment on column IVR_EVR_DETAILS_DDMM.REF_ID  is 'Channel Id for call, Package Id for Package, Gift, Renew';
comment on column IVR_EVR_DETAILS_DDMM.REQUEST_MODE is 'Mode of Request - 0: IVR, 1: SMS, 2:USSD, 3: GUI';
comment on column IVR_EVR_DETAILS_DDMM.CALL_REF_NUM is 'Call Reference Number to track entire call events';
comment on column IVR_EVR_DETAILS_DDMM.CLD_NUM is 'Called party Number or DNIS';
comment on column IVR_EVR_DETAILS_DDMM.CLG_NUM is 'Calling party number or CLI';
comment on column IVR_EVR_DETAILS_DDMM.CONTENT_ID is 'holds Content_Id for Play/Pause/Stop event';
comment on column IVR_EVR_DETAILS_DDMM.CONTENT is 'holds either Content_file for Play/Pause/Stop event or CCXML/VXML document name used to answer the call';
comment on column IVR_EVR_DETAILS_DDMM.DURATION is 'Duration for which the contents were played or call duration for call completion event';
comment on column IVR_EVR_DETAILS_DDMM.CHARGE_UNIT is 'Charge Units: Amount-1, Min-2, Content-3, Subscription-4, Dedication-5, Other-6';
comment on column IVR_EVR_DETAILS_DDMM.GEN_TIME is 'EVR generation time';
commit;

/*==============================================================*/
/* Table: IVR_EVR_EVENT_MAST                                    */
/*==============================================================*/
create table IVR_EVR_EVENT_MAST  (
      EVR_EVENT            NUMBER(3)                       not null,
      DESCRIPTION          varchar(40)                     not null,
      DISPLAY              varchar(30)                     not null,
      constraint PK_IVR_EVR_EVENT_MAST primary key (EVR_EVENT)
      );

/*==============================================================*/
/* Table: IVR_EVR_REASON_MAST                                   */
/*==============================================================*/
create table IVR_EVR_REASON_MAST  (
      EVR_REASON           NUMBER(3)                       not null,
      DESCRIPTION          varchar(40)                     not null,
      DISPLAY              varchar(40)                     not null,
      constraint PK_IVR_EVR_REASON_MAST primary key (EVR_REASON)
      );

/*==============================================================*/
/* Table: IVR_SCHEDULE                                          */
/*==============================================================*/
create table IVR_SCHEDULE  (
      SCHEDULE_ID          NUMBER(6)                       not null,
      SCHEDULE_TYPE        NUMBER(3)                       not null,
      CALL_REF_NUM         varchar(20),
      EXT_REF_NUM          varchar(20),
      REQUEST_MODE         NUMBER(3),
      RECIPIENT_NO         varchar(20)                     not null,
      SENDER_NO            varchar(20)                     not null,
      APPL_NAME            varchar(20)                     not null,
      ENTRY_TIME           TIMESTAMP                       not null,
      SCHEDULE_TIME        TIMESTAMP,
      DEL_ATTEMPTS         NUMBER(3),
      STATUS               NUMBER(3),
      ERROR_ID             NUMBER(6),
      END_TIME             TIMESTAMP,
      RSP_REQ              NUMBER(1),
      FEEDBACK             varchar(10),
      APP_NOP              NUMBER(1),
      APP_PARAM1           varchar(50),
      APP_PARAM2           varchar(50),
      APP_PARAM3           varchar(50),
      APP_PARAM4           varchar(50),
      APP_PARAM5           varchar(50),
      CALL_TIME            TIMESTAMP,
      DURATION             NUMBER(6),
      constraint PK_IVR_SCHEDULE primary key (SCHEDULE_ID)
   );

/*==============================================================*/
/* Table: IVR_NOTIFICATION                                      */
/*==============================================================*/
create table IVR_NOTIFICATION  (
      NOTI_ID              NUMBER(6)                       not null,
      NOTI_EVENT           NUMBER(3)                       not null,
      CALL_REF_NUM         varchar(20),
      DEL_AGENT            NUMBER(3)                       not null,
      SUB_AGENT            NUMBER(3)                       not null,
      USER_ID              varchar(20)                     not null,
      TEXT                 varchar(200),
      NOTI_TIME            TIMESTAMP                       not null,
      DEL_TIME             TIMESTAMP                       not null,
      STATUS               NUMBER(3),
      constraint PK_IVR_NOTIFICATION primary key (NOTI_ID)
      );

/*==============================================================*/
/* Table: IVR_NOTIFY_EVENTS                                     */
/*==============================================================*/
create table IVR_NOTIFY_EVENTS  (
      NOTI_EVENT           NUMBER(3)                       not null,
      TEXT                 varchar(200)                    not null,
      SEVERITY             NUMBER(1),
      DESCRIPTION          varchar(200),
      DISPLAY              NUMBER(1),
      constraint PK_IVR_NOTIFY_EVENTS primary key (NOTI_EVENT)
      );


alter table IVR_APPLICATION
   add constraint FK_APPLICATION_PROVIDER_ID foreign key (PROVIDER_ID)
      references IVR_APP_PROVIDER (PROVIDER_ID);


alter table IVR_APPLICATION
   add constraint FK_APPL_CHARGE_ID foreign key (CHARGE_ID)
      references IVR_CHARGING_SLAB (CHARGE_ID);



alter table IVR_CHANNEL_CONFIG
   add constraint FK_CHANNEL_APPL_ID foreign key (APPL_ID)
      references IVR_APPLICATION (APPL_ID);


alter table IVR_CONFIG_DETAILS
   add constraint FK_CONFIG_KEY foreign key (CONFIG_KEY)
      references IVR_CONFIG_MAIN (CONFIG_KEY);


alter table IVR_EVR_DETAILS_DDMM
   add constraint FK_EVR_CAUSE foreign key (EVR_CAUSE)
      references IVR_EVR_CAUSE_MAST (EVR_CAUSE);


alter table IVR_EVR_DETAILS_DDMM
   add constraint FK_EVR_EVENT foreign key (EVR_EVENT)
      references IVR_EVR_EVENT_MAST (EVR_EVENT);


alter table IVR_EVR_DETAILS_DDMM
   add constraint FK_EVR_REASON foreign key (EVR_REASON)
      references IVR_EVR_REASON_MAST (EVR_REASON);


alter table IVR_NOTIFICATION
   add constraint FK_NOTIFICATION_EVENT foreign key (NOTI_EVENT)
      references IVR_NOTIFY_EVENTS (NOTI_EVENT);


alter table IVR_SCHEDULE
   add constraint FK_SCHEDULE_APPL_NAME foreign key (APPL_NAME)
      references IVR_APPLICATION (APPL_NAME);


alter table IVR_SHORT_CODE_MAST
   add constraint FK_SC_APPL_ID foreign key (APPL_ID)
      references IVR_APPLICATION (APPL_ID);
