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

HENV henv;        /* 环境句柄 */
HDBC hdbc;        /* 连接句柄 */
HSTMT hstmt;      /* 语句句柄 */
SQLRETURN sret;   /* 返回代码 */
SQLLEN row_count; /* 返回行数 */

int main(void)
{
    db_config config = read_config("config.ini");

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
    if (RC_NOTSUCCESSFUL(sret))
    {
        printf("odbc: fail to connect to server!\n");
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        exit(0);
    }
    printf("odbc: connect to server success!\n");

    /* 申请一个语句句柄 */
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

    // 查询
    SQLCHAR sql[] = "select * from dba_objects where object_type='SCH' and OBJECT_NAME ='EXAMPLE_SCHEMA' and STATUS='VALID'";
    sret = SQLExecDirect(hstmt, sql, SQL_NTS);
    SQLRowCount(hstmt, &row_count);
    if (!row_count)
    {
        printf("odbc: no EXAMPLE_SCHEMA then create\n");
        strcpy(sql, (SQLCHAR *)"create schema example_schema");
        sret = SQLExecDirect(hstmt, sql, SQL_NTS);
        // SQLRowCount returns -1 成功也是 -1 ？？？
        (sret) == SQL_NULL_DATA ? printf("odbc: create schema suc\n") : printf("odbc: create schema fail\n");
        strcpy(sql, (SQLCHAR *)"create table example_schema.example_table(name varchar)");
        sret = SQLExecDirect(hstmt, sql, SQL_NTS);
        (sret) == SQL_NULL_DATA ? printf("odbc: create table suc\n") : printf("odbc: create table fail\n");
    }

    // 插入数据
    strcpy(sql, (SQLCHAR *)"insert into example_schema.example_table(name) values('chinese'), ('math'), ('english'), ('gym') ");
    sret = SQLExecDirect(hstmt, sql, SQL_NTS);
    if (RC_NOTSUCCESSFUL(sret))
    {
        printf("odbc: insert fail\n");
        exit(0);
    }
    printf("odbc: insert success\n");

    // 删除数据
    strcpy(sql, (SQLCHAR *)"delete from example_schema.example_table where name='math' ");
    sret = SQLExecDirect(hstmt, sql, SQL_NTS);
    if (RC_NOTSUCCESSFUL(sret))
    {
        printf("odbc: delete fail\n");
        exit(0);
    }
    printf("odbc: delete success\n");

    // 更新数据
    strcpy(sql, (SQLCHAR *)"update example_schema.example_table set name = 'english-new' where name='english' ");
    sret = SQLExecDirect(hstmt, sql, SQL_NTS);
    if (RC_NOTSUCCESSFUL(sret))
    {
        printf("odbc: update fail\n");
        exit(0);
    }
    printf("odbc: update success\n");

    // 查询数据
    strcpy(sql, (SQLCHAR *)"select * from example_schema.example_table");
    sret = SQLExecDirect(hstmt, sql, SQL_NTS);
    if (RC_NOTSUCCESSFUL(sret))
    {
        printf("odbc: select fail\n");
        exit(0);
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
