--  @(#) $Id: IvrUserMgmtCreateDb.sql,v 1.1.4.1 2016/04/12 21:59:10 shijuc Exp $

drop table if exists GUI_USER_TYPE_MASTER;

drop table if exists GUI_MODULE_MAST;

drop table if exists GUI_SUBMODULE_MAST;

drop table if exists GUI_USER_PERMISSIONS_DET;

drop table if exists GUI_USER_DETAILS;

drop table if exists IVR_GUI_LOGS;

drop table if exists GUI_PROCEDURE_EXC_TIME;

drop table if exists USER_LOGGED_INFORMATION;


CREATE TABLE `GUI_USER_TYPE_MASTER` (
  `USER_TYPE_ID` smallint(6) NOT NULL auto_increment,
  `USER_TYPE_NAME` varchar(20) default NULL,
  `USER_TYPE` smallint(10) default NULL,
  PRIMARY KEY  (`USER_TYPE_ID`),
  KEY `AK_KEY_2` (`USER_TYPE_NAME`)
) ENGINE=InnoDb AUTO_INCREMENT=1 DEFAULT CHARSET=latin1;

CREATE TABLE `GUI_MODULE_MAST` (
  `MODULE_ID` int(11) NOT NULL default '0',
  `MODULE_NAME` varchar(20) default NULL,
  `FLAG` char(1) default NULL,
  PRIMARY KEY  (`MODULE_ID`)
) ENGINE=InnoDb DEFAULT CHARSET=latin1;

CREATE TABLE `GUI_SUBMODULE_MAST` (
  `SUBMODULE_ID` smallint(6) NOT NULL,
  `MODULE_ID` smallint(6) default NULL,
  `SUBMODULE_NAME` varchar(50) NOT NULL,
  `FLAG` char(1) default NULL,
  `PERMISSION_DISPLAY` char(1) default NULL,
  PRIMARY KEY  (`SUBMODULE_ID`),
  KEY `AK_KEY_2` (`SUBMODULE_NAME`,`MODULE_ID`)
) ENGINE=InnoDb DEFAULT CHARSET=latin1 COMMENT='Sub module in Products';

CREATE TABLE `GUI_USER_PERMISSIONS_DET` (
  `USER_TYPE_ID` smallint(6) default NULL,
  `SUBMODULE_ID` smallint(6) default NULL,
  `WRITE_PERMISSION` smallint(1) default NULL
) ENGINE=InnoDb DEFAULT CHARSET=latin1;

CREATE TABLE `GUI_USER_DETAILS` (
  `USER_ID` int(10) NOT NULL auto_increment,
  `USER_TYPE_ID` bigint(4) default NULL,
  `USER_NAME` varchar(25) NOT NULL,
  `USER_PASS` varchar(35) default NULL,
  `USER_PRIVILEGE` varchar(1) default NULL,
  `MAIL_ID` varchar(100) default NULL,
  `CONTACT_NUMBER` varchar(20) default NULL,
  PRIMARY KEY  (`USER_ID`)
) ENGINE=InnoDb AUTO_INCREMENT=1 DEFAULT CHARSET=latin1;


CREATE TABLE `IVR_GUI_LOGS` (
  `GEN_TIME` datetime  default NULL,
  `UNAME` varchar(20) default NULL,
  `MODULE` varchar(20) default NULL,
  `SUBMODULE` varchar(50) default NULL,
  `OPERATION` varchar(50) default NULL,
  `PREVIOUS_VALUE` varchar(500) default NULL,
  `NEW_VALUE` varchar(500) default NULL,
  `IPADDRESS` varchar(50) default NULL,
  `NODE` varchar(50) default NULL
) ENGINE=InnoDb DEFAULT CHARSET=latin1;


CREATE TABLE `GUI_PROCEDURE_EXC_TIME` (
  `FROM_DATE` datetime default NULL,
  `PROC_NAME` varchar(50) default NULL
);


CREATE TABLE `USER_LOGGED_INFORMATION` (
  `USER_ID` int(11) default NULL,
  `BROWSER` varchar(50) default NULL,
  `CLIENT_ADDRESS` varchar(50) default NULL,
  `LOGGED_DATE` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP
) ENGINE=InnoDb DEFAULT CHARSET=latin1;
