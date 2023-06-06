#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DPI.h"
#include "DPIext.h"
#include "DPItypes.h"

#include <config_parser.h>

dhenv henv;   /* 环境句柄 */
dhcon hcon;   /* 连接句柄 */
dhstmt hstmt; /* 语句句柄 */
dhdesc hdesc; /* 描述符句柄 */
DPIRETURN rt; /* 函数返回值 */

enum enum_field_types
{
    DB_TYPE_NULL = 0,
    DB_TYPE_CHAR = DSQL_CHAR,
    DB_TYPE_VARCHAR = DSQL_VARCHAR, // use this char
    DB_TYPE_INT = DSQL_INT,         // int
    DB_TYPE_FLOAT = DSQL_FLOAT,
    DB_TYPE_DOUBLE = DSQL_DOUBLE,
    DB_TYPE_TIMESTAMP = DSQL_TIMESTAMP,
    DB_TYPE_CLOB = DSQL_CLOB,
    DB_TYPE_BLOB = DSQL_BLOB,
    DB_TYPE_MAX = 255
};

// int get_c_type(const enum_field_types type)
// {
//     switch (type)
//     {
//     case DB_TYPE_CHAR:
//     case DB_TYPE_VARCHAR:
//     case DB_TYPE_CLOB:
//     case DB_TYPE_BLOB:
//         // sdbyte
//         return DSQL_C_NCHAR;
//     case DB_TYPE_INT:
//         // sdint4
//         return DSQL_C_SLONG;
//     case DB_TYPE_FLOAT:
//         return DSQL_C_FLOAT;
//     case DB_TYPE_DOUBLE:
//         return DSQL_C_DOUBLE;
//     case DB_TYPE_TIMESTAMP:
//         return DSQL_C_TIMESTAMP;
//     default:
//         return 0;
//         break;
//     }
// }

/******************************************************
Notes:
定义相应常量
*******************************************************/
#define ROWS 10         // 数组绑定一次插入和读取的行数
#define CHARS 80 * 1024 // 一次读取和写入的字节数800K
#define FLEN 500        // 文件名长度(带地址路径)
#define DM_SVR "LOCALHOST"
#define DM_USER "SYSDBA"
#define DM_PWD "SYSDBA"
// 函数检查及错误信息显示
#define DPIRETURN_CHECK(rt, hndl_type, hndl) \
    if (!DSQL_SUCCEEDED(rt))                 \
    {                                        \
        dpi_err_msg_print(hndl_type, hndl);  \
        return rt;                           \
    }
#define FUN_CHECK(rt)        \
    if (!DSQL_SUCCEEDED(rt)) \
    {                        \
        goto END;            \
    }
