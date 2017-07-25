#include "../inc/mySqlWrapper.h"
#include <cstdlib>


// Named constant for passing to SqlException when passing it a string
// that cannot be deleted.
static const bool DONT_DELETE_MSG=false;

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
SqlException::SqlException(const int nErrCode,
									char* szErrMess,
									bool bDeleteMsg/*=true*/) :
									mnErrCode(nErrCode)
{
	mpszErrMess = sqlite3_mprintf("%s[%d]: %s",
								errorCodeAsString(nErrCode),
								nErrCode,
								szErrMess ? szErrMess : "");

	if (bDeleteMsg && szErrMess)
	{
		sqlite3_free(szErrMess);
	}
}

									
SqlException::SqlException(const SqlException&  e) :
									mnErrCode(e.mnErrCode)
{
	mpszErrMess = 0;
	if (e.mpszErrMess)
	{
		mpszErrMess = sqlite3_mprintf("%s", e.mpszErrMess);
	}
}


const char* SqlException::errorCodeAsString(int nErrCode)
{
	switch (nErrCode)
	{
		case SQLITE_OK          : return "SQLITE_OK";
		case SQLITE_ERROR       : return "SQLITE_ERROR";
		case SQLITE_INTERNAL    : return "SQLITE_INTERNAL";
		case SQLITE_PERM        : return "SQLITE_PERM";
		case SQLITE_ABORT       : return "SQLITE_ABORT";
		case SQLITE_BUSY        : return "SQLITE_BUSY";
		case SQLITE_LOCKED      : return "SQLITE_LOCKED";
		case SQLITE_NOMEM       : return "SQLITE_NOMEM";
		case SQLITE_READONLY    : return "SQLITE_READONLY";
		case SQLITE_INTERRUPT   : return "SQLITE_INTERRUPT";
		case SQLITE_IOERR       : return "SQLITE_IOERR";
		case SQLITE_CORRUPT     : return "SQLITE_CORRUPT";
		case SQLITE_NOTFOUND    : return "SQLITE_NOTFOUND";
		case SQLITE_FULL        : return "SQLITE_FULL";
		case SQLITE_CANTOPEN    : return "SQLITE_CANTOPEN";
		case SQLITE_PROTOCOL    : return "SQLITE_PROTOCOL";
		case SQLITE_EMPTY       : return "SQLITE_EMPTY";
		case SQLITE_SCHEMA      : return "SQLITE_SCHEMA";
		case SQLITE_TOOBIG      : return "SQLITE_TOOBIG";
		case SQLITE_CONSTRAINT  : return "SQLITE_CONSTRAINT";
		case SQLITE_MISMATCH    : return "SQLITE_MISMATCH";
		case SQLITE_MISUSE      : return "SQLITE_MISUSE";
		case SQLITE_NOLFS       : return "SQLITE_NOLFS";
		case SQLITE_AUTH        : return "SQLITE_AUTH";
		case SQLITE_FORMAT      : return "SQLITE_FORMAT";
		case SQLITE_RANGE       : return "SQLITE_RANGE";
		case SQLITE_ROW         : return "SQLITE_ROW";
		case SQLITE_DONE        : return "SQLITE_DONE";
		case MY_SQLITE_ERROR    : return "MY_SQLITE_ERROR";
		default: return "UNKNOWN_ERROR";
	}
}


SqlException::~SqlException()
{
	if (mpszErrMess)
	{
		sqlite3_free(mpszErrMess);
		mpszErrMess = 0;
	}
}


////////////////////////////////////////////////////////////////////////////////

SqlBuffer::SqlBuffer()
{
	mpBuf = 0;
}


SqlBuffer::~SqlBuffer()
{
	clear();
}


void SqlBuffer::clear()
{
	if (mpBuf)
	{
		sqlite3_free(mpBuf);
		mpBuf = 0;
	}

}


const char* SqlBuffer::format(const char* szFormat, ...)
{
	clear();
	va_list va;
	va_start(va, szFormat);
	mpBuf = sqlite3_vmprintf(szFormat, va);
	va_end(va);
	return mpBuf;
}

SqlQuery::SqlQuery()
{
	mpVM = 0;
	mbEof = true;
	mnCols = 0;
	mbOwnVM = false;
}


