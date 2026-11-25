CREATE SCHEMA IF NOT EXISTS ivrdb AUTHORIZATION ivr;
GRANT USAGE, CREATE ON SCHEMA ivrdb TO ivr;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA ivrdb TO ivr;

GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA ivrdb TO ivr;

ALTER DEFAULT PRIVILEGES IN SCHEMA ivrdb
GRANT ALL ON TABLES TO ivr;

ALTER DEFAULT PRIVILEGES IN SCHEMA ivrdb
GRANT ALL ON SEQUENCES TO ivr;

SET search_path TO ivrdb;

\i 'IvrUserMgmtCreateDb.sql'
\i 'IvrCreateTables.sql'
\i 'IvrApplBuilderCreateTable.sql'
\i 'IvrTriggers.sql'
\i 'IvrCreateProcedure.sql'
\i 'IvrUserMgmtInsertDefault.sql'
\i 'IvrInsertDefault.sql'
\i 'IvrApplBuilderInsertDefault.sql'
\i 'IVR_CREATE_EVR_DETAILS.sql'
\i 'IVR_DELETE_IVR_EVR_DETAILS.sql'

CALL  CREATE_IVR_EVR_DETAILS_PROC(0);

-- Run jobs
--\i 'IvrJobs.sql'