/******************************************************
Notes:
定义常用句柄和变量
*******************************************************/
dhenv henv;         /* 环境句柄 */
dhcon hcon;         /* 连接句柄 */
dhstmt hstmt;       /* 语句句柄 */
dhdesc hdesc;       /* 描述符句柄 */
dhloblctr hloblctr; /* lob类型控制句柄 */
DPIRETURN rt;       /* 函数返回值 */
/******************************************************
Notes:
    错误信息获取打印
Param:
    hndl_type:	句柄类型
    hndl:		句柄
Return:
    无
*******************************************************/
void dpi_err_msg_print(sdint2 hndl_type, dhandle hndl)
{
    sdint4 err_code;
    sdint2 msg_len;
    sdbyte err_msg[SDBYTE_MAX];
    // 获取错误信息字段
    /*	dpi_get_diag_field(hndl_type, hndl, 1, DSQL_DIAG_MESSAGE_TEXT, err_msg, sizeof(err_msg), NULL);
        printf("err_msg = %s\n", err_msg);*/
    // 获取错误信息集合
    dpi_get_diag_rec(hndl_type, hndl, 1, &err_code, err_msg, sizeof(err_msg), &msg_len);
    printf("err_msg = %s, err_code = %d\n", err_msg, err_code);
}
/******************************************************
Notes:
    连接数据库
Param:
    server: 服务器IP
    uid:	数据库登录账号
    pwd:	数据库登录密码
Return:
    DSQL_SUCCESS 执行成功
    DSQL_ERROR   执行失败
*******************************************************/
DPIRETURN
dm_dpi_connect(sdbyte *server, sdbyte *uid, sdbyte *pwd)
{
    // 申请环境句柄
    rt = dpi_alloc_env(&henv);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_ENV, henv);
    // 申请连接句柄
    rt = dpi_alloc_con(henv, &hcon);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
    // 连接数据库服务器
    rt = dpi_login(hcon, server, uid, pwd);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
    return DSQL_SUCCESS;
}
/************************************************
    断开数据库连接
************************************************/
DPIRETURN
dm_dpi_disconnect()
{
    // 断开连接
    rt = dpi_logout(hcon);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
    // 释放连接句柄和环境句柄
    rt = dpi_free_con(hcon);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
    rt = dpi_free_env(henv);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_ENV, henv);
    return DSQL_SUCCESS;
}
/************************************************
    初始化表
************************************************/
DPIRETURN
dm_init_table()
{
    // 申请语句句柄
    rt = dpi_alloc_stmt(hcon, &hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
    // 执行sql
    dpi_exec_direct(hstmt, "drop table dpi_demo");
    rt = dpi_exec_direct(hstmt, "create table dpi_demo(c1 int, c2 char(20), c3 varchar(50), c4 numeric(7,3), c5 timestamp(5), c6 clob, c7 blob)");
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    // 释放语句句柄
    rt = dpi_free_stmt(hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);

    printf("dm init table success\n");
    return DSQL_SUCCESS;
}
/************************************************
    通过参数绑定的方式执行sql语句
************************************************/
DPIRETURN
dm_insert_with_bind_param()
{
    sdint4 c1 = 0; // 与字段匹配的变量
    sdbyte c2[10];
    sdbyte c3[10];
    ddouble c4;
    dpi_timestamp_t c5;
    sdbyte c6[18];
    sdbyte c7[18];
    slength c1_ind_ptr;
    slength c2_ind_ptr; // 缓冲区长度
    slength c3_ind_ptr;
    slength c4_ind_ptr = 0;
    slength c5_ind_ptr = 0;
    slength c6_ind_ptr;
    slength c7_ind_ptr;
    // 分配语句句柄
    rt = dpi_alloc_stmt(hcon, &hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
    // 分配语句句柄
    rt = dpi_prepare(hstmt, "insert into dpi_demo(c1,c2,c3,c4,c5,c6,c7) values(?,?,?,?,?,?,?)");
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    // 字段变量赋值
    c1 = 201410;
    memcpy(c2, "abcde", 5);
    memcpy(c3, "abcdefghi", 9);
    c4 = 0.009;
    c5.year = 2011;
    c5.month = 3;
    c5.day = 1;
    c5.hour = 11;
    c5.minute = 45;
    c5.second = 50;
    c5.fraction = 900000000;
    memcpy(c6, "adfadsfetre2345ert", 18);
    memcpy(c7, "1234567890abcdef12", 18);
    c1_ind_ptr = sizeof(c1);
    c2_ind_ptr = 5; // 获取缓冲区长度
    c3_ind_ptr = 9;
    c4_ind_ptr = sizeof(c4);
    c5_ind_ptr = sizeof(c5);
    c6_ind_ptr = 18;
    c7_ind_ptr = 18;
    // 绑定参数
    rt = dpi_bind_param(hstmt, 1, DSQL_PARAM_INPUT, DSQL_C_SLONG, DSQL_INT, sizeof(c1), 0, &c1, sizeof(c1), &c1_ind_ptr);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    rt = dpi_bind_param(hstmt, 2, DSQL_PARAM_INPUT, DSQL_C_NCHAR, DSQL_CHAR, sizeof(c2), 0, c2, sizeof(c2), &c2_ind_ptr);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    rt = dpi_bind_param(hstmt, 3, DSQL_PARAM_INPUT, DSQL_C_NCHAR, DSQL_VARCHAR, sizeof(c3), 0, c3, sizeof(c3), &c3_ind_ptr);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    rt = dpi_bind_param(hstmt, 4, DSQL_PARAM_INPUT, DSQL_C_DOUBLE, DSQL_DOUBLE, sizeof(c4), 0, &c4, sizeof(c4), &c4_ind_ptr);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    rt = dpi_bind_param(hstmt, 5, DSQL_PARAM_INPUT, DSQL_C_TIMESTAMP, DSQL_TIMESTAMP, sizeof(c5), 0, &c5, sizeof(c5), &c5_ind_ptr);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    rt = dpi_bind_param(hstmt, 6, DSQL_PARAM_INPUT, DSQL_C_NCHAR, DSQL_CLOB, sizeof(c6), 0, c6, sizeof(c6), &c6_ind_ptr);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    rt = dpi_bind_param(hstmt, 7, DSQL_PARAM_INPUT, DSQL_C_NCHAR, DSQL_BLOB, sizeof(c7), 0, c7, sizeof(c7), &c7_ind_ptr);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    // 执行Dsql
    rt = dpi_exec(hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    // 释放语句句柄
    rt = dpi_free_stmt(hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    printf("dm insert with bind param success\n");
    return DSQL_SUCCESS;
}
/************************************************
    通过参数绑定数组的方式执行sql语句
************************************************/
DPIRETURN
dm_insert_with_bind_array()
{
    sdint4 c1[ROWS]; // 定义字段相应的变量
    sdbyte c2[ROWS][10];
    sdbyte c3[ROWS][10];
    ddouble c4[ROWS];
    dpi_timestamp_t c5[ROWS];
    sdbyte c6[ROWS][18];
    sdbyte c7[ROWS][18];
    slength c1_ind_ptr[ROWS]; // 缓冲区长度
    slength c2_ind_ptr[ROWS]; // 缓冲区长度
    slength c3_ind_ptr[ROWS]; // 缓冲区长度
    slength c4_ind_ptr[ROWS]; // 缓冲区长度
    slength c5_ind_ptr[ROWS]; // 缓冲区长度
    slength c6_ind_ptr[ROWS];
    slength c7_ind_ptr[ROWS];
    int i;
    int i_array_rows = ROWS;
    // 分配语句句柄
    rt = dpi_alloc_stmt(hcon, &hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
    // 设置语句句柄属性
    rt = dpi_set_stmt_attr(hstmt, DSQL_ATTR_PARAMSET_SIZE, (dpointer)i_array_rows,
                           sizeof(i_array_rows));
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    // 准备sql
    rt = dpi_prepare(hstmt, "insert into dpi_demo(c1,c2,c3,c4,c5,c6,c7) values(?,?,?,?,?,?,?)");
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    // 赋值
    for (i = 0; i < i_array_rows; i++)
    {
        c1[i] = i + 10;
        memcpy(c2[i], "abcde", 5);
        memcpy(c3[i], "abcdefghi", 9);
        c4[i] = 0.00901;
        c5[i].year = 2011;
        c5[i].month = 3;
        c5[i].day = 1;
        c5[i].hour = 11;
        c5[i].minute = 45;
        c5[i].second = 50;
        c5[i].fraction = 900;
        memcpy(c6[i], "adfadsfetre2345ert", 18);
        memcpy(c7[i], "1234567890abcdef12", 18);
        c1_ind_ptr[i] = sizeof(c1[i]);
        c2_ind_ptr[i] = 5; // 获取缓冲区长度
        c3_ind_ptr[i] = 9;
        c4_ind_ptr[i] = sizeof(c4[i]);
        c5_ind_ptr[i] = sizeof(c5[i]);
        c6_ind_ptr[i] = 18;
        c7_ind_ptr[i] = 18;
    }
    // 绑定参数
    rt = dpi_bind_param(hstmt, 1, DSQL_PARAM_INPUT, DSQL_C_SLONG, DSQL_INT, sizeof(c1[0]), 0, &c1[0], sizeof(c1[0]), &c1_ind_ptr[0]);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    rt = dpi_bind_param(hstmt, 2, DSQL_PARAM_INPUT, DSQL_C_NCHAR, DSQL_CHAR,
                        sizeof(c2[0]), 0, c2[0], sizeof(c2[0]), &c2_ind_ptr[0]);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    rt = dpi_bind_param(hstmt, 3, DSQL_PARAM_INPUT, DSQL_C_NCHAR, DSQL_VARCHAR,
                        sizeof(c3[0]), 0, c3[0], sizeof(c3[0]), &c3_ind_ptr[0]);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    rt = dpi_bind_param(hstmt, 4, DSQL_PARAM_INPUT, DSQL_C_DOUBLE, DSQL_DOUBLE,
                        sizeof(c4[0]), 0, &c4[0], sizeof(c4[0]), (slength *)&c4_ind_ptr[0]);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    rt = dpi_bind_param(hstmt, 5, DSQL_PARAM_INPUT, DSQL_C_TIMESTAMP,
                        DSQL_TIMESTAMP, sizeof(c5[0]), 0, &c5[0], sizeof(c5[0]), (slength *)&c5_ind_ptr[0]);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    rt = dpi_bind_param(hstmt, 6, DSQL_PARAM_INPUT, DSQL_C_NCHAR, DSQL_CLOB,
                        sizeof(c6[0]), 0, c6[0], sizeof(c6[0]), &c6_ind_ptr[0]);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    rt = dpi_bind_param(hstmt, 7, DSQL_PARAM_INPUT, DSQL_C_NCHAR, DSQL_BLOB,
                        sizeof(c7[0]), 0, c7[0], sizeof(c7[0]), &c7_ind_ptr[0]);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    // 执行Dsql
    rt = dpi_exec(hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    // 释放语句句柄
    rt = dpi_free_stmt(hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    printf("dm insert with bind array success\n");
    return DSQL_SUCCESS;
}
/************************************************
    fetch获取结果集
************************************************/
DPIRETURN
dm_select_with_fetch()
{
    sdint4 c1 = 0; // 与字段匹配的变量，用于获取字段值
    sdbyte c2[20];
    sdbyte c3[50];
    ddouble c4;
    dpi_timestamp_t c5;
    sdbyte c6[50];
    sdbyte c7[FLEN];
    slength c1_ind = 0; // 缓冲区
    slength c2_ind = 0;
    slength c3_ind = 0;
    slength c4_ind = 0;
    slength c5_ind = 0;
    slength c6_ind = 0;
    slength c7_ind = 0;
    ulength row_num; // 行数
    sdint4 dataflag = 0;
    // 分配语句句柄
    DPIRETURN_CHECK(dpi_alloc_stmt(hcon, &hstmt), DSQL_HANDLE_STMT, hstmt);
    // 执行sql语句
    DPIRETURN_CHECK(dpi_exec_direct(hstmt, "select c1,c2,c3,c4,c5,c6,c7 from dpi_demo"), DSQL_HANDLE_STMT, hstmt);
    // 绑定输出列
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 1, DSQL_C_SLONG, &c1, sizeof(c1), &c1_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 2, DSQL_C_NCHAR, &c2, sizeof(c2), &c2_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 3, DSQL_C_NCHAR, &c3, sizeof(c3), &c3_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 4, DSQL_C_DOUBLE, &c4, sizeof(c4), &c4_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 5, DSQL_C_TIMESTAMP, &c5, sizeof(c5), &c5_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 6, DSQL_C_NCHAR, &c6, sizeof(c6), &c6_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 7, DSQL_C_NCHAR, &c7, sizeof(c7), &c7_ind), DSQL_HANDLE_STMT, hstmt);
    printf("dm_select_with_fetch......\n");
    printf("----------------------------------------------------------------------\n");
    while (dpi_fetch(hstmt, &row_num) != DSQL_NO_DATA)
    {
        printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
        printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
        printf("c6 = %s, c7 = %s\n", c6, c7);
        dataflag = 1;
    }
    printf("----------------------------------------------------------------------\n");
    if (!dataflag)
    {
        printf("dm no data\n");
    }
    /* 释放语句句柄 */
    DPIRETURN_CHECK(dpi_free_stmt(hstmt), DSQL_HANDLE_STMT, hstmt);
    return DSQL_SUCCESS;
}
/************************************************
    使用参数绑定后再fetch获取结果集
************************************************/
DPIRETURN
dm_select_with_fetch_with_param()
{
    sdint4 c1 = 0; // 与字段匹配的变量，用于获取字段值
    slength c1_param_ind = 0;
    sdbyte c2[20];
    sdbyte c3[50];
    ddouble c4;
    dpi_timestamp_t c5;
    sdbyte c6[50];
    sdbyte c7[FLEN];
    slength c1_ind = 0; // 缓冲区
    slength c2_ind = 0;
    slength c3_ind = 0;
    slength c4_ind = 0;
    slength c5_ind = 0;
    slength c6_ind = 0;
    slength c7_ind = 0;
    ulength row_num; // 行数
    sdint4 dataflag = 0;
    c1 = 10; // 读取c1=10的数据
    // 分配语句句柄
    DPIRETURN_CHECK(dpi_alloc_stmt(hcon, &hstmt), DSQL_HANDLE_STMT, hstmt);
    // 准备sql
    DPIRETURN_CHECK(dpi_prepare(hstmt, "select c1,c2,c3,c4,c5,c6,c7 from dpi_demo where c1 = ?"), DSQL_HANDLE_STMT, hstmt);
    // 绑定参数
    DPIRETURN_CHECK(dpi_bind_param(hstmt, 1, DSQL_PARAM_INPUT, DSQL_C_STINYINT, DSQL_INT, sizeof(c1), 0, &c1, sizeof(c1), &c1_param_ind), DSQL_HANDLE_STMT, hstmt);
    // 执行sql
    DPIRETURN_CHECK(dpi_exec(hstmt), DSQL_HANDLE_STMT, hstmt);
    // 绑定输出列
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 1, DSQL_C_SLONG, &c1, sizeof(c1), &c1_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 2, DSQL_C_NCHAR, &c2, sizeof(c2), &c2_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 3, DSQL_C_NCHAR, &c3, sizeof(c3), &c3_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 4, DSQL_C_DOUBLE, &c4, sizeof(c4), &c4_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 5, DSQL_C_TIMESTAMP, &c5, sizeof(c5), &c5_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 6, DSQL_C_NCHAR, &c6, sizeof(c6), &c6_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 7, DSQL_C_NCHAR, &c7, sizeof(c7), &c7_ind), DSQL_HANDLE_STMT, hstmt);
    // 打印输出信息
    printf("dm_select_with_fetch_with_param......\n");
    printf("----------------------------------------------------------------------\n");
    while (dpi_fetch(hstmt, &row_num) != DSQL_NO_DATA)
    {
        printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
        printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
        printf("c6 = %s, c7 = %s\n", c6, c7);
        dataflag = 1;
    }
    printf("----------------------------------------------------------------------\n");
    if (!dataflag)
    {
        printf("dm no data\n");
    }
    /* 释放语句句柄 */
    DPIRETURN_CHECK(dpi_free_stmt(hstmt), DSQL_HANDLE_STMT, hstmt);
    return DSQL_SUCCESS;
}
/************************************************
    fetch获取结果集,scroll结果集
************************************************/
DPIRETURN
dm_select_with_fetch_scroll()
{
    sdint4 c1 = 0; // 与字段匹配的变量，用于获取字段值
    sdbyte c2[20];
    sdbyte c3[50];
    ddouble c4;
    dpi_timestamp_t c5;
    sdbyte c6[50];
    sdbyte c7[FLEN];
    slength c1_ind = 0; // 缓冲区
    slength c2_ind = 0;
    slength c3_ind = 0;
    slength c4_ind = 0;
    slength c5_ind = 0;
    slength c6_ind = 0;
    slength c7_ind = 0;
    ulength row_num; // 行数
    ulength val = DSQL_CURSOR_DYNAMIC;
    sdint4 dataflag = 0;
    // 分配语句句柄
    DPIRETURN_CHECK(dpi_alloc_stmt(hcon, &hstmt), DSQL_HANDLE_STMT, hstmt);
    // 设置语句句柄属性
    DPIRETURN_CHECK(dpi_set_stmt_attr(hstmt, DSQL_ATTR_CURSOR_TYPE, (dpointer)val, 0), DSQL_HANDLE_STMT, hstmt);
    // 执行sql
    DPIRETURN_CHECK(dpi_exec_direct(hstmt, "select c1,c2,c3,c4,c5,c6,c7 from dpi_demo"), DSQL_HANDLE_STMT, hstmt);
    // 绑定输出列
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 1, DSQL_C_SLONG, &c1, sizeof(c1), &c1_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 2, DSQL_C_NCHAR, &c2, sizeof(c2), &c2_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 3, DSQL_C_NCHAR, &c3, sizeof(c3), &c3_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 4, DSQL_C_DOUBLE, &c4, sizeof(c4), &c4_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 5, DSQL_C_TIMESTAMP, &c5, sizeof(c5), &c5_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 6, DSQL_C_NCHAR, &c6, sizeof(c6), &c6_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 7, DSQL_C_NCHAR, &c7, sizeof(c7), &c7_ind), DSQL_HANDLE_STMT, hstmt);
    // 显示输出信息
    printf("dm_select_with_fetch_scroll......\n");
    printf("----------------------------------------------------------------------\n");
    while (dpi_fetch_scroll(hstmt, DSQL_FETCH_NEXT, 0, &row_num) != DSQL_NO_DATA)
    {
        printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
        printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
        printf("c6 = %s, c7 = %s\n", c6, c7);
        dataflag = 1;
    }
    if (!dataflag)
    {
        printf("dm no data\n");
        return DSQL_SUCCESS;
    }
    DPIRETURN_CHECK(dpi_fetch_scroll(hstmt, DSQL_FETCH_FIRST, 0, &row_num), DSQL_HANDLE_STMT, hstmt);
    printf("move first : 1\n");
    printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
    printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
    printf("c6 = %s, c7 = %s\n", c6, c7);
    DPIRETURN_CHECK(dpi_fetch_scroll(hstmt, DSQL_FETCH_LAST, 0, &row_num), DSQL_HANDLE_STMT, hstmt);
    printf("move last : 19\n");
    printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
    printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
    printf("c6 = %s, c7 = %s\n", c6, c7);
    DPIRETURN_CHECK(dpi_fetch_scroll(hstmt, DSQL_FETCH_ABSOLUTE, 6, &row_num), DSQL_HANDLE_STMT, hstmt);
    printf("move absolute 6: 14\n");
    printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
    printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
    printf("c6 = %s, c7 = %s\n", c6, c7);
    DPIRETURN_CHECK(dpi_fetch_scroll(hstmt, DSQL_FETCH_PRIOR, 0, &row_num), DSQL_HANDLE_STMT, hstmt);
    printf("move prior : 13\n");
    printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
    printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
    printf("c6 = %s, c7 = %s\n", c6, c7);
    DPIRETURN_CHECK(dpi_fetch_scroll(hstmt, DSQL_FETCH_RELATIVE, 3, &row_num), DSQL_HANDLE_STMT, hstmt);
    printf("move relative 3: 16\n");
    printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
    printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
    printf("c6 = %s, c7 = %s\n", c6, c7);
    printf("----------------------------------------------------------------------\n");
    // 释放语句句柄
    DPIRETURN_CHECK(dpi_free_stmt(hstmt), DSQL_HANDLE_STMT, hstmt);
    return DSQL_SUCCESS;
}
/************************************************
    fetch获取结果集输出到数组
************************************************/
DPIRETURN
dm_select_with_fetch_array()
{
    sdint4 c1[ROWS]; // 与字段匹配的变量，用于获取字段值
    sdbyte c2[ROWS][20];
    sdbyte c3[ROWS][50];
    ddouble c4[ROWS];
    dpi_timestamp_t c5[ROWS];
    sdbyte c6[ROWS][50];
    sdbyte c7[ROWS][FLEN];
    slength c1_ind[ROWS]; // 缓冲区
    slength c2_ind[ROWS];
    slength c3_ind[ROWS];
    slength c4_ind[ROWS];
    slength c5_ind[ROWS];
    slength c6_ind[ROWS];
    slength c7_ind[ROWS];
    ulength row_num; // 行数
    ulength i;
    ulength i_array_rows = ROWS;
    // 分配语句句柄
    DPIRETURN_CHECK(dpi_alloc_stmt(hcon, &hstmt), DSQL_HANDLE_STMT, hstmt);
    // 设置语句句柄属性
    DPIRETURN_CHECK(dpi_set_stmt_attr(hstmt, DSQL_ATTR_ROWSET_SIZE, (dpointer)i_array_rows, sizeof(i_array_rows)), DSQL_HANDLE_STMT, hstmt);
    // 执行sql
    DPIRETURN_CHECK(dpi_exec_direct(hstmt, "select c1,c2,c3,c4,c5,c6,c7 from dpi_demo"), DSQL_HANDLE_STMT, hstmt);
    // 绑定输出列
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 1, DSQL_C_SLONG, &c1[0], sizeof(c1[0]), &c1_ind[0]), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 2, DSQL_C_NCHAR, &c2[0], sizeof(c2[0]), &c2_ind[0]), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 3, DSQL_C_NCHAR, &c3[0], sizeof(c3[0]), &c3_ind[0]), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 4, DSQL_C_DOUBLE, &c4[0], sizeof(c4[0]), &c4_ind[0]), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 5, DSQL_C_TIMESTAMP, &c5[0], sizeof(c5[0]), &c5_ind[0]), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 6, DSQL_C_NCHAR, &c6[0], sizeof(c6[0]), &c6_ind[0]), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 7, DSQL_C_NCHAR, &c7[0], sizeof(c7[0]), &c7_ind[0]), DSQL_HANDLE_STMT, hstmt);
    // 打印输出信息
    printf("dm_select_with_fetch_array......\n");
    printf("----------------------------------------------------------------------\n");
    if (dpi_fetch(hstmt, &row_num) != DSQL_NO_DATA)
    {
        row_num = row_num > ROWS ? ROWS : row_num;
        for (i = 0; i < row_num; i++)
        {
            printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1[i], c2[i], c3[i], c4[i]);
            printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5[i].year, c5[i].month, c5[i].day, c5[i].hour, c5[i].minute, c5[i].second, c5[i].fraction);
            printf("c6 = %s, c7 = %s\n", c6[i], c7[i]);
        }
    }
    else
    {
        printf("dm no data\n");
    }
    printf("----------------------------------------------------------------------\n");
    // 释放语句句柄
    DPIRETURN_CHECK(dpi_free_stmt(hstmt), DSQL_HANDLE_STMT, hstmt);
    return DSQL_SUCCESS;
}