SqlQuery::SqlQuery(const SqlQuery& rQuery)
{
	mpVM = rQuery.mpVM;
	// Only one object can own the VM
	const_cast<SqlQuery&>(rQuery).mpVM = 0;
	mbEof = rQuery.mbEof;
	mnCols = rQuery.mnCols;
	mbOwnVM = rQuery.mbOwnVM;
}


SqlQuery::SqlQuery(sqlite3* pDB,
							sqlite3_stmt* pVM,
							bool bEof,
							bool bOwnVM/*=true*/)
{
	mpDB = pDB;
	mpVM = pVM;
	mbEof = bEof;
	mnCols = sqlite3_column_count(mpVM);
	mbOwnVM = bOwnVM;
}


SqlQuery::~SqlQuery()
{
	try
	{
		finalize();
	}
	catch (...)
	{
	}
}


SqlQuery& SqlQuery::operator=(const SqlQuery& rQuery)
{
	try
	{
		finalize();
	}
	catch (...)
	{
	}
	mpVM = rQuery.mpVM;
	// Only one object can own the VM
	const_cast<SqlQuery&>(rQuery).mpVM = 0;
	mbEof = rQuery.mbEof;
	mnCols = rQuery.mnCols;
	mbOwnVM = rQuery.mbOwnVM;
	return *this;
}


int SqlQuery::numFields()
{
	checkVM();
	return mnCols;
}


const char* SqlQuery::fieldValue(int nField)
{
	checkVM();

	if (nField < 0 || nField > mnCols-1)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Invalid field index requested",
								DONT_DELETE_MSG);
	}

	return (const char*)sqlite3_column_text(mpVM, nField);
}


const char* SqlQuery::fieldValue(const char* szField)
{
	int nField = fieldIndex(szField);
	return (const char*)sqlite3_column_text(mpVM, nField);
}


int SqlQuery::getIntField(int nField, int nNullValue/*=0*/)
{
	if (fieldDataType(nField) == SQLITE_NULL)
	{
		return nNullValue;
	}
	else
	{
		return sqlite3_column_int(mpVM, nField);
	}
}


int SqlQuery::getIntField(const char* szField, int nNullValue/*=0*/)
{
	int nField = fieldIndex(szField);
	return getIntField(nField, nNullValue);
}


double SqlQuery::getFloatField(int nField, double fNullValue/*=0.0*/)
{
	if (fieldDataType(nField) == SQLITE_NULL)
	{
		return fNullValue;
	}
	else
	{
		return sqlite3_column_double(mpVM, nField);
	}
}


double SqlQuery::getFloatField(const char* szField, double fNullValue/*=0.0*/)
{
	int nField = fieldIndex(szField);
	return getFloatField(nField, fNullValue);
}


const char* SqlQuery::getStringField(int nField, const char* szNullValue/*=""*/)
{
	if (fieldDataType(nField) == SQLITE_NULL)
	{
		return szNullValue;
	}
	else
	{
		return (const char*)sqlite3_column_text(mpVM, nField);
	}
}


const char* SqlQuery::getStringField(const char* szField, const char* szNullValue/*=""*/)
{
	int nField = fieldIndex(szField);
	return getStringField(nField, szNullValue);
}


const unsigned char* SqlQuery::getBlobField(int nField, int& nLen)
{
	checkVM();

	if (nField < 0 || nField > mnCols-1)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Invalid field index requested",
								DONT_DELETE_MSG);
	}

	nLen = sqlite3_column_bytes(mpVM, nField);
	return (const unsigned char*)sqlite3_column_blob(mpVM, nField);
}


const unsigned char* SqlQuery::getBlobField(const char* szField, int& nLen)
{
	int nField = fieldIndex(szField);
	return getBlobField(nField, nLen);
}


bool SqlQuery::fieldIsNull(int nField)
{
	return (fieldDataType(nField) == SQLITE_NULL);
}


bool SqlQuery::fieldIsNull(const char* szField)
{
	int nField = fieldIndex(szField);
	return (fieldDataType(nField) == SQLITE_NULL);
}


