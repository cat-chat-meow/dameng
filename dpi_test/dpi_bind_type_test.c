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

int get_c_type(const enum_field_types type)
{
    switch (type)
    {
    case DB_TYPE_CHAR:
    case DB_TYPE_VARCHAR:
    case DB_TYPE_CLOB:
    case DB_TYPE_BLOB:
        // sdbyte
        return DSQL_C_NCHAR;
    case DB_TYPE_INT:
        // sdint4
        return DSQL_C_SLONG;
    case DB_TYPE_FLOAT:
        return DSQL_C_FLOAT;
    case DB_TYPE_DOUBLE:
        return DSQL_C_DOUBLE;
    case DB_TYPE_TIMESTAMP:
        return DSQL_C_TIMESTAMP;
    default:
        return 0;
        break;
    }
}

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

#define ROWS 10

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

/*
入口函数
*/
int main(int argc, char *argv[])
{
    return 0;
}
