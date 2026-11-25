--  @(#) $Id: IvrUserMgmtInsertDefault.sql,v 1.1.4.1.2.2 2025/06/12 20:03:00 mram Exp $

INSERT INTO gui_user_type_master (user_type_id, user_type_name, user_type)VALUES (1, 'admin', 0);

UPDATE gui_user_type_master SET user_type_id = 1 WHERE user_type_name = 'admin';

INSERT INTO gui_module_mast (module_id, module_name, flag) VALUES (1, 'IVR', 'P');
INSERT INTO gui_submodule_mast (submodule_id, module_id, submodule_name, flag, permission_display) VALUES (1, 1, 'Application Builder', 'P', '1');
INSERT INTO gui_submodule_mast (submodule_id, module_id, submodule_name, flag, permission_display) VALUES (2, 1, 'IVR_System', NULL, '1');
INSERT INTO gui_submodule_mast (submodule_id, module_id, submodule_name, flag, permission_display) VALUES (3, 1, 'IVR_Application', 'P', '1');
INSERT INTO gui_submodule_mast (submodule_id, module_id, submodule_name, flag, permission_display) VALUES (4, 1, 'IVR_Shortcode', NULL, '1');
INSERT INTO gui_submodule_mast (submodule_id, module_id, submodule_name, flag, permission_display) VALUES (5, 1, 'IVR_Snapshot', NULL, '0');
INSERT INTO gui_submodule_mast (submodule_id, module_id, submodule_name, flag, permission_display) VALUES (6, 1, 'IVR_Channel Usage', NULL, '0');
INSERT INTO gui_submodule_mast (submodule_id, module_id, submodule_name, flag, permission_display) VALUES (7, 1, 'IVR_Notification', NULL, '1');
INSERT INTO gui_submodule_mast (submodule_id, module_id, submodule_name, flag, permission_display) VALUES (8, 1, 'IVR_Approval', NULL, '1');
INSERT INTO gui_submodule_mast (submodule_id, module_id, submodule_name, flag, permission_display) VALUES (9, 1, 'IVR_MIS_Shortcode', NULL, '0');

INSERT INTO gui_user_permissions_det (user_type_id, submodule_id, write_permission) VALUES (1, 1, 1);
INSERT INTO gui_user_permissions_det (user_type_id, submodule_id, write_permission) VALUES (1, 2, 1);
INSERT INTO gui_user_permissions_det (user_type_id, submodule_id, write_permission) VALUES (1, 3, 1);
INSERT INTO gui_user_permissions_det (user_type_id, submodule_id, write_permission) VALUES (1, 4, 1);
INSERT INTO gui_user_permissions_det (user_type_id, submodule_id, write_permission) VALUES (1, 5, 1);
INSERT INTO gui_user_permissions_det (user_type_id, submodule_id, write_permission) VALUES (1, 6, 1);
INSERT INTO gui_user_permissions_det (user_type_id, submodule_id, write_permission) VALUES (1, 7, 1);
INSERT INTO gui_user_permissions_det (user_type_id, submodule_id, write_permission) VALUES (1, 8, 1);
INSERT INTO gui_user_permissions_det (user_type_id, submodule_id, write_permission) VALUES (1, 9, 1);
INSERT INTO gui_user_permissions_det (user_type_id, submodule_id, write_permission) VALUES (1, 11, 1);

INSERT INTO gui_module_mast (module_id, module_name, flag) VALUES (5, 'Cust Care', 'P');
INSERT INTO gui_submodule_mast (submodule_id, module_id, submodule_name, flag, permission_display) VALUES (39, 5, 'Tracker', NULL, '0');
INSERT INTO gui_user_permissions_det (user_type_id, submodule_id, write_permission) VALUES (1, 39, 1);

-- INSERT INTO gui_module_mast (module_id, module_name, flag) VALUES (6, 'Alarm', 'P');
-- INSERT INTO gui_submodule_mast (submodule_id, module_id, submodule_name, flag, permission_display) VALUES (40, 6, 'Alarm', NULL, '0');
-- INSERT INTO gui_user_permissions_det (user_type_id, submodule_id, write_permission) VALUES (1, 40, 1);

INSERT INTO gui_procedure_exc_time (from_date, proc_name) VALUES (CURRENT_TIMESTAMP, 'IVR_SNAPSHOT_CUMULATIVE_PROC');

INSERT INTO gui_procedure_exc_time (from_date, proc_name)VALUES (CURRENT_TIMESTAMP, 'IVR_SHORTCODE_CUMULATIVE_PROC');

INSERT INTO gui_user_details ( user_id, user_type_id, user_name, user_pass, user_privilege, mail_id, contact_number
)VALUES (1, 1, 'admin', 'JFUuyKJT1Ls=', 'S', 'test123@test.co.in', '9651313131');

UPDATE gui_user_details SET user_id = 1 WHERE user_name = 'admin';