DPIRETURN
dm_insert_select_complex_type_value()
{
    dhobj obj;          // 复合对象句柄
    dhobjdesc obj_desc; // 复合对象描述句柄
    udint4 cnt, cnt1;
    slength len;
    sdint2 type, type1, type2;
    sdint2 prec, prec1, prec2;
    sdint2 scale, scale1, scale2;
    int c1_data, c1_val;
    char c2_data[21], c2_val[21];
    slength val_len[2], data_len[2];
    /* 分配语句句柄 */
    DPIRETURN_CHECK(dpi_alloc_stmt(hcon, &hstmt), DSQL_HANDLE_STMT, hstmt);
    dpi_exec_direct(hstmt, "drop table t");
    dpi_exec_direct(hstmt, "drop class cls1");
    DPIRETURN_CHECK(dpi_exec_direct(hstmt, "create class cls1 as c1 int; c2 varchar(20); end;"), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_exec_direct(hstmt, "create table t(c1 cls1)"), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_desc_obj(hcon, "SYSDBA", "CLS1", &obj_desc), DSQL_HANDLE_DBC, hcon);
    DPIRETURN_CHECK(dpi_alloc_obj(hcon, &obj), DSQL_HANDLE_DBC, hcon);
    DPIRETURN_CHECK(dpi_bind_obj_desc(obj, obj_desc), DSQL_HANDLE_OBJECT, obj);
    // 复合类型获取描述信息
    DPIRETURN_CHECK(dpi_get_obj_desc_attr(obj_desc, 0, DSQL_ATTR_OBJ_FIELD_COUNT, &cnt1, sizeof(cnt1), NULL), DSQL_HANDLE_OBJDESC, obj_desc);
    printf("cnt is : %d\n", cnt1);
    DPIRETURN_CHECK(dpi_get_obj_desc_attr(obj_desc, 1, DSQL_ATTR_OBJ_TYPE, &type1, sizeof(type1), NULL), DSQL_HANDLE_OBJDESC, obj_desc);
    printf("type1 is : %d\n", type1);
    if (type1 != DSQL_INT)
    {
        printf("type error");
    }
    DPIRETURN_CHECK(dpi_get_obj_desc_attr(obj_desc, 2, DSQL_ATTR_OBJ_TYPE, &type2, sizeof(type2), NULL), DSQL_HANDLE_OBJDESC, obj_desc);
    printf("type2 is : %d\n", type2);
    if (type1 != DSQL_VARCHAR)
    {
        printf("type error");
    }
    DPIRETURN_CHECK(dpi_get_obj_desc_attr(obj_desc, 1, DSQL_ATTR_OBJ_PREC, &prec1, sizeof(prec1), NULL), DSQL_HANDLE_OBJDESC, obj_desc);
    printf("prec1 is : %d\n", prec1);
    DPIRETURN_CHECK(dpi_get_obj_desc_attr(obj_desc, 2, DSQL_ATTR_OBJ_PREC, &prec2, sizeof(prec2), NULL), DSQL_HANDLE_OBJDESC, obj_desc);
    printf("prec1 is : %d\n", prec2);
    DPIRETURN_CHECK(dpi_get_obj_desc_attr(obj_desc, 1, DSQL_ATTR_OBJ_SCALE, &scale1, sizeof(scale1), NULL), DSQL_HANDLE_OBJDESC, obj_desc);
    printf("scale1 is : %d\n", scale1);
    DPIRETURN_CHECK(dpi_get_obj_desc_attr(obj_desc, 2, DSQL_ATTR_OBJ_SCALE, &scale2, sizeof(scale2), NULL), DSQL_HANDLE_OBJDESC, obj_desc);
    printf("scale2 is : %d\n", scale2);

    // 复合类型插入
    c1_data = 1;
    strcpy(c2_data, "aaa");
    data_len[0] = sizeof(c1_data);
    data_len[1] = strlen(c2_data);
    DPIRETURN_CHECK(dpi_set_obj_val(obj, 1, DSQL_C_SLONG, &c1_data, data_len[0]), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_set_obj_val(obj, 2, DSQL_C_NCHAR, &c2_data, data_len[1]), DSQL_HANDLE_STMT, hstmt);
    // 执行sql
    DPIRETURN_CHECK(dpi_prepare(hstmt, "insert into t(c1) values(?)"), DSQL_HANDLE_STMT, hstmt);
    // 绑定输出列
    len = sizeof(obj);
    DPIRETURN_CHECK(dpi_bind_param(hstmt, 1, DSQL_PARAM_INPUT, DSQL_C_CLASS, DSQL_CLASS, 0, 0, &obj, sizeof(obj), &len), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_exec(hstmt), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_commit(hcon), DSQL_HANDLE_DBC, hcon);
    // 复合类型查询
    DPIRETURN_CHECK(dpi_exec_direct(hstmt, "select c1 from t"), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 1, DSQL_C_CLASS, &obj, sizeof(obj), &len), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_fetch(hstmt, NULL), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_get_obj_val(obj, 1, DSQL_C_SLONG, &c1_val, sizeof(c1_val), &val_len[0]), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_get_obj_val(obj, 2, DSQL_C_NCHAR, c2_val, sizeof(c2_val), &val_len[1]), DSQL_HANDLE_STMT, hstmt);
    printf("c1_val=%d,c2_val=%s\n", c1_val, c2_val);
    if (c1_val != c1_data || strcmp(c2_val, c2_data) != 0)
    {
        printf("dpi_get_obj_val获取结果 error");
    }
    printf("----------------------------------------------------------------------\n");
    // 释放语句句柄
    DPIRETURN_CHECK(dpi_free_stmt(hstmt), DSQL_HANDLE_STMT, hstmt);
    return DSQL_SUCCESS;
}

