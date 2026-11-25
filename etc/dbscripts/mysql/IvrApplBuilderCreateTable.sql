--  @(#) $Id: IvrApplBuilderCreateTable.sql,v 1.1.4.1 2016/04/12 21:59:09 shijuc Exp $
CREATE TABLE `APP_BUILDER_MAST` (
  `APP_ID` int(11) NOT NULL auto_increment,
  `APP_NAME` varchar(25) default NULL,
  `APP_DESC` varchar(200) default NULL,
  `TYPE` smallint(6) default NULL COMMENT '1-Incoming 2-Outgoing',
  `USER_ID` int(5) default NULL,
  PRIMARY KEY  (`APP_ID`)
) ENGINE=InnoDb AUTO_INCREMENT=14 DEFAULT CHARSET=latin1;

CREATE TABLE `IVR_VXML_ELEMENT_MASTER` (
  `ELEMENT_ID` tinyint(4) NOT NULL,
  `PARENT` tinyint(1) default NULL,
  `CHILD` tinyint(1) default NULL,
  `DISPLAY` varchar(50) NOT NULL,
  `DESCRIPTION` varchar(200) default NULL,
  PRIMARY KEY  (`ELEMENT_ID`)
) ENGINE=InnoDb DEFAULT CHARSET=latin1 COMMENT='This table contains the master record for each element of va';

CREATE TABLE `IVR_VXML_ELEMENT_PARENT` (
  `ELEMENT_ID` tinyint(4) default NULL,
  `PARENT_ID` tinyint(4) default NULL,
  KEY `FK_REFERENCE_3` (`ELEMENT_ID`),
  KEY `FK_REFERENCE_6` (`PARENT_ID`)
) ENGINE=InnoDb DEFAULT CHARSET=latin1;

CREATE TABLE `IVR_VXML_ELEMENT_CHILD` (
  `ELEMENT_ID` tinyint(4) default NULL,
  `CHILD_ID` tinyint(4) default NULL,
  KEY `FK_REFERENCE_4` (`ELEMENT_ID`),
  KEY `FK_REFERENCE_7` (`CHILD_ID`)
) ENGINE=InnoDb DEFAULT CHARSET=latin1;

CREATE TABLE `IVR_VXML_ATTR_DATATYPE_MASTER` (
  `DATATYPE_ID` tinyint(4) NOT NULL,
  `VALUE` tinyint(1) default NULL,
  `DISPLAY` varchar(50) default NULL,
  PRIMARY KEY  (`DATATYPE_ID`)
) ENGINE=InnoDb DEFAULT CHARSET=latin1 COMMENT='cONATAINS THE DIFFERENT DATA TYPE AVAILABLE IN VXML.';

CREATE TABLE `IVR_VXML_ATTR_DATATYPE_VALUES` (
  `DATATYPE_ID` tinyint(4) default NULL,
  `VALUE` varchar(50) default NULL,
  KEY `FK_REFERENCE_8` (`DATATYPE_ID`)
) ENGINE=InnoDb DEFAULT CHARSET=latin1;

CREATE TABLE `IVR_VXML_ATTRIBUTE_MASTER` (
  `ATTRIBUTE_ID` tinyint(4) NOT NULL,
  `DATATYPE_ID` tinyint(4) default NULL,
  `DISPLAY` varchar(50) default NULL,
  `DESCRIPTION` varchar(200) default NULL,
  PRIMARY KEY  (`ATTRIBUTE_ID`),
  KEY `FK_REFERENCE_5` (`DATATYPE_ID`)
) ENGINE=InnoDb DEFAULT CHARSET=latin1 COMMENT='This table contains the master record for attributes.';

CREATE TABLE `IVR_VXML_ELEMENT_DETAILS` (
  `ELEMENT_ID` tinyint(4) default NULL,
  `ATTRIBUTE_ID` tinyint(4) default NULL,
  `STATUS` tinyint(1) default NULL,
  KEY `FK_REFERENCE_1` (`ELEMENT_ID`),
  KEY `FK_REFERENCE_2` (`ATTRIBUTE_ID`)
) ENGINE=InnoDb DEFAULT CHARSET=latin1;

CREATE TABLE `IVR_VXML_SUPPORTED_PARENT_ELEMENT` (
  `PARENT_ELEMENT_ID` int(11) NOT NULL,
  `ELEMENT_ID` int(11) NOT NULL,
  `DISPLAY` varchar(30) NOT NULL,
  `DESCRIPTION` varchar(100) NOT NULL,
  `IMAGE_NAME` varchar(30) NOT NULL,
  `DISPLAY_ORDER` int(11) NOT NULL default '0',
  PRIMARY KEY  (`PARENT_ELEMENT_ID`),
  KEY `ELEMENT_ID` (`ELEMENT_ID`)
) ENGINE=InnoDb DEFAULT CHARSET=latin1;

CREATE TABLE `LIBRARY_ROUTINES` (
  `ROUTINE_ID` int(11) NOT NULL auto_increment COMMENT 'Auto generated sequence Number',
  `PROVIDER_ID` int(11) NOT NULL default '0' COMMENT 'Provider Id Associated with Routine, 0: System Defined, other than 0 : Provider Id who has created the Routine',
  `NAME` varchar(50) NOT NULL COMMENT 'Unique Library Routine Name to be used for GUI display and identification purpose',
  `DESCRIPTION` varchar(200) default NULL COMMENT 'Full Description of Library Routine Functionality',
  `IN_PARAMS` varchar(200) default NULL COMMENT 'CSV List of IN_PARAMS  of Library Routine',
  `OUT_PARAMS` varchar(200) default NULL COMMENT 'CSV List of OUT_PARAMS of Library Routine',
  `ACTIVE` varchar(1) default 'Y' COMMENT 'ACTIVE filed tells whether LIBRARY_ROUTINE is active or not. ''N'' :  In-Active, ''Y'' : Active',
  `IN_PARAM_TYPE` varchar(50) default NULL,
  `IN_VALUE_LIST` varchar(200) default NULL,
  PRIMARY KEY  (`ROUTINE_ID`)
) ENGINE=InnoDb AUTO_INCREMENT=3 DEFAULT CHARSET=latin1 COMMENT='LIBRARY_ROUTINES  table holds the system/user defined librar';

CREATE TABLE `SPEAK_FORMAT_DISPLAY` (
  `SPEAK_ID` int(11) NOT NULL auto_increment,
  `TYPE` varchar(20) default NULL,
  `FORMAT` text,
  `DISPLAY_ORDER` int(11) default NULL,
  PRIMARY KEY (`SPEAK_ID`)
) ENGINE=InnoDb DEFAULT CHARSET=latin1;

CREATE TABLE `PROPERTY` (
  `NAME` varchar(50) NOT NULL,
  `TYPE` char(1) NOT NULL,
  `VALUE` varchar(250) default NULL,
  `DESCRIPTION` varchar(200) default NULL,
  `MIN` smallint(3) default NULL,
  `MAX` smallint(3) default NULL,
  `VALIDATION_EXPR` varchar(200) default NULL,
  PRIMARY KEY  (`NAME`)
) ENGINE=InnoDb DEFAULT CHARSET=latin1;


CREATE TABLE `IVR_SNAPSHOT_CUMULATIVE` (
  `GEN_TIME` date NOT NULL,
  `EVR_EVENT` smallint(5) NOT NULL,
  `EVR_CAUSE` smallint(5) NOT NULL,
  `EVR_REASON` smallint(5) NOT NULL default '0',
  `COUNT` smallint(5) default '0',
  `EVR_DURATION` int(11) default NULL,
  PRIMARY KEY  (`GEN_TIME`,`EVR_EVENT`,`EVR_CAUSE`,`EVR_REASON`)
);


CREATE TABLE `IVR_MIS_CONTENT_CUMULATIVE` (
  `GEN_DATE` date NOT NULL,
  `GEN_HOUR` smallint(2) NOT NULL,
  `PROVIDER_ID` smallint(5) NOT NULL,
  `NO_CONTENT_UPLOAD` smallint(5) default NULL,
  `NO_CONTENT_DELETE` smallint(5) default NULL,
  `NO_CONTENT_AVAILABLE` smallint(5) default NULL,
  `NO_CONTENT_BLOCKED` smallint(6) default NULL,
  `NO_CONTENT_PREVIEW` smallint(6) default NULL,
  `NO_CONTENT_REJECTED` smallint(6) default NULL,
  PRIMARY KEY  (`GEN_DATE`,`GEN_HOUR`,`PROVIDER_ID`)
);


CREATE TABLE `IVR_KEY_SUBS_MIS_CUMULATIVE` (
  `GEN_TIME` datetime NOT NULL,
  `EVR_EVENT` smallint(6) NOT NULL,
  `EVR_CAUSE` smallint(6) NOT NULL,
  `REQUEST_MODE` int(11) NOT NULL,
  `PKG_ID` int(11) NOT NULL,
  `CNT` int(11) default '0',
  `AMT` int(11) default NULL,
  PRIMARY KEY  (`GEN_TIME`,`EVR_EVENT`,`EVR_CAUSE`,`REQUEST_MODE`,`PKG_ID`)
);


CREATE TABLE `IVR_CONTENT_MIS_CUMULATIVE` (
  `GEN_TIME` datetime NOT NULL,
  `EVR_EVENT` smallint(6) NOT NULL,
  `EVR_CAUSE` smallint(6) NOT NULL,
  `CONTENT_ID` varchar(20) NOT NULL,
  `CHARGE_UNIT` smallint(6) NOT NULL,
  `CNT` int(11) default '0',
  `CNT_DUR` int(11) default '0',
  `AMT` int(11) default '0',
  PRIMARY KEY  (`GEN_TIME`,`EVR_EVENT`,`EVR_CAUSE`,`CONTENT_ID`,`CHARGE_UNIT`)
);


CREATE TABLE `IVR_APPL_MIS_CUMULATIVE` (
  `GEN_TIME` datetime NOT NULL,
  `EVR_EVENT` smallint(6) NOT NULL,
  `EVR_CAUSE` smallint(6) NOT NULL,
  `APP_ID` varchar(20) NOT NULL,
  `CHARGE_UNIT` smallint(6) NOT NULL,
  `CNT` int(11) default '0',
  `AMT` int(11) default '0',
  PRIMARY KEY  (`GEN_TIME`,`EVR_EVENT`,`EVR_CAUSE`,`APP_ID`,`CHARGE_UNIT`)
);


