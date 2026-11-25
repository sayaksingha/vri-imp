--  @(#) $Id: IvrInsertDefault.sql,v 1.1.4.1.2.2 2016/05/12 01:21:36 mram Exp $
#---------------------------------------------------------------------
#-- IVR_CONFIG_MAIN -- IVR Config Values                             -
#---------------------------------------------------------------------

INSERT INTO IVR_CONFIG_MAIN (CONFIG_KEY, CONFIG_TYPE, FIELD_TYPE, MIN_VAL, MAX_VAL, DEF_VAL, VALUE, GUI_TXT, HELP_TXT, MODIFIED_DATE, MODIFIED_USER, DISP_ORDER,GROUP_VAL) VALUES
('KER_MMP_LICENSE', 'S', 'T', '0', '1', '0', '1', 'MMP License Enabled', 'Describes the License Key.(0-Disable, 1-Enable)', sysdate(), 'admin', 0,1),
('KER_RING_DURATION', 'N', 'T', '1', '30', '2', '2', 'Ring Duration Before Call Answered', 'Describes the initial ring duration before answering the call',  sysdate(), 'admin', 0,1),
('SYS_ALL_LOGS_REQUIRED', 'N', 'L', '0', '1', '1', '1', 'Detail Logs Required', 'All logs required (0-Disabled, 1-Enabled)',  sysdate(), 'admin', 0,1),
('SYS_CHARGING_FUNCTION_TYPE', 'N', 'L', '0', '2', '0', '0', 'Charging Function Type', 'Describes the System charging Information (0-None, 1-Duration, 2-Content)',  sysdate(), 'admin', 0,1),
('SYS_SCHEDULE_RETRY_ATTEMPTS', 'N', 'T', '1', '5', '3', '3', 'Schedule Retry Attempts', 'Describes the Schedule Retry attempts count',  sysdate(), 'admin', 0,1),
('SYS_SCHEDULE_RETRY_TIME', 'N', 'T', '1', '60', '30', '30', 'Schedule Retry Interval', 'Describes the Schedule Retry interval(in Seconds)',  sysdate(), 'admin', 0,1);

COMMIT;

-- -------------------------------------------------------------------
-- IVR_CONFIG_DETAILS                                                -
-- -------------------------------------------------------------------
INSERT INTO IVR_CONFIG_DETAILS (CONFIG_KEY, VALUE, DISPLAY_NAME) VALUES
('SYS_CHARGING_FUNCTION_TYPE', '0', 'None'), 
('SYS_CHARGING_FUNCTION_TYPE', '1', 'Duration Based'), 
('SYS_CHARGING_FUNCTION_TYPE', '2', 'Content Based');

INSERT INTO IVR_CONFIG_DETAILS (CONFIG_KEY, VALUE, DISPLAY_NAME) VALUES
('SYS_ALL_LOGS_REQUIRED', '0', 'Disabled'),
('SYS_ALL_LOGS_REQUIRED', '1', 'Enabled');

-- -------------------------------------------------------------------
-- IVR_LANG_MAST                                                -
-- -------------------------------------------------------------------
INSERT INTO IVR_LANG_MAST(LANG_ID, LANG, STATUS, LANG_DIR, LANG_SORT_ORDER)
   VALUES (0, 'Unicode English', 1, 'english', 0);

INSERT INTO IVR_LANG_MAST(LANG_ID, LANG, STATUS, LANG_DIR, LANG_SORT_ORDER)
   VALUES (1, 'English', 1, 'english', 1);

INSERT INTO IVR_LANG_MAST(LANG_ID, LANG, STATUS, LANG_DIR, LANG_SORT_ORDER)
   VALUES (2, 'Creole', 0, 'creole', 0);

INSERT INTO IVR_LANG_MAST(LANG_ID, LANG, STATUS, LANG_DIR, LANG_SORT_ORDER)
   VALUES (3, 'Divehi', 1, 'divehi', 0);

#---------------------------------------------------------------------
#-- IVR_EVR_EVENT_MAST                                               -
#---------------------------------------------------------------------
INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES (0, 'Unknown Event', 'NO_EVT');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES (1, 'Incoming Call', 'EVT_IN_CALL');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES (2, 'Outgoing Call', 'EVT_OUT_CALL');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES (3, 'Play', 'EVT_EXT_PLAY');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES (4, 'Stop', 'EVT_EXT_STOP');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES (5, 'Ext Patch', 'EVT_EXT_PATCH');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES (6, 'Tear', 'EVT_TEAR');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES (7, 'Schedule Call', 'EVT_SCHEDULE_CALL');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES (8, 'CCXML', 'EVT_CCXML');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES (9, 'VXML', 'EVT_VXML');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(10,'Media Play','EVT_MEDIA_PLAY');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(11,'Media Stop','EVT_MEDIA_STOP');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(12,'Media Patch','EVT_MEDIA_PATCH');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(13,'Media Tear','EVT_MEDIA_TEAR');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(14,'Media DTMF','EVT_MEDIA_DTMF');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(15,'MMP Notification','EVT_NOTIFICATION');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(16,'MMP Dedication','EVT_DEDICATION');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(17,'MMP Gift','EVT_GIFT');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(18,'MMP Package Subscription','EVT_PACKAGE_SUB');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(19,'MMP Package Unsubscription','EVT_PACKAGE_UNSUB');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(20,'MMP Package Renewal','EVT_RENEW');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(21,'MMP Daily Subscription','EVT_DAILY_SUB');

INSERT INTO IVR_EVR_EVENT_MAST(EVR_EVENT, DISPLAY, DESCRIPTION)
VALUES(22,'MMP Daily Renewal','EVT_DAILY_RENEW');

COMMIT;

#--------------------------------------------------------------------------------
#-- IVR_EVR_CAUSE_MAST
#--------------------------------------------------------------------------------
INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (0, '', 'IVR_NONE');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (1, 'Incoming Call Received', 'IVR_CALL_RECEIVED');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (2, 'Call Dialled', 'IVR_CALL_DIALLED');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (3, 'Call Answered', 'IVR_CALL_ANSWERED');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (4, 'Call Disconnected', 'IVR_CALL_DISCONNECTED');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (5, 'Call Rejected', 'IVR_CALL_REJECTED');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (6, 'Ext Request Received', 'IVR_EXT_REQ_RECEIVED');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (7, 'Ext Request Complete', 'IVR_EXT_REQ_COMPLETE');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (8, 'Ext Request Failed', 'IVR_EXT_REQ_FAILED');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (9, 'Request Submited', 'IVR_REQ_SUBMITTED');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (10, 'Request Received', 'IVR_REQ_RECEIVED');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (11, 'Request Complete', 'IVR_REQ_COMPLETE');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (12, 'Request Failed', 'IVR_REQ_FAILED');

INSERT INTO IVR_EVR_CAUSE_MAST(EVR_CAUSE, DISPLAY, DESCRIPTION)
VALUES (13, 'Call Completed', 'CAS_IVR_CALL_COMPLETE');

   COMMIT;

#--------------------------------------------------------------------------------
#-- IVR_EVR_REASON_MAST
#--------------------------------------------------------------------------------
INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (0, 'Unknown', 'NONE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (20, 'Nested Internal Routing', 'NESTED_ROUTING');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (21, 'IVR Call Busy', 'CALL_BUSY');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (22, 'IVR Call Not Reachable', 'CALL_NOT_REACHABLE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (23, 'Call No Reply', 'CALL_NO_REPLY');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (24, 'Call Barred', 'CALL_BARRED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (25, 'Called Invalid Destination Number', 'CALL_INVALID_DEST_NUMBER');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (26, 'Call Application InActive', 'CALL_APP_NOT_ACTIVE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (27, 'EXT Request Invalid Channel', 'EXT_REQ_INVALID_CHANNEL');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (28, 'EXT Request Invalid Application', 'EXT_REQ_INVAILD_APP');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (29,   'EXT Request Resource Unavailable',
         'EXT_REQ_RESOURCE_UNAVAILABLE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (30,
         'Scheduled Request Channel Unavailable',
         'SCH_REQ_CHANNEL_UNAVAILABLE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (31, 'Scheduled Request Invalid Application', 'SCH_REQ_INVALID_APP');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (32, 'Scheduled Request InActive Application', 'SCH_REQ_INACTIVE_APP');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (33,
         'Scheduled Request Resource Unavailable',
         'SCH_REQ_RESOURCE_UNAVAILABLE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (34, 'CCXML Invalid Url', 'CCXML_INVAILD_URL');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (35, 'CCXML Invalid Document', 'CCXML_INVALID_DOC');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (36, 'CCXML Resource Unavailable', 'CCXML_RESOURCE_UNAVAILABLE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (37, 'VXML Invalid Url', 'VXML_INVAILD_URL');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (38, 'VXML Invalid Document', 'VXML_INVALID_DOC');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (39, 'VXML Resource Unavailable', 'VXML_RESOURCE_UNAVAILABLE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (40, 'Media Request Invalid File', 'MEDIA_REQ_INVALID_FILE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (41, 'Media Request Invalid Stream', 'MEDIA_REQ_INVALID_STREAM');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (42, 'Media Request Invalid Format', 'MEDIA_REQ_INVALID_FORMAT');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (43, 'Media Request Invalid State', 'MEDIA_REQ_INVALID_STATE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (44,
         'Media Request Resource Unavailable ',
         'MEDIA_REQ_RESOURCE_UNAVAILABLE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (45, 'SMS Failure Notification', 'NOTIFICATION_SMS_FAILED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (46,
         'Invalid SNS Application Notification',
         'NOTIFICATION_SNS_INVALID_APP');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (47,
         'No SNS Credentials Notifications',
         'NOTIFICATION_SNS_NO_CREDENTIAL');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (48,
         'Invalid SNS Credentials Notification',
         'NOTIFICATION_SNS_INVALID_CREDE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (49, 'Dedication Charging Failed', 'DEDICATION_CHARGING_FAILED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (50, 'Dedication Application Not Active', 'DEDICATION_APP_NOT_ACTIVE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (51, 'Dedication Attempts Exceeded', 'DEDICATION_ATTEMPTS_EXCEEDS');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (52, 'Dedication Expired', 'DEDICATION_EXPIRED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (53, 'Call Dedication Failed', 'DEDICATION_CALL_FAILED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (54, 'Invalid Gift Subscriber', 'GIFT_REQ_INVALID_RECEIPENT');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (55, 'Invalid Gift Sending Subscriber', 'GIFT_REQ_INVALID_SENDER');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (61, 'Tone Dedication Success', 'DEDICATE_SUCCESS');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (62, 'Package Gifting Success', 'GIFT_SUCCESS');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (63, 'Loyalty', 'LOYALTY_SUCCESS');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (64, 'Package Registration Success', 'PACKAGE_REG_SUCCESS');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (65, 'Package Change Success', 'PACKAGE_CHANGE_SUCCESS');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (66, 'Package Deleted', 'PACKAGE_DELETED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (67, 'Package Charging Failed', 'PACKAGE_CHARGE_FAILED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (68, 'Package Limit Reached', 'PACKAGE_QUOTA_EXCEED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (69, 'Package Already Subscribed', 'PACKAGE_ALREADY_SUBSCRIBED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (70, 'Not Registered to Package', 'PACKAGE_NOT_REGISTERED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (71, 'Package Not Available', 'PAKCAGE_NOT_AVAILABLE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (72, 'Packages is Currently Disabled.', 'PACKAGE_DISABLED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (73, 'Found Multiple PackageId in Received req', 'MULTIPLE_PACKAGE_IN_REQ');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (74, 'Package Expired', 'PACKAGE_EXPIRED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (76, 'Subscription Type is changed', 'SUBS_TYPE_CHANGE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (77, 'Package Renewal Success', 'PACKAGE_RENEW_SUCCESS');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (78, 'Already Renewed Package', 'PACKAGE_ALREADY_RENEWED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (79, 'Auto Renewal of Package Disabled', 'AUTO_RENEW_DISABLED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (81, 'Package UnRegistartion Success', 'PACKAGE_UNREG_SUCCESS');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (85, 'DailyBilling Success', 'DAILYBILL_SUCCESS');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (86, 'Provisional Validation Success', 'PROV_VAL_SUCCESS');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (88, 'Invalid Content Id for Dedication', 'DEDICATE_INVALID_CONTENT_ID');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (89, 'Request rejected', 'REQ_REJECTED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (91, 'Call Busy', 'SYSTEM_ERR');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (92, 'Database Error', 'DB_ERR');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (93, 'Invalid Data', 'INVALID_DATA');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (94, 'Invalid Package', 'INVALID_PACKAGE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (95, 'Failed to get Balance Amount', 'GET_COS_FAILED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (96, 'Number of Data Received is more / less t', 'NUMBER_OF_DATA_ERR');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (97, 'Mandatory Data missing', 'MANDATORY_DATA_MISSING');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (98, 'Msisdn found in BlackList', 'MSISDN_IN_DEDICTD_BL');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (99, 'Found Multiple ToneId in Received reques', 'MULTIPLE_TONEID_IN_REQ');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (100, 'Multiple Subscription Feature Disabled', 'MUTIPLE_GIFTS_DISABLED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (101, 'Nill Balance', 'NILL_BALANCE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (102, 'Configuration Error,while Billing', 'BILLING_CFG_ERR');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (103, 'Billing Failed', 'BILLING_FAILURE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (104, 'Insufficient Balance', 'INSUFFICIENT_BALANCE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (105, 'Blocked Subscriber', 'BLOCKED_SUBSCRIBER');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (106, 'Still In Validity', 'STILL_IN_VALIDITY_LIMIT');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (107, 'InSufficient Balance for Daily', 'SUFF_BAL_DAILY_FAIL');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (108, 'Registration for Daily Billing Success', 'REG_DAILY_BILL_SUC');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (109, 'Subscription Type Get Fails', 'GET_SUBS_TYPE_FAIL');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (110, 'Subscriber Not Registered', 'SUBSCRIBER_NOT_REGISTERED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (111, 'Number Normalisation Fails', 'NUMBER_NORM_FAILED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (112, 'Special Characters Found in Request', 'SPECAIL_CHARS_PRESENT');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (113, 'Gift Number Registered', 'GIFT_NUMBER_REGISTERED');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (114, 'UnAuthorized Interface', 'UNAUTHORISED_INTERFACE');

INSERT INTO IVR_EVR_REASON_MAST(EVR_REASON, DISPLAY, DESCRIPTION)
   VALUES (115, 'Daily Bill Disabled', 'DAILY_BILL_DISABLED');

   COMMIT;


#-- ------------------------------------------------------------------
#--               Table : IVR_APP_PROVIDER                           -
#-- ------------------------------------------------------------------

INSERT INTO IVR_APP_PROVIDER
(PROVIDER_ID, REVENUE_SHARE, STATUS, PARENT_PROVIDER_ID) VALUES
(1, 1, 0, 0);

COMMIT;

#-- ------------------------------------------------------------------
#--               Table : IVR_APPLICATION                            -
#-- ------------------------------------------------------------------
INSERT INTO IVR_APPLICATION
 (APPL_ID, PROVIDER_ID, APPL_NAME, APPL_TYPE, APPL_DESC, APPL_FILE, UPLOAD_DATE, LAST_UPDATE_DATE, DELETED_DATE, DELETED_FLAG, TEST_CALL_CLI, CHARGE_ID, STATUS) VALUES
 (-1, 1, 'Dummy', 1, 'Dummy default application', NULL, sysdate(), sysdate(), NULL, 'N', NULL, NULL, 4);

 INSERT INTO IVR_APPLICATION
 (PROVIDER_ID, APPL_NAME, APPL_TYPE, APPL_DESC, APPL_FILE, UPLOAD_DATE, LAST_UPDATE_DATE, DELETED_DATE, DELETED_FLAG, TEST_CALL_CLI, CHARGE_ID, STATUS) VALUES
 (1, 'ivrapp', 1, 'Sample Incoming Application', NULL, sysdate(), sysdate(), NULL, 'N', NULL, 1, 3);

INSERT INTO IVR_SHORT_CODE_MAST
(SHORT_CODE, APPL_ID, DISPLAY, STATUS) VALUES
('1111', 1, 'Sample', 0);

#-- ------------------------------------------------------------------
#--               Table : IVR_CHARGING_SLAB                          -
#-- ------------------------------------------------------------------

INSERT INTO IVR_CHARGING_SLAB(CHARGE_ID, SLAB_NAME, AMOUNT_PER_MINUTE, SERVICE_KEY, MSC_ADDRESS, DEST_NUMBER, SMSC_ADDRESS, VLR_NUMBER, ENTRY_TIME) 
   VALUES (1, 'Dummy', 0, 0, NULL, NULL, NULL, NULL, sysdate());

COMMIT;

#--------------------------------------------------------------------------------
#-- IVR_NOTIFY_EVENTS
#--------------------------------------------------------------------------------
INSERT into IVR_NOTIFY_EVENTS(NOTI_EVENT,TEXT,SEVERITY,DESCRIPTION,DISPLAY) VALUES (0,'NONE',0,'None',0);
INSERT into IVR_NOTIFY_EVENTS(NOTI_EVENT,TEXT,SEVERITY,DESCRIPTION,DISPLAY) VALUES (1,'APPL_ADDED',0,'Application Added',1);
INSERT into IVR_NOTIFY_EVENTS(NOTI_EVENT,TEXT,SEVERITY,DESCRIPTION,DISPLAY) VALUES (2,'APPL_LIVE',0,'Application in Live',1);
INSERT into IVR_NOTIFY_EVENTS(NOTI_EVENT,TEXT,SEVERITY,DESCRIPTION,DISPLAY) VALUES (3,'APPL_TESTED',0,'Application tested ',1);
INSERT into IVR_NOTIFY_EVENTS(NOTI_EVENT,TEXT,SEVERITY,DESCRIPTION,DISPLAY) VALUES (4,'APPL_APPROVED',0,'Application Approved',1);
INSERT into IVR_NOTIFY_EVENTS(NOTI_EVENT,TEXT,SEVERITY,DESCRIPTION,DISPLAY) VALUES (5,'APPL_BLOCKED',1,'Application Blocked',1);
INSERT into IVR_NOTIFY_EVENTS(NOTI_EVENT,TEXT,SEVERITY,DESCRIPTION,DISPLAY) VALUES (6,'APPL_DELETED',0,'Application Deleted',1);

COMMIT;

#-- ------------------------------------------------------------------
#--               Table : IVR_CHANNEL_CONFIG                         -
#-- ------------------------------------------------------------------
insert into IVR_CHANNEL_CONFIG
values      (1, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (2, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (3, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (4, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (5, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (6, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (7, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (8, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (9, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (10, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (11, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (12, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (13, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (14, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (15, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (16, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (17, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (18, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (19, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (20, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (21, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (22, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (23, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (24, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (25, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (26, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (27, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (28, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (29, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (30, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (31, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (32, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (33, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (34, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (35, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (36, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (37, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (38, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (39, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (40, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (41, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (42, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (43, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (44, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (45, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (46, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (47, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (48, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (49, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (50, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (51, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (52, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (53, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (54, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (55, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (56, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (57, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (58, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (59, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (60, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (61, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (62, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (63, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (64, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (65, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (66, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (67, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (68, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (69, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (70, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (71, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (72, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (73, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (74, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (75, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (76, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (77, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (78, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (79, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (80, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (81, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (82, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (83, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (84, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (85, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (86, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (87, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (88, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (89, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (90, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (91, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (92, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (93, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (94, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (95, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (96, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (97, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (98, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (99, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (100, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (101, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (102, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (103, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (104, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (105, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (106, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (107, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (108, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (109, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (110, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (111, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (112, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (113, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (114, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (115, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (116, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (117, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (118, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (119, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (120, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (121, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (122, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (123, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (124, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (125, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (126, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (127, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (128, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (129, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (130, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (131, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (132, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (133, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (134, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (135, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (136, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (137, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (138, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (139, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (140, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (141, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (142, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (143, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (144, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (145, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (146, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (147, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (148, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (149, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (150, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (151, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (152, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (153, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (154, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (155, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (156, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (157, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (158, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (159, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (160, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (161, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (162, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (163, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (164, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (165, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (166, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (167, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (168, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (169, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (170, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (171, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (172, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (173, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (174, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (175, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (176, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (177, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (178, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (179, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (180, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (181, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (182, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (183, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (184, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (185, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (186, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (187, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (188, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (189, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (190, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (191, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (192, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (193, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (194, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (195, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (196, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (197, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (198, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (199, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (200, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (201, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (202, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (203, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (204, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (205, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (206, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (207, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (208, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (209, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (210, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (211, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (212, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (213, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (214, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (215, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (216, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (217, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (218, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (219, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (220, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (221, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (222, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (223, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (224, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (225, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (226, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (227, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (228, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (229, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (230, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (231, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (232, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (233, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (234, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (235, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (236, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (237, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (238, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (239, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (240, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (241, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (242, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (243, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (244, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (245, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (246, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (247, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (248, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (249, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (250, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (251, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (252, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (253, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (254, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (255, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (256, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (257, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (258, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (259, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (260, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (261, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (262, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (263, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (264, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (265, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (266, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (267, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (268, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (269, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (270, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (271, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (272, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (273, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (274, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (275, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (276, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (277, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (278, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (279, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (280, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (281, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (282, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (283, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (284, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (285, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (286, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (287, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (288, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (289, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (290, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (291, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (292, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (293, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (294, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (295, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (296, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (297, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (298, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (299, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (300, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (301, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (302, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (303, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (304, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (305, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (306, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (307, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (308, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (309, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (310, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (311, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (312, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (313, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (314, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (315, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (316, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (317, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (318, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (319, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (320, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (321, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (322, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (323, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (324, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (325, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (326, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (327, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (328, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (329, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (330, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (331, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (332, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (333, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (334, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (335, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (336, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (337, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (338, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (339, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (340, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (341, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (342, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (343, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (344, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (345, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (346, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (347, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (348, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (349, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (350, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (351, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (352, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (353, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (354, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (355, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (356, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (357, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (358, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (359, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (360, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (361, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (362, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (363, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (364, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (365, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (366, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (367, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (368, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (369, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (370, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (371, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (372, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (373, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (374, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (375, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (376, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (377, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (378, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (379, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (380, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (381, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (382, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (383, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (384, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (385, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (386, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (387, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (388, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (389, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (390, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (391, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (392, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (393, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (394, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (395, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (396, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (397, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (398, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (399, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (400, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (401, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (402, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (403, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (404, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (405, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (406, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (407, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (408, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (409, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (410, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (411, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (412, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (413, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (414, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (415, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (416, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (417, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (418, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (419, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (420, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (421, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (422, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (423, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (424, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (425, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (426, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (427, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (428, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (429, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (430, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (431, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (432, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (433, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (434, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (435, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (436, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (437, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (438, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (439, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (440, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (441, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (442, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (443, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (444, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (445, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (446, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (447, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (448, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (449, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (450, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (451, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (452, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (453, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (454, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (455, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (456, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (457, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (458, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (459, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (460, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (461, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (462, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (463, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (464, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (465, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (466, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (467, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (468, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (469, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (470, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (471, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (472, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (473, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (474, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (475, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (476, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (477, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (478, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (479, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (480, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (481, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (482, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (483, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (484, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (485, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (486, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (487, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (488, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (489, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (490, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (491, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (492, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (493, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (494, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (495, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (496, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (497, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (498, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (499, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (500, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (501, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (502, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (503, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (504, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (505, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (506, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (507, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (508, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (509, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (510, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (511, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (512, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (513, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (514, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (515, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (516, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (517, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (518, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (519, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (520, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (521, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (522, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (523, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (524, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (525, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (526, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (527, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (528, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (529, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (530, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (531, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (532, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (533, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (534, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (535, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (536, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (537, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (538, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (539, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (540, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (541, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (542, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (543, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (544, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (545, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (546, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (547, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (548, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (549, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (550, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (551, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (552, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (553, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (554, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (555, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (556, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (557, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (558, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (559, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (560, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (561, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (562, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (563, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (564, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (565, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (566, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (567, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (568, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (569, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (570, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (571, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (572, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (573, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (574, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (575, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (576, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (577, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (578, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (579, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (580, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (581, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (582, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (583, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (584, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (585, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (586, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (587, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (588, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (589, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (590, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (591, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (592, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (593, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (594, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (595, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (596, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (597, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (598, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (599, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (600, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (601, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (602, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (603, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (604, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (605, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (606, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (607, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (608, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (609, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (610, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (611, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (612, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (613, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (614, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (615, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (616, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (617, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (618, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (619, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (620, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (621, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (622, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (623, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (624, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (625, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (626, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (627, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (628, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (629, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (630, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (631, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (632, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (633, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (634, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (635, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (636, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (637, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (638, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (639, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (640, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (641, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (642, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (643, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (644, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (645, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (646, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (647, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (648, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (649, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (650, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (651, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (652, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (653, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (654, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (655, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (656, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (657, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (658, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (659, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (660, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (661, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (662, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (663, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (664, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (665, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (666, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (667, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (668, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (669, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (670, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (671, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (672, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (673, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (674, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (675, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (676, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (677, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (678, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (679, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (680, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (681, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (682, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (683, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (684, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (685, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (686, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (687, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (688, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (689, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (690, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (691, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (692, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (693, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (694, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (695, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (696, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (697, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (698, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (699, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (700, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (701, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (702, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (703, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (704, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (705, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (706, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (707, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (708, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (709, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (710, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (711, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (712, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (713, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (714, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (715, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (716, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (717, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (718, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (719, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (720, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (721, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (722, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (723, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (724, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (725, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (726, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (727, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (728, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (729, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (730, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (731, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (732, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (733, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (734, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (735, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (736, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (737, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (738, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (739, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (740, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (741, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (742, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (743, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (744, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (745, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (746, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (747, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (748, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (749, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (750, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (751, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (752, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (753, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (754, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (755, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (756, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (757, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (758, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (759, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (760, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (761, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (762, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (763, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (764, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (765, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (766, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (767, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (768, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (769, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (770, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (771, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (772, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (773, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (774, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (775, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (776, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (777, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (778, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (779, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (780, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (781, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (782, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (783, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (784, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (785, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (786, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (787, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (788, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (789, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (790, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (791, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (792, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (793, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (794, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (795, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (796, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (797, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (798, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (799, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (800, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (801, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (802, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (803, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (804, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (805, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (806, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (807, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (808, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (809, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (810, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (811, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (812, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (813, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (814, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (815, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (816, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (817, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (818, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (819, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (820, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (821, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (822, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (823, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (824, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (825, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (826, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (827, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (828, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (829, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (830, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (831, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (832, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (833, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (834, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (835, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (836, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (837, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (838, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (839, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (840, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (841, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (842, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (843, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (844, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (845, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (846, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (847, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (848, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (849, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (850, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (851, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (852, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (853, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (854, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (855, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (856, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (857, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (858, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (859, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (860, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (861, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (862, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (863, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (864, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (865, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (866, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (867, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (868, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (869, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (870, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (871, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (872, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (873, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (874, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (875, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (876, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (877, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (878, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (879, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (880, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (881, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (882, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (883, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (884, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (885, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (886, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (887, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (888, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (889, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (890, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (891, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (892, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (893, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (894, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (895, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (896, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (897, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (898, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (899, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (900, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (901, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (902, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (903, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (904, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (905, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (906, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (907, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (908, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (909, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (910, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (911, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (912, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (913, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (914, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (915, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (916, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (917, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (918, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (919, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (920, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (921, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (922, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (923, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (924, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (925, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (926, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (927, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (928, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (929, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (930, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (931, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (932, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (933, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (934, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (935, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (936, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (937, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (938, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (939, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (940, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (941, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (942, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (943, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (944, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (945, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (946, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (947, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (948, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (949, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (950, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (951, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (952, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (953, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (954, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (955, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (956, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (957, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (958, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (959, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (960, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (961, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (962, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (963, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (964, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (965, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (966, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (967, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (968, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (969, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (970, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (971, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (972, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (973, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (974, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (975, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (976, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (977, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (978, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (979, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (980, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (981, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (982, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (983, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (984, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (985, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (986, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (987, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (988, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (989, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (990, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (991, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (992, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (993, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (994, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (995, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (996, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (997, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (998, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (999, 1, -1, 0);

insert into IVR_CHANNEL_CONFIG
values      (1000, 1, -1, 0);
commit;
