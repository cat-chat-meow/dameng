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

HENV henv;      /* 环境句柄 */
HDBC hdbc;      /* 连接句柄 */
HSTMT hstmt;    /* 语句句柄 */
SQLRETURN sret; /* 返回代码 */

int main(void)
{
    db_config config = read_config();

    SQLCHAR sql[] = "insert into TEST001(NAME) values(?)";
    SQLCHAR in_c1[20] = {0};
    SQLLEN in_c1_ind_ptr;

    memcpy(in_c1, "物理", 8);
    in_c1_ind_ptr = 8;

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

    // 创建表
    sret = SQLExecDirect(hstmt, (SQLCHAR *)"CREATE TABLE TEST001(ID INT, NAME VARCHAR(20))", SQL_NTS);
    if (RC_NOTSUCCESSFUL(sret))
    {
        FREE_HANDLE("odbc: create table fail!\n", sret, henv, hdbc, 0);
        SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    }
    printf("odbc: create table success!\n");

    // 清空表，初始化测试环境
    sret = SQLExecDirect(hstmt, (SQLCHAR *)"delete from TEST001", SQL_NTS);

    // 绑定参数方式插入数据
    printf("insert with bind..\n sql: %s\n para: %s\n", (char *)sql, (char *)in_c1);
    sret = SQLPrepare(hstmt, sql, SQL_NTS);
    sret = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR,
                            sizeof(in_c1) / sizeof(SQLLEN), 0, in_c1, 0, &in_c1_ind_ptr);
    sret = SQLExecute(hstmt);
    if (RC_NOTSUCCESSFUL(sret))
    {
        FREE_HANDLE("odbc: insert into table with bind fail!\n", sret, henv, hdbc, 0);
        SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    }
    printf("odbc: insert into table with bind success!\n");

    // 查询数据
    SQLExecDirect(hstmt, (SQLCHAR *)"select * from TEST001", SQL_NTS);
    SQLBindCol(hstmt, 1, SQL_C_SLONG, &out_c1, sizeof(out_c1) / sizeof(SQLLEN), &out_c1_ind);
    SQLBindCol(hstmt, 2, SQL_C_CHAR, &out_c2, sizeof(out_c2) / sizeof(SQLLEN), &out_c2_ind);

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
