/*==============================================================*/
/* User: IVR                                                    */   
/*==============================================================*/
!echo "*********************************************************"
!echo "Creating User"
create user IVR identified by IVR;
GRANT ALL PRIVILEGES TO IVR;

conn IVR/IVR@xe;

!echo "*********************************************************"
!echo "Creating User Management Db"
@IvrUserMgmtCreateDb.sql

!echo "*********************************************************"
!echo "Creating IVR Tables"
@IvrCreateTables.sql

!echo "*********************************************************"
!echo "Creating Application Builder Tables"
@IvrApplBuilderCreateTable.sql

!echo "*********************************************************"
!echo "Creating IVR Triggers"
@IvrTriggers.sql

!echo "*********************************************************"
!echo "Creating IVR Procedures"
@IvrCreateProcedure.sql

!echo "*********************************************************"
!echo "Inserting User Management Default Data"
@IvrUserMgmtInsertDefault.sql

!echo "*********************************************************"
!echo "Inserting Default Data"
@IvrInsertDefault.sql

!echo "*********************************************************"
!echo "Inserting Application Builder Default Data"
@IvrApplBuilderInsertDefault.sql

!echo "*********************************************************"
!echo "Creating IVR EVR Tables"
@IVR_CREATE_EVR_DETAILS.sql

!echo "*********************************************************"
!echo "Deleting IVR EVR Tables"
@IVR_DELETE_IVR_EVR_DETAILS.sql

EXECUTE CREATE_IVR_EVR_DETAILS_PROC(0);

!echo "*********************************************************"
!echo "Creating IVR Jobs"
@IvrJobs.sql