/************************************************
    自测使用 base
    fetch获取结果集,scroll结果集
************************************************/
DPIRETURN
dm_select_with_fetch_scroll_self()
{
    sdbyte c1[23]; // 与字段匹配的变量，用于获取字段值
    sdbyte c2[20];
    sdbyte c3[50];
    ddouble c4;
    dpi_timestamp_t c5;
    sdbyte c6[50];
    sdbyte c7[FLEN];
    slength c1_ind = 0; // 缓冲区
    slength c2_ind = 0;
    slength c3_ind = 0;
    slength c4_ind = 0;
    slength c5_ind = 0;
    slength c6_ind = 0;
    slength c7_ind = 0;
    ulength row_num;                   // 行数
    ulength val = DSQL_CURSOR_DYNAMIC; // 这个才能获取到行数
    sdint4 dataflag = 0;
    // 分配语句句柄
    DPIRETURN_CHECK(dpi_alloc_stmt(hcon, &hstmt), DSQL_HANDLE_STMT, hstmt);
    // 设置语句句柄属性
    DPIRETURN_CHECK(dpi_set_stmt_attr(hstmt, DSQL_ATTR_CURSOR_TYPE, (dpointer)val, 0), DSQL_HANDLE_STMT, hstmt);
    // 执行sql
    DPIRETURN_CHECK(dpi_exec_direct(hstmt, "select c1,c2,c3,c4,c5,c6,c7 from dpi_demo"), DSQL_HANDLE_STMT, hstmt);

    sdint2 rows1;
    DPIRETURN_CHECK(dpi_row_count(hstmt, &rows1), DSQL_HANDLE_STMT, hstmt);
    printf("---- before bind, rows = %d\n", rows1);

    // 绑定输出列
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 1, DSQL_C_NCHAR, &c1, sizeof(c1), &c1_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 2, DSQL_C_NCHAR, &c2, sizeof(c2), &c2_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 3, DSQL_C_NCHAR, &c3, sizeof(c3), &c3_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 4, DSQL_C_DOUBLE, &c4, sizeof(c4), &c4_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 5, DSQL_C_TIMESTAMP, &c5, sizeof(c5), &c5_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 6, DSQL_C_NCHAR, &c6, sizeof(c6), &c6_ind), DSQL_HANDLE_STMT, hstmt);
    DPIRETURN_CHECK(dpi_bind_col(hstmt, 7, DSQL_C_NCHAR, &c7, sizeof(c7), &c7_ind), DSQL_HANDLE_STMT, hstmt);

    // 显示输出信息
    printf("dm_select_with_fetch_scroll......\n");
    printf("----------------------------------------------------------------------\n");

    sdint2 col_cnt;
    DPIRETURN_CHECK(dpi_number_columns(hstmt, &col_cnt), DSQL_HANDLE_STMT, hstmt);
    printf("---- col_cnt = %d\n", col_cnt);

    sdint2 rows;
    DPIRETURN_CHECK(dpi_row_count(hstmt, &rows), DSQL_HANDLE_STMT, hstmt);
    printf("---- rows = %d\n", rows);

    sdint2 fetch_rows = 0;
    while (dpi_fetch_scroll(hstmt, DSQL_FETCH_NEXT, 0, &row_num) != DSQL_NO_DATA)
    {
        printf("c1 = %s, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
        printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
        printf("c6 = %s, c7 = %s\n", c6, c7);

        printf("---- index 1[%d] 2[%d] 3[%d] 4[%d] 5[%d] 6[%d] 7[%d]\n",
               c1_ind, c2_ind, c3_ind, c4_ind, c5_ind, c6_ind, c7_ind);
        dataflag = 1;
        ++fetch_rows;
    }

    printf("---- fetch_rows = %d\n", fetch_rows);
    if (!dataflag)
    {
        printf("dm no data\n");
        return DSQL_SUCCESS;
    }
    DPIRETURN_CHECK(dpi_fetch_scroll(hstmt, DSQL_FETCH_FIRST, 0, &row_num), DSQL_HANDLE_STMT, hstmt);
    printf("move first : 1\n");
    printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
    printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
    printf("c6 = %s, c7 = %s\n", c6, c7);
    DPIRETURN_CHECK(dpi_fetch_scroll(hstmt, DSQL_FETCH_LAST, 0, &row_num), DSQL_HANDLE_STMT, hstmt);
    printf("move last : 19\n");
    printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
    printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
    printf("c6 = %s, c7 = %s\n", c6, c7);
    DPIRETURN_CHECK(dpi_fetch_scroll(hstmt, DSQL_FETCH_ABSOLUTE, 6, &row_num), DSQL_HANDLE_STMT, hstmt);
    printf("move absolute 6: 14\n");
    printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
    printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
    printf("c6 = %s, c7 = %s\n", c6, c7);
    DPIRETURN_CHECK(dpi_fetch_scroll(hstmt, DSQL_FETCH_PRIOR, 0, &row_num), DSQL_HANDLE_STMT, hstmt);
    printf("move prior : 13\n");
    printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
    printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
    printf("c6 = %s, c7 = %s\n", c6, c7);
    DPIRETURN_CHECK(dpi_fetch_scroll(hstmt, DSQL_FETCH_RELATIVE, 3, &row_num), DSQL_HANDLE_STMT, hstmt);
    printf("move relative 3: 16\n");
    printf("c1 = %d, c2 = %s, c3 = %s, c4 = %f, \n", c1, c2, c3, c4);
    printf("c5 = %d-%d-%d %d:%d:%d.%d\n", c5.year, c5.month, c5.day, c5.hour, c5.minute, c5.second, c5.fraction);
    printf("c6 = %s, c7 = %s\n", c6, c7);
    printf("----------------------------------------------------------------------\n");
    // 释放语句句柄
    DPIRETURN_CHECK(dpi_free_stmt(hstmt), DSQL_HANDLE_STMT, hstmt);
    return DSQL_SUCCESS;
}

