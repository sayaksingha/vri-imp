--  @(#) $Id: IvrUserMgmtCreateDb.sql,v 1.1.4.1 2025/06/12 21:59:12 mram Exp $
-- Table: gui_user_type_master
CREATE TABLE gui_user_type_master (
    user_type_id     INT PRIMARY KEY,
    user_type_name   VARCHAR(20) UNIQUE,
    user_type        SMALLINT
);

-- Table: gui_module_mast
CREATE TABLE gui_module_mast (
    module_id     INT PRIMARY KEY,
    module_name   VARCHAR(20),
    flag          CHAR(1)
);

-- Table: gui_submodule_mast
CREATE TABLE gui_submodule_mast (
    submodule_id        INT PRIMARY KEY,
    module_id           INT,
    submodule_name      VARCHAR(50) NOT NULL,
    flag                VARCHAR(1),
    permission_display  VARCHAR(1),
    UNIQUE (module_id, submodule_name)
);

-- Table: gui_user_permissions_det
CREATE TABLE gui_user_permissions_det (
    user_type_id     INT,
    submodule_id     INT,
    write_permission INT
);

-- Table: gui_user_details
CREATE TABLE gui_user_details (
    user_id         INT PRIMARY KEY,
    user_type_id    INT,
    user_name       VARCHAR(25) NOT NULL,
    user_pass       VARCHAR(35),
    user_privilege  VARCHAR(1),
    mail_id         VARCHAR(100),
    contact_number  VARCHAR(20)
);

-- Table: ivr_gui_logs
CREATE TABLE ivr_gui_logs (
    gen_time        TIMESTAMP,
    uname           VARCHAR(20),
    module          VARCHAR(20),
    submodule       VARCHAR(50),
    operation       VARCHAR(50),
    previous_value  VARCHAR(500),
    new_value       VARCHAR(500),
    ipaddress       VARCHAR(50),
    node            VARCHAR(50) DEFAULT NULL
);

-- Table: gui_procedure_exc_time
CREATE TABLE gui_procedure_exc_time (
    from_date   TIMESTAMP,
    proc_name   VARCHAR(50) DEFAULT NULL
);

-- Table: user_logged_information
CREATE TABLE user_logged_information (
    user_id         INT,
    browser         VARCHAR(50),
    client_address  VARCHAR(50),
    logged_date     TIMESTAMP NOT NULL DEFAULT statement_timestamp()
);

