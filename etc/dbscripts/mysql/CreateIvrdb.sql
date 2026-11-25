--  @(#) $Id: CreateIvrdb.sql,v 1.1.4.1 2016/04/12 21:59:08 shijuc Exp $ 
/*==============================================================*/
/* DBMS name:      MySQL 5.0                                    */
/* Created on:     14-05-2012 11:00:00                          */
/*==============================================================*/

use ivr;
set storage_engine='InnoDb';
set foreign_key_checks = 0;
DROP USER 'ivr'@'%';
DROP USER ivr@localhost;

CREATE USER 'ivr'@'%' IDENTIFIED BY 'ivr';

CREATE USER 'ivr'@'localhost' IDENTIFIED BY 'ivr';

DROP DATABASE IF EXISTS ivr;

CREATE DATABASE IF NOT EXISTS ivr;

GRANT ALL PRIVILEGES ON *.* TO 'ivr'@'%';
GRANT ALL PRIVILEGES ON *.* TO 'ivr'@'localhost';

FLUSH PRIVILEGES;

USE ivr;

set foreign_key_checks = 0;

set global event_scheduler=ON;

source IvrUserMgmtCreateDb.sql

source IvrCreateTables.sql

source IvrCreateProcedure.sql

source IvrUserMgmtInsertDefault.sql

source IvrInsertDefault.sql

source IvrApplBuilderCreateTable.sql

source IvrApplBuilderInsertDefault.sql

source IVR_CREATE_EVR_DETAILS.sql

source IVR_DELETE_IVR_EVR_DETAILS.sql

source IvrCreateEvents.sql

select "IVR Database Installation Completed"  as IVR_SCRIPT_OUTPUT ;

