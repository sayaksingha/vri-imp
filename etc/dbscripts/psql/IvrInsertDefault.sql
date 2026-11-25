-- IVR_CONFIG_MAIN -- IVR Config Values

INSERT INTO ivr_config_main 
(config_key, config_type, field_type, min_val, max_val, def_val, value, gui_txt, help_txt, modified_date, modified_user, disp_order, group_val) 
VALUES
('KER_MMP_LICENSE', 'S', 'T', '0', '1', '0', '1', 'MMP License Enabled', 'Describes the License Key.(0-Disable, 1-Enable)', CURRENT_TIMESTAMP, 'admin', 1, 1);

INSERT INTO ivr_config_main 
(config_key, config_type, field_type, min_val, max_val, def_val, value, gui_txt, help_txt, modified_date, modified_user, disp_order, group_val) 
VALUES
('KER_RING_DURATION', 'N', 'T', '0', '30', '2', '0', 'Ring Duration Before Call Answered', 'Describes the initial ring duration before answering the call', CURRENT_TIMESTAMP, 'admin', 1, 1);

INSERT INTO ivr_config_main 
(config_key, config_type, field_type, min_val, max_val, def_val, value, gui_txt, help_txt, modified_date, modified_user, disp_order, group_val) 
VALUES
('SYS_ALL_LOGS_REQUIRED', 'N', 'L', '0', '1', '1', '1', 'Detail Logs Required', 'All logs required (0-Disabled, 1-Enabled)', CURRENT_TIMESTAMP, 'admin', 1, 1);

INSERT INTO ivr_config_main 
(config_key, config_type, field_type, min_val, max_val, def_val, value, gui_txt, help_txt, modified_date, modified_user, disp_order, group_val) 
VALUES
('SYS_CHARGING_FUNCTION_TYPE', 'N', 'L', '0', '2', '0', '0', 'Charging Function Type', 'Describes the System charging Information (0-None, 1-Duration, 2-Content)', CURRENT_TIMESTAMP, 'admin', 1, 1);

INSERT INTO ivr_config_main 
(config_key, config_type, field_type, min_val, max_val, def_val, value, gui_txt, help_txt, modified_date, modified_user, disp_order, group_val) 
VALUES
('SYS_SCHEDULE_RETRY_ATTEMPTS', 'N', 'T', '1', '5', '3', '3', 'Schedule Retry Attempts', 'Describes the Schedule Retry attempts count', CURRENT_TIMESTAMP, 'admin', 1, 1);

INSERT INTO ivr_config_main 
(config_key, config_type, field_type, min_val, max_val, def_val, value, gui_txt, help_txt, modified_date, modified_user, disp_order, group_val) 
VALUES
('SYS_SCHEDULE_RETRY_TIME', 'N', 'T', '1', '60', '30', '30', 'Schedule Retry Interval', 'Describes the Schedule Retry interval(in Seconds)', CURRENT_TIMESTAMP, 'admin', 1, 1);


-- -------------------------------------------------------------------
-- ivr_config_details
-- -------------------------------------------------------------------

INSERT INTO ivr_config_details (config_key, value, display_name) VALUES ('SYS_CHARGING_FUNCTION_TYPE', '0', 'None');
INSERT INTO ivr_config_details (config_key, value, display_name) VALUES ('SYS_CHARGING_FUNCTION_TYPE', '1', 'Duration Based');
INSERT INTO ivr_config_details (config_key, value, display_name) VALUES ('SYS_CHARGING_FUNCTION_TYPE', '2', 'Content Based');

INSERT INTO ivr_config_details (config_key, value, display_name) VALUES ('SYS_ALL_LOGS_REQUIRED', '0', 'Disabled');
INSERT INTO ivr_config_details (config_key, value, display_name) VALUES ('SYS_ALL_LOGS_REQUIRED', '1', 'Enabled');


-- -------------------------------------------------------------------
-- ivr_lang_mast (Converted on 2025-06-12)
-- -------------------------------------------------------------------