int SqlQuery::fieldIndex(const char* szField)
{
	checkVM();

	if (szField)
	{
		for (int nField = 0; nField < mnCols; nField++)
		{
			const char* szTemp = sqlite3_column_name(mpVM, nField);

			if (strcmp(szField, szTemp) == 0)
			{
				return nField;
			}
		}
	}

	throw SqlException(MY_SQLITE_ERROR,
							(char *)"Invalid field name requested",
							DONT_DELETE_MSG);
}


const char* SqlQuery::fieldName(int nCol)
{
	checkVM();

	if (nCol < 0 || nCol > mnCols-1)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Invalid field index requested",
								DONT_DELETE_MSG);
	}

	return sqlite3_column_name(mpVM, nCol);
}


const char* SqlQuery::fieldDeclType(int nCol)
{
	checkVM();

	if (nCol < 0 || nCol > mnCols-1)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Invalid field index requested",
								DONT_DELETE_MSG);
	}

	return sqlite3_column_decltype(mpVM, nCol);
}


int SqlQuery::fieldDataType(int nCol)
{
	checkVM();

	if (nCol < 0 || nCol > mnCols-1)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Invalid field index requested",
								DONT_DELETE_MSG);
	}

	return sqlite3_column_type(mpVM, nCol);
}


bool SqlQuery::eof()
{
	checkVM();
	return mbEof;
}


void SqlQuery::nextRow()
{
	checkVM();

	int nRet = sqlite3_step(mpVM);

	if (nRet == SQLITE_DONE)
	{
		// no rows
		mbEof = true;
	}
	else if (nRet == SQLITE_ROW)
	{
		// more rows, nothing to do
	}
	else
	{
		nRet = sqlite3_finalize(mpVM);
		mpVM = 0;
		const char* szError = sqlite3_errmsg(mpDB);
		throw SqlException(nRet,
								(char*)szError,
								DONT_DELETE_MSG);
	}
}


void SqlQuery::finalize()
{
	if (mpVM && mbOwnVM)
	{
		int nRet = sqlite3_finalize(mpVM);
		mpVM = 0;
		if (nRet != SQLITE_OK)
		{
			const char* szError = sqlite3_errmsg(mpDB);
			throw SqlException(nRet, (char*)szError, DONT_DELETE_MSG);
		}
	}
}


void SqlQuery::checkVM()
{
	if (mpVM == 0)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Null Virtual Machine pointer",
								DONT_DELETE_MSG);
	}
}


////////////////////////////////////////////////////////////////////////////////

SqlTable::SqlTable()
{
	mpaszResults = 0;
	mnRows = 0;
	mnCols = 0;
	mnCurrentRow = 0;
}


SqlTable::SqlTable(const SqlTable& rTable)
{
	mpaszResults = rTable.mpaszResults;
	// Only one object can own the results
	const_cast<SqlTable&>(rTable).mpaszResults = 0;
	mnRows = rTable.mnRows;
	mnCols = rTable.mnCols;
	mnCurrentRow = rTable.mnCurrentRow;
}


SqlTable::SqlTable(char** paszResults, int nRows, int nCols)
{
	mpaszResults = paszResults;
	mnRows = nRows;
	mnCols = nCols;
	mnCurrentRow = 0;
}


SqlTable::~SqlTable()
{
	try
	{
		finalize();
	}
	catch (...)
	{
	}
}


SqlTable& SqlTable::operator=(const SqlTable& rTable)
{
	try
	{
		finalize();
	}
	catch (...)
	{
	}
	mpaszResults = rTable.mpaszResults;
	// Only one object can own the results
	const_cast<SqlTable&>(rTable).mpaszResults = 0;
	mnRows = rTable.mnRows;
	mnCols = rTable.mnCols;
	mnCurrentRow = rTable.mnCurrentRow;
	return *this;
}


void SqlTable::finalize()
{
	if (mpaszResults)
	{
		sqlite3_free_table(mpaszResults);
		mpaszResults = 0;
	}
}


int SqlTable::numFields()
{
	checkResults();
	return mnCols;
}


int SqlTable::numRows()
{
	checkResults();
	return mnRows;
}


