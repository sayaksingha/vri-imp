--  @(#) $Id: IvrCreateTables.sql,v 1.1.4.1 2016/04/12 21:59:09 shijuc Exp $

drop table if exists IVR_APPLICATION;

drop table if exists IVR_APP_PROVIDER;

drop table if exists IVR_CDR_DETAILS_DDMM;

drop table if exists IVR_CHANNEL_COMMANDS;

drop table if exists IVR_CHANNEL_CONFIG;

drop table if exists IVR_CHARGING_SLAB;

drop table if exists IVR_CONFIG_DETAILS;

drop table if exists IVR_CONFIG_MAIN;

drop table if exists IVR_EVR_CAUSE_MAST;

drop table if exists IVR_EVR_DETAILS_DDMM;

drop table if exists IVR_EVR_EVENT_MAST;

drop table if exists IVR_EVR_REASON_MAST;

drop table if exists IVR_SCHEDULE;

drop table if exists IVR_NOTIFICATION;

drop table if exists IVR_NOTIFY_EVENTS;

drop table if exists IVR_SHORT_CODE_MAST;

drop table if exists IVR_SUBSCRIBER;

drop table if exists IVR_LANG_MAST;

/*==============================================================*/
/* Table: IVR_LANG_MAST                                         */
/*==============================================================*/
create table IVR_LANG_MAST
(
   LANG_ID      integer not null comment 'Unique Language Identifier',
   LANG         varchar(30) not null comment 'Language Name',
   STATUS       smallint not null default 0 comment 'Status of the language. 0: CREATED, 1: AVAILABLE, 2: BLOCKED, 3: DELETED',
   LANG_DIR     varchar(10) not null default 'EN' comment 'Language Directory Name to be used for IVR Application Builder',
   LANG_SORT_ORDER smallint not null default 0 comment 'Starts with 1 to sort the Language for display and index with 1 will be treated as default language',
   primary key (LANG_ID)
)
comment = "Language Table. This table will contain different Languages supported by IVR Platform for different IVR Applications.";

/*==============================================================*/
/* Table: IVR_SUBSCRIBER                                        */
/*==============================================================*/
create table IVR_SUBSCRIBER
(
   SUBS_NO              varchar(20) not null comment 'Subscriber Number',
   IMSI                 varchar(20),
   SUBS_NAME            varchar(30),
   SUBS_TYPE            smallint comment 'Subscriber Type 0: Prepaid, 1: Postpaid',
   SUBS_DOB             date,
   EMAIL_ID             varchar(50),
   PREF_LANG_ID         integer,
   SNS_TYPE             smallint comment 'SNS Type describes the subscribers SNS subscription type, 0: NONE, 1: FACEBOOK, 2: TWITTER, 3: BOTH',
   LOGIN_PWD            varchar(20),
   SERVICES             integer comment 'Bit wise information about the subscribed services.(Bit0 - NONE, 1 - MMP, 2 - VMS)',
   STATUS               smallint default 1 comment 'It maintains the status of the subscription. 0: ACTIVE, 1: IN-ACTIVE, 2: BLOCKED',
   ENTRY_DATE           datetime,
   primary key (SUBS_NO)
)
comment = "Subscriber Table holds information about the subscribers of IVR platform";

/*==============================================================*/
/* Table: IVR_APPLICATION                                       */
/*==============================================================*/
create table IVR_APPLICATION
(
   APPL_ID              integer not null auto_increment comment 'Application Id, Auto Sequence Number',
   PROVIDER_ID          integer comment 'Provider ID of Application Provider',
   APPL_NAME            varchar(20) not null,
   APPL_TYPE            integer comment 'Application Type- 1: Inbound, 2: Outbound',
   APPL_DESC            varchar(150),
   APPL_FILE            varchar(200),
   UPLOAD_DATE          datetime comment 'Date when application was uploaded',
   LAST_UPDATE_DATE     datetime comment 'Last update date for application',
   DELETED_DATE         datetime comment 'Date when application was deleted',
   DELETED_FLAG         char default 'N' comment 'It holds the status of application,whether it is deleted or still available.',
   TEST_CALL_CLI        varchar(20),
   CHARGE_ID            integer comment 'Charge Slab Id for Call charges per minute for application browsing, default is from system config',
   STATUS               smallint not null default 0 comment 'It maintains the status of the uploaded content. 0: UPLOADED, 1: BLOCKED, 2: TESTED, 3: AVAILABLE, 4: DELETED',
   REASON               varchar(50),
   primary key (APPL_ID),
   key AK_APPL_NAME (APPL_NAME)
)
comment = "Application table holds details about the Services deployed on IVR platform";

/*==============================================================*/
/* Table: IVR_SHOT_CODE_MAST                                    */
/*==============================================================*/
create table IVR_SHORT_CODE_MAST
(
   SHORT_CODE           varchar(20) not null comment 'Unique Short Code',
   APPL_ID              integer comment 'Application Id , this short code is associated with',
   DISPLAY              varchar(100) not null comment 'Display text/Description for shortcode',
   STATUS               smallint not null default 0 comment 'It maintains the status of the shortcode. 0: AVAILABLE, 1: BLOCKED, 2: DELETED',
   primary key (SHORT_CODE)
)
comment = "Application table holds details about the Services deployed on IVR platform";

/*==============================================================*/
/* Table: IVR_APP_PROVIDER                                      */
/*==============================================================*/
create table IVR_APP_PROVIDER
(
   PROVIDER_ID          integer not null comment 'Autogenerated Sequence Number',
   REVENUE_SHARE        float comment 'Revenue Share percentage with content providers',
   STATUS               integer comment 'Provider Flag - 0: None, 1:Active, 2:Deactivate, 3:Deleted',
   PARENT_PROVIDER_ID   integer,
   primary key (PROVIDER_ID)
)
comment = "Application Providers Master Information";

/*==============================================================*/
/* Table: IVR_CDR_DETAILS_DDMM                                  */
/*==============================================================*/
create table IVR_CDR_DETAILS_DDMM
(
   SUBS_NO              varchar(20) comment 'Subscriber Number who has been charged',
   SUBS_TYPE            smallint comment 'Subscriber Type 0: Prepaid, 1: Postpaid',
   CALL_REF_NUM         varchar(20) comment 'Call Reference Number',
   SERVICE_TYPE         smallint comment 'Service type tells the type of charge 0: SERVICE_CHARGE, 1: CONTENT_CHARGE, 3: PACKAGE_CHARGE, 4: DEDICATION_CHARGE, Service charge is online charge (charge per minute for IVR browsing), content/package/dedication charge is offline charges; 5:RENEW, 6:DEDICATE, 7:GIFT,',
   CONTENT              integer comment 'Content Id/Service Id/Package Id, for which subscriber is charged for',
   DURATION             integer comment 'Duration for which the contents were played or call duration for call completion event',
   UNITS                integer comment 'Charge unit',
   CHARGE               integer,
   INTERFACE            tinyint,
   REF_ID               integer comment 'REF ID becomes pkg-id in case of sub/renew',
   GEN_TIME             datetime comment 'EVR generation time'
);

/*==============================================================*/
/* Table: IVR_CHANNEL_COMMANDS                                  */
/*==============================================================*/
create table IVR_CHANNEL_COMMANDS
(
   COMMAND_ID           integer not null auto_increment comment 'Aotogenerated Sequence Number',
   CHANNEL_ID           integer not null,
   COMMAND              tinyint not null comment 'Command supported on the circuits: 0-UNKNOWN, 1-AVAILABLE,2-BLOCK, 3- UNBLOCK, 4 - RELEASE',
   TIME                 datetime,
   STATUS               smallint comment 'Indicates the status fo the channel : 0 - UNKNOWN, 1 - AVAILABLE, 2- BLOCKED, 3- UNBLOCKED, 4- RELEASED, 5 - IN_TRANSIT',
   PATCH_CHANNEL_ID     integer comment 'Specifies the patch channel Id when a patched call is to be released',
   primary key (COMMAND_ID)
);

/*==============================================================*/
/* Table: IVR_CHANNEL_CONFIG                                    */
/*==============================================================*/
create table IVR_CHANNEL_CONFIG
(
   CHANNEL_NO           integer not null,
   CHANNEL_TYPE         tinyint not null comment 'Channel Type- 0: UNKNOWN, 1: IN, 2: OUT, 3: IN-OUT',
   APPL_ID              integer comment 'Application Id ',
   STATUS               smallint default 0 comment 'Channel Status- 0: AVAILABLE, 1: BLOCKED, 2: BUSY',
   primary key (CHANNEL_NO)
)
comment = "Stores details of the channels (in/out), channel reservation details";

/*==============================================================*/
/* Table: IVR_CHARGING_SLAB                                     */
/*==============================================================*/
create table IVR_CHARGING_SLAB
(
   CHARGE_ID            integer not null auto_increment comment 'Autogenerated Sequence Number',
   SLAB_NAME            varchar(30) not null,
   AMOUNT_PER_MINUTE    numeric not null,
   SERVICE_KEY          integer not null,
   MSC_ADDRESS          varchar(20),
   DEST_NUMBER          varchar(20),
   SMSC_ADDRESS         varchar(20),
   VLR_NUMBER           varchar(20),
   ENTRY_TIME           datetime not null,
   primary key (CHARGE_ID)
);

/*==============================================================*/
/* Table: IVR_CONFIG_DETAILS                                    */
/*==============================================================*/
create table IVR_CONFIG_DETAILS
(
   CONFIG_KEY           varchar(50),
   VALUE                varchar(50),
   DISPLAY_NAME         varchar(50)
);

/*==============================================================*/
/* Table: IVR_CONFIG_MAIN                                       */
/*==============================================================*/
create table IVR_CONFIG_MAIN
(
   CONFIG_KEY           varchar(50) not null,
   CONFIG_TYPE          char,
   FIELD_TYPE           char,
   MIN_VAL              varchar(15),
   MAX_VAL              varchar(15),
   DEF_VAL              varchar(200),
   VALUE                varchar(200),
   GUI_TXT              varchar(200),
   HELP_TXT             varchar(500),
   MODIFIED_DATE        datetime,
   MODIFIED_USER        varchar(20),
   DISP_ORDER           int(4),
   GROUP_VAL            varchar(5),
   primary key (CONFIG_KEY)
)
comment = "All Kernal/System related configurations are stored.";

/*==============================================================*/
/* Table: IVR_EVR_CAUSE_MAST                                    */
/*==============================================================*/
create table IVR_EVR_CAUSE_MAST
(
   EVR_CAUSE            smallint not null,
   DESCRIPTION          varchar(40) not null,
   DISPLAY              varchar(30) not null,
   primary key (EVR_CAUSE)
);

/*==============================================================*/
/* Table: IVR_EVR_DETAILS_DDMM                                  */
/*==============================================================*/
create table IVR_EVR_DETAILS_DDMM
(
   EVR_EVENT            smallint,
   EVR_CAUSE            smallint,
   EVR_REASON           smallint,
   REF_ID               integer comment 'Channel Id for call, Package Id for Package, Gift, Renew',
   SHORT_CODE           varchar(20) not null,
   REQUEST_MODE         integer comment 'Mode of Request - 0: IVR, 1: SMS, 2:USSD, 3: GUI',
   CALL_REF_NUM         varchar(20) comment 'Call Reference Number to track entire call events',
   CLD_NUM              varchar(20) comment 'Called party Number or DNIS',
   CLG_NUM              varchar(20) comment 'Calling party number or CLI',
   CONTENT_ID           integer comment 'holds Content_Id for Play/Pause/Stop event',
   CONTENT              varchar(100) comment 'holds either Content_file for Play/Pause/Stop event or CCXML/VXML document name used to answer the call',
   DURATION             integer comment 'Duration for which the contents were played or call duration for call completion event',
   CHARGE_UNIT          smallint default 0 comment 'Charge Units: Amount-1, Min-2, Content-3, Subscription-4, Dedication-5, Other-6
            ',
   CHARGE               integer,
   GEN_TIME             datetime comment 'EVR generation time',
   SCHEDULE_ID          integer,
   CAMP_ID              integer
);

/*==============================================================*/
/* Table: IVR_EVR_EVENT_MAST                                    */
/*==============================================================*/
create table IVR_EVR_EVENT_MAST
(
   EVR_EVENT            smallint not null,
   DESCRIPTION          varchar(40) not null,
   DISPLAY              varchar(30) not null,
   primary key (EVR_EVENT)
);

/*==============================================================*/
/* Table: IVR_EVR_REASON_MAST                                   */
/*==============================================================*/
create table IVR_EVR_REASON_MAST
(
   EVR_REASON           smallint not null,
   DESCRIPTION          varchar(40) not null,
   DISPLAY              varchar(30) not null,
   primary key (EVR_REASON)
);

/*==============================================================*/
/* Table: IVR_SCHEDULE                                          */
/*==============================================================*/
create table IVR_SCHEDULE
(
   SCHEDULE_ID          integer not null auto_increment comment 'Autogenerated sequence number',
   SCHEDULE_TYPE        smallint not null comment 'Schedule Type tells the type of schedule- 0: DEDICATION, 1: REMINDER, 2: OTHER',
   CALL_REF_NUM         varchar(20),
   EXT_REF_NUM          varchar(20) comment 'External Reference number: reference number for the scheduling entity',
   REQUEST_MODE         smallint comment 'Mode of Request - 0: IVR, 1: SMS, 2:USSD, 3: GUI',
   RECIPIENT_NO         varchar(20) not null comment 'Subscriber number for whom dedication is for',
   SENDER_NO            varchar(20) not null,
   APPL_NAME            varchar(20) not null comment 'Short Code to identify Application for dialing out',
   ENTRY_TIME           datetime not null comment 'Entry time holds when content was dedicated',
   SCHEDULE_TIME        datetime comment 'Schedule Time holds the time of delivery, it can be deferred delivery or immediate delivery.',
   DEL_ATTEMPTS         smallint comment 'Total delivery attempts exercised for dedication delivery',
   STATUS               smallint comment 'Status tells. 0: SCHEDULED, 1: IN_PROCESS 2: DELIVERED, 3: TEMP_FAILURE, 4: FAILED',
   ERROR_ID             integer,
   END_TIME             datetime comment 'The time after which the schedule will not be attempted though there are valid attempts.',
   RSP_REQ              tinyint,
   FEEDBACK             varchar(10),
   APP_NOP              tinyint,
   APP_PARAM1           varchar(50),
   APP_PARAM2           varchar(50),
   APP_PARAM3           varchar(50),
   APP_PARAM4           varchar(50),
   APP_PARAM5           varchar(50),
   CALL_TIME            datetime,
   DURATION             integer,
   primary key (SCHEDULE_ID)
)
comment = "Schedule table holds the details of the outgoing calls to be dialled for subscribers, either for playing dedication or some other application(VXML)";

/*==============================================================*/
/* Table: IVR_NOTIFICATION                                      */
/*==============================================================*/
create table IVR_NOTIFICATION
(
   NOTI_ID              integer not null auto_increment comment 'Autogenerated sequence number',
   NOTI_EVENT           smallint not null comment 'Notification event. 0: NONE, 1: APPL_ADDED, 2: APPL_LIVE, 3: APPL_TESTED, 4: APPL_APPROVED, 5: APPL_BLOCKED, 6: APPL_DELETED, 11: CONTENT_ADDED, 12: CONTENT_BLOCKED, 13: CONTENT_LIVE, 14: CONTENT_DELETED, 21: PACKAGE_ADDED, 22: PACKAGE_BLOCKED, 23: PACKAGE_LIVE, 24: PACKAGE_DELETED 31: DEDICATION_PENDING, 41: SNS_UPDATE, ',
   CALL_REF_NUM         varchar(20),
   DEL_AGENT            smallint not null comment 'Delivery Agent describes how to deliver the Notification, i.e. via GUI/SMS/SNS/e-mail.',
   SUB_AGENT            smallint not null comment 'Submit Agent tells who has submitted the notification, i.e. ModuleID',
   USER_ID              varchar(20) not null comment 'User Id/Provider Id/Subscrier Id whom the notification belongs to.',
   TEXT                 varchar(200) comment 'Text to notify or Post on SNS, links in case of LIKE or SHARE on SNS',
   NOTI_TIME            datetime not null comment 'Notification Time tells when the notification was submitted',
   DEL_TIME             datetime not null comment 'Notification Delivery Time tells when to deliver the notification',
   STATUS               smallint comment 'Notification status. 0: SUBMITTED, 1: DELIVERED, 2: FAILED',
   primary key (NOTI_ID)
)
comment = "Notification table holds the event notifications for subscribers and content providers & Administrator";

/*==============================================================*/
/* Table: IVR_NOTIFY_EVENTS                                     */
/*==============================================================*/
create table IVR_NOTIFY_EVENTS
(
   NOTI_EVENT           smallint not null comment 'Notification event',
   TEXT                 varchar(200) not null comment 'Notification Text',
   SEVERITY             tinyint comment 'Notification Severity, 0: INFO, 1: WARNING, 2: CRITICAL',
   DESCRIPTION          varchar(200),
   DISPLAY              bool,
   primary key (NOTI_EVENT)
);

alter table IVR_APPLICATION add constraint FK_APPLICATION_PROVIDER_ID foreign key (PROVIDER_ID)
      references IVR_APP_PROVIDER (PROVIDER_ID) on delete restrict on update restrict;

alter table IVR_APPLICATION add constraint FK_APPL_CHARGE_ID foreign key (CHARGE_ID)
      references IVR_CHARGING_SLAB (CHARGE_ID) on delete restrict on update restrict;

alter table IVR_APP_PROVIDER add constraint FK_PROVIDER_USER_ID foreign key (PROVIDER_ID)
      references GUI_USER_DETAILS (USER_ID) on delete restrict on update restrict;

alter table IVR_CHANNEL_CONFIG add constraint FK_CHANNEL_APPL_ID foreign key (APPL_ID)
      references IVR_APPLICATION (APPL_ID) on delete restrict on update restrict;

alter table IVR_CONFIG_DETAILS add constraint FK_CONFIG_KEY foreign key (CONFIG_KEY)
      references IVR_CONFIG_MAIN (CONFIG_KEY) on delete restrict on update restrict;

alter table IVR_EVR_DETAILS_DDMM add constraint FK_EVR_CAUSE foreign key (EVR_CAUSE)
      references IVR_EVR_CAUSE_MAST (EVR_CAUSE) on delete restrict on update restrict;

alter table IVR_EVR_DETAILS_DDMM add constraint FK_EVR_EVENT foreign key (EVR_EVENT)
      references IVR_EVR_EVENT_MAST (EVR_EVENT) on delete restrict on update restrict;

alter table IVR_EVR_DETAILS_DDMM add constraint FK_EVR_REASON foreign key (EVR_REASON)
      references IVR_EVR_REASON_MAST (EVR_REASON) on delete restrict on update restrict;

alter table IVR_SCHEDULE add constraint FK_SCHEDULE_APPL_NAME foreign key (APPL_NAME)
      references IVR_APPLICATION (APPL_NAME) on delete restrict on update restrict;

alter table IVR_NOTIFICATION add constraint FK_NOTIFICATION_EVENT foreign key (NOTI_EVENT)
      references IVR_NOTIFY_EVENTS (NOTI_EVENT) on delete restrict on update restrict;

alter table IVR_SHORT_CODE_MAST add constraint FK_SC_APPL_ID foreign key (APPL_ID)
      references IVR_APPLICATION (APPL_ID) on delete restrict on update restrict;

