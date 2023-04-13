#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#include <config_parser.h>

/* 检测返回代码是否为成功标志，当为成功标志返回 TRUE，否则返回 FALSE */
#define RC_SUCCESSFUL(rc) ((rc) == SQL_SUCCESS || (rc) == SQL_SUCCESS_WITH_INFO)
/* 检测返回代码是否为失败标志，当为失败标志返回 TRUE，否则返回 FALSE */
#define RC_NOTSUCCESSFUL(rc) (!(RC_SUCCESSFUL(rc)))

#define FREE_HANDLE(str, rc, henv, hdbc, is_exit) \
    if (RC_NOTSUCCESSFUL(rc))                     \
    {                                             \
        printf(str);                              \
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);      \
        SQLFreeHandle(SQL_HANDLE_ENV, henv);      \
        if (is_exit)                              \
            exit(0);                              \
    }

HENV henv;        /* 环境句柄 */
HDBC hdbc;        /* 连接句柄 */
HSTMT hstmt;      /* 语句句柄 */
SQLRETURN sret;   /* 返回代码 */
SQLLEN row_count; /* 返回行数 */

int main(void)
{
    db_config config = read_config();

    int out_c1 = 0;
    SQLCHAR out_c2[20] = {0};
    SQLLEN out_c1_ind = 0;
    SQLLEN out_c2_ind = 0;

    /* 申请句柄 */
    SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);
    SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
    SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

    sret = SQLConnect(hdbc,
                      (SQLCHAR *)config.db_name, SQL_NTS,
                      (SQLCHAR *)config.db_user, SQL_NTS,
                      (SQLCHAR *)config.db_pwd, SQL_NTS);
    FREE_HANDLE("odbc: fail to connect to server!\n", sret, henv, hdbc, 1);
    printf("odbc: connect to server success!\n");

    /* 申请一个语句句柄 */
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

    // 如果 sql 执行失败必须释放 重新申请 不然下面的全部都会失败
    // 查询
    SQLCHAR sql[] = "SELECT * FROM DBA_OBJECTS WHERE OBJECT_TYPE='SCH' AND OBJECT_NAME ='EXAMPLE_SCHEMA' AND STATUS='VALID'";
    sret = SQLExecDirect(hstmt, sql, SQL_NTS);
    // row count 不能用于 select 查询，仅适用于 delete update insert 这种更新行
    // SQLRowCount(hstmt, &row_count);

    // select use SQLFetch
    // Fetch result set rows
    while (SQLFetch(hstmt) == SQL_SUCCESS)
    {
        row_count++;
    }
    printf("odbc: handle sql %s \n sret{%d} row{%ld}\n", (char *)sql, sret, row_count);
    if (!row_count || RC_NOTSUCCESSFUL(sret))
    {

        FREE_HANDLE("odbc: select fail!\n", sret, henv, hdbc, 0);
        SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

        printf("odbc: no EXAMPLE_SCHEMA then create\n");
        sret = SQLExecDirect(hstmt, (SQLCHAR *)"CREATE SCHEMA EXAMPLE_SCHEMA", SQL_NTS);
        // SQLRowCount returns -1 成功也是 -1 ？？？
        if (RC_NOTSUCCESSFUL(sret))
        {
            FREE_HANDLE("odbc: create schema fail!\n", sret, henv, hdbc, 0);
            SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
        }
        sret = SQLExecDirect(hstmt, (SQLCHAR *)"CREATE TABLE TEST(ID INT NOT NULL, NAME VARCHAR(20))", SQL_NTS);
        if (RC_NOTSUCCESSFUL(sret))
        {
            FREE_HANDLE("odbc: create table fail!\n", sret, henv, hdbc, 0);
            SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
        }
    }

    // SELECT TABLEDEF('SYSDBA','TEST');
    // 插入数据
    sret = SQLExecDirect(hstmt, (SQLCHAR *)"INSERT INTO TEST(ID, NAME) VALUES('1', 'CHINESE'), ('2', 'MATH'), ('3', 'ENGLISH'), ('4', 'GYM')", SQL_NTS);
    if (RC_NOTSUCCESSFUL(sret))
    {
        FREE_HANDLE("odbc: insert fail!\n", sret, henv, hdbc, 1);
    }
    printf("odbc: insert success\n");

    // 删除数据
    sret = SQLExecDirect(hstmt, (SQLCHAR *)"DELETE FROM TEST WHERE NAME='MATH' ", SQL_NTS);
    if (RC_NOTSUCCESSFUL(sret))
    {
        FREE_HANDLE("odbc: delete fail!\n", sret, henv, hdbc, 1);
    }
    printf("odbc: delete success\n");

    // 更新数据
    sret = SQLExecDirect(hstmt, (SQLCHAR *)"UPDATE TEST SET NAME = 'ENGLISH-NEW' WHERE NAME='ENGLISH' ", SQL_NTS);
    if (RC_NOTSUCCESSFUL(sret))
    {
        FREE_HANDLE("odbc: update fail!\n", sret, henv, hdbc, 1);
    }
    printf("odbc: update success\n");

    // 查询数据
    sret = SQLExecDirect(hstmt, (SQLCHAR *)"SELECT * FROM TEST", SQL_NTS);
    if (RC_NOTSUCCESSFUL(sret))
    {
        FREE_HANDLE("odbc: select fail!\n", sret, henv, hdbc, 1);
    }
    SQLBindCol(hstmt, 1, SQL_C_SLONG, &out_c1, sizeof(out_c1), &out_c1_ind);
    SQLBindCol(hstmt, 2, SQL_C_CHAR, &out_c2, sizeof(out_c2), &out_c2_ind);

    printf("odbc: select from table...\n");
    while (SQLFetch(hstmt) != SQL_NO_DATA)
    {
        printf("c1 = %d, c2 = %s ,\n", out_c1, out_c2);
    }
    printf("odbc: select success\n");

    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

    SQLDisconnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);
    return 0;
}
