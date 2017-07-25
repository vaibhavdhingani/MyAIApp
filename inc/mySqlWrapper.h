#ifndef _MY_SQL_WRAPPER_H_
#define _MY_SQL_WRAPPER_H_

#include "sqlite3.h"
#include <cstdio>
#include <cstring>

#define MY_SQLITE_ERROR 1000

class SqlException
{
public:
    SqlException(const int nErrCode,
                    char* szErrMess,
                    bool bDeleteMsg=true);
    SqlException(const SqlException&  e);

    virtual ~SqlException();
    const int errorCode() { return mnErrCode; }
    const char* errorMessage() { return mpszErrMess; }
    static const char* errorCodeAsString(int nErrCode);

private:
    int mnErrCode;
    char* mpszErrMess;
};


class SqlBuffer
{
public:

    SqlBuffer();

    ~SqlBuffer();

    const char* format(const char* szFormat, ...);

    operator const char*() { return mpBuf; }

    void clear();

private:

    char* mpBuf;
};


class SqlQuery
{
public:
    SqlQuery();
    SqlQuery(const SqlQuery& rQuery);
    SqlQuery(sqlite3* pDB,
				sqlite3_stmt* pVM,
                bool bEof,
                bool bOwnVM=true);

    SqlQuery& operator=(const SqlQuery& rQuery);

    virtual ~SqlQuery();

    int numFields();

    int fieldIndex(const char* szField);
    const char* fieldName(int nCol);

    const char* fieldDeclType(int nCol);
    int fieldDataType(int nCol);

    const char* fieldValue(int nField);
    const char* fieldValue(const char* szField);

    int getIntField(int nField, int nNullValue=0);
    int getIntField(const char* szField, int nNullValue=0);

    double getFloatField(int nField, double fNullValue=0.0);
    double getFloatField(const char* szField, double fNullValue=0.0);

    const char* getStringField(int nField, const char* szNullValue="");
    const char* getStringField(const char* szField, const char* szNullValue="");

    const unsigned char* getBlobField(int nField, int& nLen);
    const unsigned char* getBlobField(const char* szField, int& nLen);

    bool fieldIsNull(int nField);
    bool fieldIsNull(const char* szField);

    bool eof();
    void nextRow();
    void finalize();

private:
    void checkVM();

	sqlite3* mpDB;
    sqlite3_stmt* mpVM;
    bool mbEof;
    int mnCols;
    bool mbOwnVM;
};


class SqlTable
{
public:

    SqlTable();
    SqlTable(const SqlTable& rTable);
    SqlTable(char** paszResults, int nRows, int nCols);

    virtual ~SqlTable();

    SqlTable& operator=(const SqlTable& rTable);

    int numFields();

    int numRows();

    const char* fieldName(int nCol);

    const char* fieldValue(int nField);
    const char* fieldValue(const char* szField);

    int getIntField(int nField, int nNullValue=0);
    int getIntField(const char* szField, int nNullValue=0);

    double getFloatField(int nField, double fNullValue=0.0);
    double getFloatField(const char* szField, double fNullValue=0.0);

    const char* getStringField(int nField, const char* szNullValue="");
    const char* getStringField(const char* szField, const char* szNullValue="");

    bool fieldIsNull(int nField);
    bool fieldIsNull(const char* szField);

    void setRow(int nRow);

    void finalize();

private:

    void checkResults();

    int mnCols;
    int mnRows;
    int mnCurrentRow;
    char** mpaszResults;
};


class SqlStatement
{
public:

    SqlStatement();

    SqlStatement(const SqlStatement& rStatement);

    SqlStatement(sqlite3* pDB, sqlite3_stmt* pVM);

    virtual ~SqlStatement();

    SqlStatement& operator=(const SqlStatement& rStatement);

    int execDML();

    SqlQuery execQuery();

    void bind(int nParam, const char* szValue);
    void bind(int nParam, const int nValue);
    void bind(int nParam, const double dwValue);
    void bind(int nParam, const unsigned char* blobValue, int nLen);
    void bindNull(int nParam);

    void reset();

    void finalize();

private:

    void checkDB();
    void checkVM();

    sqlite3* mpDB;
    sqlite3_stmt* mpVM;
};


class SqlDB
{
public:

    SqlDB();
    virtual ~SqlDB();
    void open(const char* szFile);
    void close();
	bool tableExists(const char* szTable);
    int execDML(const char* szSQL);
    SqlQuery execQuery(const char* szSQL);
    int execScalar(const char* szSQL);
    SqlTable getTable(const char* szSQL);
    SqlStatement compileStatement(const char* szSQL);
    sqlite_int64 lastRowId();
    void interrupt() { sqlite3_interrupt(mpDB); }
    void setBusyTimeout(int nMillisecs);
    static const char* SQLiteVersion() { return SQLITE_VERSION; }

private:

    SqlDB(const SqlDB& db);
    SqlDB& operator=(const SqlDB& db);
    sqlite3_stmt* compile(const char* szSQL);
    void checkDB();
    sqlite3* mpDB;
    int mnBusyTimeoutMs;
};

#endif
