--  @(#) $Id: IvrCreateEvents.sql,v 1.1.4.1 2016/04/12 21:59:09 shijuc Exp $
#/*==============================================================*/
#/* DBMS name:       MYSQL Version 5.5.8-log                     */
#/*==============================================================*/

DELIMITER $$

#-- set GLOBAL event_scheduler=ON;  -- with root login
#-- select @@event_scheduler;  -- to check

#/*==============================================================*/
#/* Event    :    CALL_CREATE_IVR_EVR_DETAILS                    */
#/*==============================================================*/


DROP EVENT IF EXISTS CALL_CREATE_IVR_EVR_DETAILS;

CREATE EVENT CALL_CREATE_IVR_EVR_DETAILS

ON SCHEDULE EVERY 1 DAY

STARTS CURRENT_TIMESTAMP

DO

BEGIN

CALL CREATE_IVR_EVR_DETAILS_PROC(1);

END $$

CALL CREATE_IVR_EVR_DETAILS_PROC(0);

COMMIT $$

#/*==============================================================*/
#/* Event    :    CALL_DELETE_IVR_EVR_DETAILS                    */
#/*==============================================================*/

DROP EVENT IF EXISTS CALL_DELETE_IVR_EVR_DETAILS;

CREATE EVENT CALL_DELETE_IVR_EVR_DETAILS

ON SCHEDULE EVERY 1 DAY

STARTS CURRENT_TIMESTAMP

DO

BEGIN

CALL DELETE_IVR_EVR_DETAILS_PROC(30);

END $$

COMMIT $$