INSERT INTO ivr_lang_mast (lang_id, lang, status, lang_dir, lang_sort_order)
VALUES (0, 'Unicode English', 1, 'english', 0);

INSERT INTO ivr_lang_mast (lang_id, lang, status, lang_dir, lang_sort_order)
VALUES (1, 'English', 1, 'english', 1);

INSERT INTO ivr_lang_mast (lang_id, lang, status, lang_dir, lang_sort_order)
VALUES (2, 'Creole', 0, 'creole', 0);

INSERT INTO ivr_lang_mast (lang_id, lang, status, lang_dir, lang_sort_order)
VALUES (3, 'Divehi', 1, 'divehi', 0);

-- -------------------------------------------------------------------
-- IVR_EVR_EVENT_MAST                                               -
-- -------------------------------------------------------------------
INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (0, 'Unknown Event', 'NO_EVT');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (1, 'Incoming Call', 'EVT_IN_CALL');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (2, 'Outgoing Call', 'EVT_OUT_CALL');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (3, 'Play', 'EVT_EXT_PLAY');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (4, 'Stop', 'EVT_EXT_STOP');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (5, 'Ext Patch', 'EVT_EXT_PATCH');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (6, 'Tear', 'EVT_TEAR');
INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (7, 'Schedule Call', 'EVT_SCHEDULE_CALL');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (8, 'CCXML', 'EVT_CCXML');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (9, 'VXML', 'EVT_VXML');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (10, 'Media Play', 'EVT_MEDIA_PLAY');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (11, 'Media Stop', 'EVT_MEDIA_STOP');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (12, 'Media Patch', 'EVT_MEDIA_PATCH');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (13, 'Media Tear', 'EVT_MEDIA_TEAR');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (14, 'Media DTMF', 'EVT_MEDIA_DTMF');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (15, 'MMP Notification', 'EVT_NOTIFICATION');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (16, 'MMP Dedication', 'EVT_DEDICATION');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (17, 'MMP Gift', 'EVT_GIFT');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (18, 'MMP Package Subscription', 'EVT_PACKAGE_SUB');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (19, 'MMP Package Unsubscription', 'EVT_PACKAGE_UNSUB');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (20, 'MMP Package Renewal', 'EVT_RENEW');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (21, 'MMP Daily Subscription', 'EVT_DAILY_SUB');

INSERT INTO ivr_evr_event_mast (evr_event, display, description)
VALUES (22, 'MMP Daily Renewal', 'EVT_DAILY_RENEW');


-- ------------------------------------------------------------------------------
-- ivr_evr_cause_mast (IVR Cause Descriptions - Converted on 2025-06-12)
-- ------------------------------------------------------------------------------

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (0, 'Null', 'IVR_NONE');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (1, 'Incoming Call Received', 'IVR_CALL_RECEIVED');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (2, 'Call Dialled', 'IVR_CALL_DIALLED');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (3, 'Call Answered', 'IVR_CALL_ANSWERED');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (4, 'Call Disconnected', 'IVR_CALL_DISCONNECTED');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (5, 'Call Rejected', 'IVR_CALL_REJECTED');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (6, 'Ext Request Received', 'IVR_EXT_REQ_RECEIVED');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (7, 'Ext Request Complete', 'IVR_EXT_REQ_COMPLETE');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (8, 'Ext Request Failed', 'IVR_EXT_REQ_FAILED');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (9, 'Request Submited', 'IVR_REQ_SUBMITTED');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (10, 'Request Received', 'IVR_REQ_RECEIVED');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (11, 'Request Complete', 'IVR_REQ_COMPLETE');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (12, 'Request Failed', 'IVR_REQ_FAILED');

INSERT INTO ivr_evr_cause_mast (evr_cause, display, description)
VALUES (13, 'Call Completed', 'CAS_IVR_CALL_COMPLETE');

-- -------------------------------------------------------------------------------
-- ivr_evr_reason_mast (IVR Event Reasons - Converted to PostgreSQL on 2025-06-12)
-- -------------------------------------------------------------------------------

INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (0, '-', 'NONE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (20, 'Nested Internal Routing', 'NESTED_ROUTING');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (21, 'IVR Call Busy', 'CALL_BUSY');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (22, 'IVR Call Not Reachable', 'CALL_NOT_REACHABLE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (23, 'Call No Reply', 'CALL_NO_REPLY');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (24, 'Call Barred', 'CALL_BARRED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (25, 'Called Invalid Dest Number', 'CALL_INVALID_DEST_NUMBER');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (26, 'Call Application InActive', 'CALL_APP_NOT_ACTIVE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (27, 'EXT Request Invalid Channel', 'EXT_REQ_INVALID_CHANNEL');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (28, 'EXT Req Invalid Application', 'EXT_REQ_INVAILD_APP');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (29, 'EXT Req Resource Unavailable', 'EXT_REQ_RESOURCE_UNAVAILABLE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (30, 'Scheduled Req Channel Unavailable', 'SCH_REQ_CHANNEL_UNAVAILABLE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (31, 'Scheduled Req Invalid Application', 'SCH_REQ_INVALID_APP');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (32, 'Scheduled Req InActive Application', 'SCH_REQ_INACTIVE_APP');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (33, 'Scheduled Req Resource Unavailable', 'SCH_REQ_RESOURCE_UNAVAILABLE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (34, 'CCXML Invalid Url', 'CCXML_INVAILD_URL');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (35, 'CCXML Invalid Document', 'CCXML_INVALID_DOC');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (36, 'CCXML Resource Unavailable', 'CCXML_RESOURCE_UNAVAILABLE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (37, 'VXML Invalid Url', 'VXML_INVAILD_URL');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (38, 'VXML Invalid Document', 'VXML_INVALID_DOC');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (39, 'VXML Resource Unavailable', 'VXML_RESOURCE_UNAVAILABLE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (40, 'Media Request Invalid File', 'MEDIA_REQ_INVALID_FILE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (41, 'Media Request Invalid Stream', 'MEDIA_REQ_INVALID_STREAM');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (42, 'Media Request Invalid Format', 'MEDIA_REQ_INVALID_FORMAT');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (43, 'Media Request Invalid State', 'MEDIA_REQ_INVALID_STATE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (44, 'Media Req Resource Unavailable', 'MEDIA_REQ_RESOURCE_UNAVAILABLE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (45, 'SMS Failure Notification', 'NOTIFICATION_SMS_FAILED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (46, 'Invalid SNS App Notification', 'NOTIFICATION_SNS_INVALID_APP');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (47, 'No SNS Credentials Notifications', 'NOTIFICATION_SNS_NO_CREDENTIAL');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (48, 'Invalid SNS Credentials Notification', 'NOTIFICATION_SNS_INVALID_CREDE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (49, 'Dedication Charging Failed', 'DEDICATION_CHARGING_FAILED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (50, 'Dedication Application Not Active', 'DEDICATION_APP_NOT_ACTIVE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (51, 'Dedication Attempts Exceeded', 'DEDICATION_ATTEMPTS_EXCEEDS');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (52, 'Dedication Expired', 'DEDICATION_EXPIRED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (53, 'Call Dedication Failed', 'DEDICATION_CALL_FAILED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (54, 'Invalid Gift Subscriber', 'GIFT_REQ_INVALID_RECEIPENT');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (55, 'Invalid Gift Sending Subscriber', 'GIFT_REQ_INVALID_SENDER');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (61, 'Tone Dedication Success', 'DEDICATE_SUCCESS');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (62, 'Package Gifting Success', 'GIFT_SUCCESS');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (63, 'Loyalty', 'LOYALTY_SUCCESS');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (64, 'Package Registration Success', 'PACKAGE_REG_SUCCESS');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (65, 'Package Change Success', 'PACKAGE_CHANGE_SUCCESS');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (66, 'Package Deleted', 'PACKAGE_DELETED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (67, 'Package Charging Failed', 'PACKAGE_CHARGE_FAILED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (68, 'Package Limit Reached', 'PACKAGE_QUOTA_EXCEED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (69, 'Package Already Subscribed', 'PACKAGE_ALREADY_SUBSCRIBED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (70, 'Not Registered to Package', 'PACKAGE_NOT_REGISTERED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (71, 'Package Not Available', 'PAKCAGE_NOT_AVAILABLE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (72, 'Packages is Currently Disabled.', 'PACKAGE_DISABLED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (73, 'Found Multiple PackageId in Received req', 'MULTIPLE_PACKAGE_IN_REQ');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (74, 'Package Expired', 'PACKAGE_EXPIRED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (76, 'Subscription Type is changed', 'SUBS_TYPE_CHANGE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (77, 'Package Renewal Success', 'PACKAGE_RENEW_SUCCESS');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (78, 'Already Renewed Package', 'PACKAGE_ALREADY_RENEWED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (79, 'Auto Renewal of Package Disabled', 'AUTO_RENEW_DISABLED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (81, 'Package UnRegistartion Success', 'PACKAGE_UNREG_SUCCESS');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (85, 'DailyBilling Success', 'DAILYBILL_SUCCESS');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (86, 'Provisional Validation Success', 'PROV_VAL_SUCCESS');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (88, 'Invalid Content Id for Dedication', 'DEDICATE_INVALID_CONTENT_ID');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (89, 'Request rejected', 'REQ_REJECTED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (91, 'Call Busy', 'SYSTEM_ERR');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (92, 'Database Error', 'DB_ERR');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (93, 'Invalid Data', 'INVALID_DATA');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (94, 'Invalid Package', 'INVALID_PACKAGE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (95, 'Failed to get Balance Amount', 'GET_COS_FAILED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (96, 'Number of Data Received is more / less t', 'NUMBER_OF_DATA_ERR');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (97, 'Mandatory Data missing', 'MANDATORY_DATA_MISSING');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (98, 'Msisdn found in BlackList', 'MSISDN_IN_DEDICTD_BL');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (99, 'Found Multiple ToneId in Received reques', 'MULTIPLE_TONEID_IN_REQ');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (100, 'Multiple Subscription Feature Disabled', 'MUTIPLE_GIFTS_DISABLED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (101, 'Nill Balance', 'NILL_BALANCE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (102, 'Configuration Error,while Billing', 'BILLING_CFG_ERR');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (103, 'Billing Failed', 'BILLING_FAILURE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (104, 'Insufficient Balance', 'INSUFFICIENT_BALANCE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (105, 'Blocked Subscriber', 'BLOCKED_SUBSCRIBER');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (106, 'Still In Validity', 'STILL_IN_VALIDITY_LIMIT');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (107, 'InSufficient Balance for Daily', 'SUFF_BAL_DAILY_FAIL');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (108, 'Registration for Daily Billing Success', 'REG_DAILY_BILL_SUC');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (109, 'Subscription Type Get Fails', 'GET_SUBS_TYPE_FAIL');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (110, 'Subscriber Not Registered', 'SUBSCRIBER_NOT_REGISTERED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (111, 'Number Normalisation Fails', 'NUMBER_NORM_FAILED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (112, 'Special Characters Found in Request', 'SPECAIL_CHARS_PRESENT');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (113, 'Gift Number Registered', 'GIFT_NUMBER_REGISTERED');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (114, 'UnAuthorized Interface', 'UNAUTHORISED_INTERFACE');
INSERT INTO ivr_evr_reason_mast(evr_reason, display, description) VALUES (115, 'Daily Bill Disabled', 'DAILY_BILL_DISABLED');

-- ------------------------------------------------------------------
-- Table: ivr_app_provider (Converted to PostgreSQL on 2025-06-12)
-- ------------------------------------------------------------------

INSERT INTO ivr_app_provider (provider_id, revenue_share, status, parent_provider_id)
VALUES (1, 1, 0, 0);

-- ------------------------------------------------------------------
-- Table: ivr_application (Converted to PostgreSQL on 2025-06-12)
-- ------------------------------------------------------------------

-- Disable all triggers for ivr_application
ALTER TABLE ivr_application DISABLE TRIGGER ALL;

INSERT INTO ivr_application (
    appl_id, provider_id, appl_name, appl_type, appl_desc,
    appl_file, upload_date, last_update_date, deleted_date,
    deleted_flag, test_call_cli, charge_id, status
) VALUES (
    -1, 1, 'Dummy', 1, 'Dummy default application',
    NULL, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, NULL,
    'N', NULL, NULL, 4
);

-- Enable all triggers for ivr_application
ALTER TABLE ivr_application ENABLE TRIGGER ALL;


-- ------------------------------------------------------------------
-- Table: ivr_charging_slab (Converted to PostgreSQL on 2025-06-12)
-- ------------------------------------------------------------------

INSERT INTO ivr_charging_slab (
    charge_id, slab_name, amount_per_minute, service_key,
    msc_address, dest_number, smsc_address, vlr_number, entry_time
) VALUES (
    1, 'Dummy', 0, 0,
    NULL, NULL, NULL, NULL, CURRENT_TIMESTAMP
);

-- ------------------------------------------------------------------
-- Table: ivr_notify_events (Converted to PostgreSQL on 2025-06-12)
-- ------------------------------------------------------------------

INSERT INTO ivr_notify_events (noti_event, text, severity, description, display) VALUES (0, 'NONE', 0, 'None', 0);
INSERT INTO ivr_notify_events (noti_event, text, severity, description, display) VALUES (1, 'APPL_ADDED', 0, 'Application Added', 1);
INSERT INTO ivr_notify_events (noti_event, text, severity, description, display) VALUES (2, 'APPL_LIVE', 0, 'Application in Live', 1);
INSERT INTO ivr_notify_events (noti_event, text, severity, description, display) VALUES (3, 'APPL_TESTED', 0, 'Application tested', 1);
INSERT INTO ivr_notify_events (noti_event, text, severity, description, display) VALUES (4, 'APPL_APPROVED', 0, 'Application Approved', 1);
INSERT INTO ivr_notify_events (noti_event, text, severity, description, display) VALUES (5, 'APPL_BLOCKED', 1, 'Application Blocked', 1);
INSERT INTO ivr_notify_events (noti_event, text, severity, description, display) VALUES (6, 'APPL_DELETED', 0, 'Application Deleted', 1);

-- ------------------------------------------------------------------
--               Table : IVR_CHANNEL_CONFIG                         -
-- ------------------------------------------------------------------

INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (1, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (2, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (3, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (4, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (5, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (6, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (7, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (8, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (9, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (10, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (11, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (12, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (13, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (14, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (15, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (16, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (17, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (18, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (19, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (20, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (21, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (22, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (23, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (24, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (25, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (26, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (27, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (28, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (29, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (30, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (31, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (32, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (33, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (34, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (35, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (36, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (37, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (38, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (39, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (40, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (41, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (42, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (43, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (44, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (45, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (46, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (47, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (48, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (49, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (50, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (51, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (52, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (53, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (54, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (55, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (56, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (57, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (58, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (59, 1, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (60, 1, -1, 0);

INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (61, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (62, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (63, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (64, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (65, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (66, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (67, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (68, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (69, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (70, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (71, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (72, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (73, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (74, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (75, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (76, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (77, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (78, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (79, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (80, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (81, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (82, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (83, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (84, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (85, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (86, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (87, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (88, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (89, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (90, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (91, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (92, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (93, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (94, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (95, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (96, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (97, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (98, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (99, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (100, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (101, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (102, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (103, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (104, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (105, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (106, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (107, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (108, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (109, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (110, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (111, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (112, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (113, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (114, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (115, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (116, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (117, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (118, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (119, 2, -1, 0);
INSERT INTO ivr_channel_config (channel_no, channel_type, appl_id, status) VALUES (120, 2, -1, 0);