const char* SqlTable::fieldValue(int nField)
{
	checkResults();

	if (nField < 0 || nField > mnCols-1)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Invalid field index requested",
								DONT_DELETE_MSG);
	}

	int nIndex = (mnCurrentRow*mnCols) + mnCols + nField;
	return mpaszResults[nIndex];
}


const char* SqlTable::fieldValue(const char* szField)
{
	checkResults();

	if (szField)
	{
		for (int nField = 0; nField < mnCols; nField++)
		{
			if (strcmp(szField, mpaszResults[nField]) == 0)
			{
				int nIndex = (mnCurrentRow*mnCols) + mnCols + nField;
				return mpaszResults[nIndex];
			}
		}
	}

	throw SqlException(MY_SQLITE_ERROR,
							(char *)"Invalid field name requested",
							DONT_DELETE_MSG);
}


int SqlTable::getIntField(int nField, int nNullValue/*=0*/)
{
	if (fieldIsNull(nField))
	{
		return nNullValue;
	}
	else
	{
		return atoi(fieldValue(nField));
	}
}


int SqlTable::getIntField(const char* szField, int nNullValue/*=0*/)
{
	if (fieldIsNull(szField))
	{
		return nNullValue;
	}
	else
	{
		return atoi(fieldValue(szField));
	}
}


double SqlTable::getFloatField(int nField, double fNullValue/*=0.0*/)
{
	if (fieldIsNull(nField))
	{
		return fNullValue;
	}
	else
	{
		return atof(fieldValue(nField));
	}
}


double SqlTable::getFloatField(const char* szField, double fNullValue/*=0.0*/)
{
	if (fieldIsNull(szField))
	{
		return fNullValue;
	}
	else
	{
		return atof(fieldValue(szField));
	}
}


const char* SqlTable::getStringField(int nField, const char* szNullValue/*=""*/)
{
	if (fieldIsNull(nField))
	{
		return szNullValue;
	}
	else
	{
		return fieldValue(nField);
	}
}


const char* SqlTable::getStringField(const char* szField, const char* szNullValue/*=""*/)
{
	if (fieldIsNull(szField))
	{
		return szNullValue;
	}
	else
	{
		return fieldValue(szField);
	}
}


bool SqlTable::fieldIsNull(int nField)
{
	checkResults();
	return (fieldValue(nField) == 0);
}


bool SqlTable::fieldIsNull(const char* szField)
{
	checkResults();
	return (fieldValue(szField) == 0);
}


const char* SqlTable::fieldName(int nCol)
{
	checkResults();

	if (nCol < 0 || nCol > mnCols-1)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Invalid field index requested",
								DONT_DELETE_MSG);
	}

	return mpaszResults[nCol];
}


void SqlTable::setRow(int nRow)
{
	checkResults();

	if (nRow < 0 || nRow > mnRows-1)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Invalid row index requested",
								DONT_DELETE_MSG);
	}

	mnCurrentRow = nRow;
}


void SqlTable::checkResults()
{
	if (mpaszResults == 0)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Null Results pointer",
								DONT_DELETE_MSG);
	}
}


////////////////////////////////////////////////////////////////////////////////

SqlStatement::SqlStatement()
{
	mpDB = 0;
	mpVM = 0;
}


SqlStatement::SqlStatement(const SqlStatement& rStatement)
{
	mpDB = rStatement.mpDB;
	mpVM = rStatement.mpVM;
	// Only one object can own VM
	const_cast<SqlStatement&>(rStatement).mpVM = 0;
}


SqlStatement::SqlStatement(sqlite3* pDB, sqlite3_stmt* pVM)
{
	mpDB = pDB;
	mpVM = pVM;
}


SqlStatement::~SqlStatement()
{
	try
	{
		finalize();
	}
	catch (...)
	{
	}
}


SqlStatement& SqlStatement::operator=(const SqlStatement& rStatement)
{
	mpDB = rStatement.mpDB;
	mpVM = rStatement.mpVM;
	// Only one object can own VM
	const_cast<SqlStatement&>(rStatement).mpVM = 0;
	return *this;
}