// 达梦数据库中提供了LAST_INSERT_ID函数，可以获取插入的自增列的值，因此可以自行封装个函数，将查询到的LAST_INSERT_ID结果返回，代码如下
DPIRETURN dm_insert_id()
{
    ulength row_num;
    sdint2 c1 = 0;
    slength c1_ind = 0;
    rt = dpi_alloc_stmt(hcon, &hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);

    // rt = dpi_exec_direct(hstmt, "select LAST_INSERT_ID from dual");
    rt = dpi_exec_direct(hstmt, "select @@IDENTITY");
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);

    DPIRETURN_CHECK(dpi_bind_col(hstmt, 1, DSQL_C_UBIGINT, &c1, sizeof(c1), &c1_ind), DSQL_HANDLE_STMT, hstmt);

    rt = dpi_fetch(hstmt, &row_num);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    // 释放语句句柄
    rt = dpi_free_stmt(hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    printf("dm insert id success\n");
    printf("c1: %d", c1);
    return c1;
}

void test_ind()
{

    // 申请语句句柄
    rt = dpi_alloc_stmt(hcon, &hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
    // 执行sql
    // rt = dpi_exec_direct(hstmt, "CREATE TABLE IDENT_TABLE_T2 (C1 INT IDENTITY(0, 1),C2 INT)");
    // rt = dpi_exec_direct(hstmt, "CREATE TABLE IDEN3 (ID INT PRIMARY KEY AUTO_INCREMENT, NAME CHAR(10)) ");
    // DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    // 释放语句句柄
    rt = dpi_free_stmt(hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);

    // 申请语句句柄
    rt = dpi_alloc_stmt(hcon, &hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
    // 执行sql
    // rt = dpi_exec_direct(hstmt, "insert into IDENT_TABLE_T2 values(3)");
    rt = dpi_exec_direct(hstmt, "insert into IDEN3(name) values('test')");
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
    // 释放语句句柄
    rt = dpi_free_stmt(hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);

    int in_id = dm_insert_id();
    printf("inid: %d", in_id);

    printf("dm init table success\n");
}

// 入口函数
DPIRETURN
main()
{
    // 连接数据库
    rt = dm_dpi_connect(DM_SVR, DM_USER, DM_PWD);
    FUN_CHECK(rt);
    // 初始化表
    rt = dm_init_table();
    FUN_CHECK(rt);
    // 通过参数绑定的方式插入数据
    rt = dm_insert_with_bind_param();
    FUN_CHECK(rt);
    // 通过数组绑定的方式插入数据
    rt = dm_insert_with_bind_array();
    FUN_CHECK(rt);

    test_ind();

    // self test
    rt = dm_select_with_fetch_scroll_self();
    FUN_CHECK(rt);
    return DSQL_SUCCESS;

    // 通过fetch查询得到结果集
    rt = dm_select_with_fetch();
    FUN_CHECK(rt);
    // 通过参数绑定查询得到结果集
    rt = dm_select_with_fetch_with_param();
    FUN_CHECK(rt);
    // 通过fetch scroll获取结果集
    rt = dm_select_with_fetch_scroll();
    FUN_CHECK(rt);
    // 查询列绑定数组输出
    rt = dm_select_with_fetch_array();
    FUN_CHECK(rt);
    // 复合类型插入查询描述信息获取示例
    rt = dm_insert_select_complex_type_value();
    FUN_CHECK(rt);
    // 断开连接
    rt = dm_dpi_disconnect();
    FUN_CHECK(rt);
END:
    return DSQL_SUCCESS;
}