int SqlStatement::execDML()
{
	checkDB();
	checkVM();

	const char* szError=0;

	int nRet = sqlite3_step(mpVM);

	if (nRet == SQLITE_DONE)
	{
		int nRowsChanged = sqlite3_changes(mpDB);

		nRet = sqlite3_reset(mpVM);

		if (nRet != SQLITE_OK)
		{
			szError = sqlite3_errmsg(mpDB);
			throw SqlException(nRet, (char*)szError, DONT_DELETE_MSG);
		}

		return nRowsChanged;
	}
	else
	{
		nRet = sqlite3_reset(mpVM);
		szError = sqlite3_errmsg(mpDB);
		throw SqlException(nRet, (char*)szError, DONT_DELETE_MSG);
	}
}


SqlQuery SqlStatement::execQuery()
{
	checkDB();
	checkVM();

	int nRet = sqlite3_step(mpVM);

	if (nRet == SQLITE_DONE)
	{
		// no rows
		return SqlQuery(mpDB, mpVM, true/*eof*/, false);
	}
	else if (nRet == SQLITE_ROW)
	{
		// at least 1 row
		return SqlQuery(mpDB, mpVM, false/*eof*/, false);
	}
	else
	{
		nRet = sqlite3_reset(mpVM);
		const char* szError = sqlite3_errmsg(mpDB);
		throw SqlException(nRet, (char*)szError, DONT_DELETE_MSG);
	}
}


void SqlStatement::bind(int nParam, const char* szValue)
{
	checkVM();
	int nRes = sqlite3_bind_text(mpVM, nParam, szValue, -1, SQLITE_TRANSIENT);

	if (nRes != SQLITE_OK)
	{
		throw SqlException(nRes,
								(char *)"Error binding string param",
								DONT_DELETE_MSG);
	}
}


void SqlStatement::bind(int nParam, const int nValue)
{
	checkVM();
	int nRes = sqlite3_bind_int(mpVM, nParam, nValue);

	if (nRes != SQLITE_OK)
	{
		throw SqlException(nRes,
								(char *)"Error binding int param",
								DONT_DELETE_MSG);
	}
}


void SqlStatement::bind(int nParam, const double dValue)
{
	checkVM();
	int nRes = sqlite3_bind_double(mpVM, nParam, dValue);

	if (nRes != SQLITE_OK)
	{
		throw SqlException(nRes,
								(char *)"Error binding double param",
								DONT_DELETE_MSG);
	}
}


void SqlStatement::bind(int nParam, const unsigned char* blobValue, int nLen)
{
	checkVM();
	int nRes = sqlite3_bind_blob(mpVM, nParam,
								(const void*)blobValue, nLen, SQLITE_TRANSIENT);

	if (nRes != SQLITE_OK)
	{
		throw SqlException(nRes,
								(char *)"Error binding blob param",
								DONT_DELETE_MSG);
	}
}

	
void SqlStatement::bindNull(int nParam)
{
	checkVM();
	int nRes = sqlite3_bind_null(mpVM, nParam);

	if (nRes != SQLITE_OK)
	{
		throw SqlException(nRes,
								(char *)"Error binding NULL param",
								DONT_DELETE_MSG);
	}
}


void SqlStatement::reset()
{
	if (mpVM)
	{
		int nRet = sqlite3_reset(mpVM);

		if (nRet != SQLITE_OK)
		{
			const char* szError = sqlite3_errmsg(mpDB);
			throw SqlException(nRet, (char*)szError, DONT_DELETE_MSG);
		}
	}
}


void SqlStatement::finalize()
{
	if (mpVM)
	{
		int nRet = sqlite3_finalize(mpVM);
		mpVM = 0;

		if (nRet != SQLITE_OK)
		{
			const char* szError = sqlite3_errmsg(mpDB);
			throw SqlException(nRet, (char*)szError, DONT_DELETE_MSG);
		}
	}
}


void SqlStatement::checkDB()
{
	if (mpDB == 0)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Database not open",
								DONT_DELETE_MSG);
	}
}


void SqlStatement::checkVM()
{
	if (mpVM == 0)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Null Virtual Machine pointer",
								DONT_DELETE_MSG);
	}
}


////////////////////////////////////////////////////////////////////////////////

SqlDB::SqlDB()
{
	mpDB = 0;
	mnBusyTimeoutMs = 60000; // 60 seconds
}


SqlDB::SqlDB(const SqlDB& db)
{
	mpDB = db.mpDB;
	mnBusyTimeoutMs = 60000; // 60 seconds
}


SqlDB::~SqlDB()
{
	close();
}


SqlDB& SqlDB::operator=(const SqlDB& db)
{
	mpDB = db.mpDB;
	mnBusyTimeoutMs = 60000; // 60 seconds
	return *this;
}


void SqlDB::open(const char* szFile)
{
	int nRet = sqlite3_open(szFile, &mpDB);

	if (nRet != SQLITE_OK)
	{
		const char* szError = sqlite3_errmsg(mpDB);
		throw SqlException(nRet, (char*)szError, DONT_DELETE_MSG);
	}

	setBusyTimeout(mnBusyTimeoutMs);
}


void SqlDB::close()
{
	if (mpDB)
	{
		sqlite3_close(mpDB);
		mpDB = 0;
	}
}


SqlStatement SqlDB::compileStatement(const char* szSQL)
{
	checkDB();

	sqlite3_stmt* pVM = compile(szSQL);
	return SqlStatement(mpDB, pVM);
}


bool SqlDB::tableExists(const char* szTable)
{
	char szSQL[128];
	sprintf(szSQL,
			"select count(*) from sqlite_master where type='table' and name='%s'",
			szTable);
	int nRet = execScalar(szSQL);
	return (nRet > 0);
}


int SqlDB::execDML(const char* szSQL)
{
	checkDB();

	char* szError=0;

	int nRet = sqlite3_exec(mpDB, szSQL, 0, 0, &szError);

	if (nRet == SQLITE_OK)
	{
		return sqlite3_changes(mpDB);
	}
	else
	{
		throw SqlException(nRet, szError);
	}
}


SqlQuery SqlDB::execQuery(const char* szSQL)
{
	checkDB();

	sqlite3_stmt* pVM = compile(szSQL);

	int nRet = sqlite3_step(pVM);

	if (nRet == SQLITE_DONE)
	{
		// no rows
		return SqlQuery(mpDB, pVM, true/*eof*/);
	}
	else if (nRet == SQLITE_ROW)
	{
		// at least 1 row
		return SqlQuery(mpDB, pVM, false/*eof*/);
	}
	else
	{
		nRet = sqlite3_finalize(pVM);
		const char* szError= sqlite3_errmsg(mpDB);
		throw SqlException(nRet, (char*)szError, DONT_DELETE_MSG);
	}
}


int SqlDB::execScalar(const char* szSQL)
{
	SqlQuery q = execQuery(szSQL);

	if (q.eof() || q.numFields() < 1)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Invalid scalar query",
								DONT_DELETE_MSG);
	}

	return atoi(q.fieldValue(0));
}


SqlTable SqlDB::getTable(const char* szSQL)
{
	checkDB();

	char* szError=0;
	char** paszResults=0;
	int nRet;
	int nRows(0);
	int nCols(0);

	nRet = sqlite3_get_table(mpDB, szSQL, &paszResults, &nRows, &nCols, &szError);

	if (nRet == SQLITE_OK)
	{
		return SqlTable(paszResults, nRows, nCols);
	}
	else
	{
		throw SqlException(nRet, szError);
	}
}


sqlite_int64 SqlDB::lastRowId()
{
	return sqlite3_last_insert_rowid(mpDB);
}


void SqlDB::setBusyTimeout(int nMillisecs)
{
	mnBusyTimeoutMs = nMillisecs;
	sqlite3_busy_timeout(mpDB, mnBusyTimeoutMs);
}


void SqlDB::checkDB()
{
	if (!mpDB)
	{
		throw SqlException(MY_SQLITE_ERROR,
								(char *)"Database not open",
								DONT_DELETE_MSG);
	}
}


sqlite3_stmt* SqlDB::compile(const char* szSQL)
{
	checkDB();

	char* szError=0;
	const char* szTail=0;
	sqlite3_stmt* pVM;

	int nRet = sqlite3_prepare(mpDB, szSQL, -1, &pVM, &szTail);

	if (nRet != SQLITE_OK)
	{
		throw SqlException(nRet, szError);
	}

	return pVM;
}

