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

#define DPIRETURN_CHECK(rt, hndl_type, hndl) \
    if (!DSQL_SUCCEEDED(rt))                 \
    {                                        \
        dpi_err_msg_print(hndl_type, hndl);  \
        return rt;                           \
    }

/******************************************************
Notes：
获取错误信息
*******************************************************/
void dpi_err_msg_print(sdint2 hndl_type, dhandle hndl)
{
    sdint4 err_code;
    sdint2 msg_len;
    sdbyte err_msg[SDBYTE_MAX];

    /* 获取错误信息集合 */
    dpi_get_diag_rec(hndl_type, hndl, 1, &err_code, err_msg, sizeof(err_msg), &msg_len);
    printf("err_msg = %s, err_code = %d\n", err_msg, err_code);
}

#define TEST_SQL 3
#define SQL_2D 256

/*
入口函数
*/
int main(int argc, char *argv[])
{
    db_config config = read_config();

    // 连接数据库
    rt = dpi_alloc_env(&henv);
    rt = dpi_alloc_con(henv, &hcon);
    rt = dpi_login(hcon,
                   (sdbyte *)config.db_server,
                   (sdbyte *)config.db_user,
                   (sdbyte *)config.db_pwd);

    DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);

    rt = dpi_alloc_stmt(hcon, &hstmt);
    DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);

    // 1 测试自增字符串
    // 2 测试自增数字
    // 3 测试非自增
    sdbyte sql_drop[TEST_SQL][SQL_2D] = {
        "DROP TABLE IF EXISTS TEST001",
        "DROP TABLE IF EXISTS TEST002",
        "DROP TABLE IF EXISTS TEST003",
    };
    sdbyte sql_create[TEST_SQL][SQL_2D] = {
        "CREATE TABLE IF NOT EXISTS TEST001 (ID INT IDENTITY(1, 1), NAME VARCHAR(50), CITY VARCHAR(50))",
        "CREATE TABLE IF NOT EXISTS TEST002 (ID INT IDENTITY(1, 1), NAME VARCHAR(50), NUMBER INT)",
        "CREATE TABLE IF NOT EXISTS TEST003 (NAME VARCHAR(50), NUMBER INT)",
    };
    sdbyte sql_init[TEST_SQL][SQL_2D] = {
        "INSERT INTO TEST001 (NAME, CITY) VALUES ('罗夫', '罗浮'), ('卡夫', '杏核'), ('赢', '星河')",
        "INSERT INTO TEST002 (NAME, NUMBER) VALUES ('罗夫', 1), ('卡夫', 114), ('赢', 523)",
        "INSERT INTO TEST003 (NAME, NUMBER) VALUES ('罗夫', 1), ('卡夫', 114), ('赢', 523)",
    };

    for (size_t i = 0; i < TEST_SQL; ++i)
    {
        printf("exec sql: %s\n", sql_drop[i]);
        rt = dpi_exec_direct(hstmt, (sdbyte *)sql_drop[i]);
        DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
        printf("exec sql: %s\n", sql_create[i]);
        rt = dpi_exec_direct(hstmt, (sdbyte *)sql_create[i]);
        DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
        printf("exec sql: %s\n", sql_init[i]);
        rt = dpi_exec_direct(hstmt, (sdbyte *)sql_init[i]);
        DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
    }

    sdbyte sql_bind[TEST_SQL][SQL_2D] = {
        "INSERT INTO TEST001 (NAME, CITY) values('haha', ?)",
        "INSERT INTO TEST002 (NAME, NUMBER) VALUES ('罗夫', ?)",
        "INSERT INTO TEST003 (NAME, NUMBER) VALUES (?, ?)",
    };

    // 查询数据
    sdbyte sql_select[TEST_SQL][SQL_2D] = {
        "SELECT NAME, CITY FROM TEST001",
        "SELECT NAME, NUMBER FROM TEST002",
        "SELECT NAME, NUMBER FROM TEST003",
    };

    printf("\ninitial done...\n\n");

    for (size_t i = 0; i < TEST_SQL; ++i)
    {
        sdint4 in_c1 = 0;
        sdbyte in_c2[50] = {0};
        slength in_c1_ind_ptr;
        slength in_c2_ind_ptr;

        in_c1 = 233;
        sdbyte str[] = "猪鼻";
        memcpy(in_c2, str, sizeof(str));
        printf("str:%s, len:%ld\n", str, sizeof(str));

        in_c1_ind_ptr = sizeof(in_c1);
        in_c2_ind_ptr = sizeof(str);

        sdint4 out_c1 = 0;
        sdbyte out_c2[50];
        sdbyte out_c3[50];
        slength out_c1_ind = 0;
        slength out_c2_ind = 0;
        slength out_c3_ind = 0;
        ulength row_num;

        printf("insert with bind..\nsql: %s\npara: [%d] [%s]\n", (char *)sql_bind[i], in_c1, (char *)in_c2);

        rt = dpi_prepare(hstmt, sql_bind[i]);
        DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);

        // 绑定参数方式插入数据
        switch (i)
        {
        case 0:
            rt = dpi_bind_param(hstmt, 1, DSQL_PARAM_INPUT, DSQL_C_NCHAR, DSQL_VARCHAR, sizeof(in_c2), 0, in_c2, sizeof(in_c2), &in_c2_ind_ptr);
            DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
            break;
        case 1:
            rt = dpi_bind_param(hstmt, 1, DSQL_PARAM_INPUT, DSQL_C_SLONG, DSQL_INT, sizeof(in_c1), 0, &in_c1, sizeof(in_c1), &in_c1_ind_ptr);
            DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
            break;
        case 2:
            rt = dpi_bind_param(hstmt, 1, DSQL_PARAM_INPUT, DSQL_C_NCHAR, DSQL_VARCHAR, sizeof(in_c2), 0, in_c2, sizeof(in_c2), &in_c2_ind_ptr);
            rt = dpi_bind_param(hstmt, 1, DSQL_PARAM_INPUT, DSQL_C_SLONG, DSQL_INT, sizeof(in_c1), 0, &in_c1, sizeof(in_c1), &in_c1_ind_ptr);
            DPIRETURN_CHECK(rt, DSQL_HANDLE_DBC, hcon);
            break;
        default:
            break;
        }

        rt = dpi_exec(hstmt);
        DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
        printf("dpi: insert into table with bind success!\n");

        // 查询
        printf("\nexec select sql: %s\n", sql_select[i]);
        rt = dpi_exec_direct(hstmt, (sdbyte *)sql_select[i]);
        DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
        switch (i)
        {
        case 1:
            rt = dpi_bind_col(hstmt, 1, DSQL_C_NCHAR, &out_c2, sizeof(out_c2), &out_c2_ind);
            DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
            rt = dpi_bind_col(hstmt, 2, DSQL_C_NCHAR, &out_c3, sizeof(out_c3), &out_c3_ind);
            DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
            break;
        case 2:
            rt = dpi_bind_col(hstmt, 1, DSQL_C_NCHAR, &out_c2, sizeof(out_c2), &out_c2_ind);
            DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
            rt = dpi_bind_col(hstmt, 2, DSQL_C_SLONG, &out_c1, sizeof(out_c1), &out_c1_ind);
            DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
            break;
        case 3:
            rt = dpi_bind_col(hstmt, 1, DSQL_C_NCHAR, &out_c2, sizeof(out_c2), &out_c2_ind);
            DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
            rt = dpi_bind_col(hstmt, 2, DSQL_C_SLONG, &out_c1, sizeof(out_c1), &out_c1_ind);
            DPIRETURN_CHECK(rt, DSQL_HANDLE_STMT, hstmt);
            break;
        default:
            break;
        }

        printf("\ndpi: select from table......\n");
        while (dpi_fetch_scroll(hstmt, DSQL_FETCH_NEXT, 0, &row_num) != DSQL_NO_DATA)
        {
            printf("c1 = %d, c2 = %s, c3 = %s,\n", out_c1, out_c2, out_c3);
        }
        printf("dpi: select from table success\n\n");
    }

    // 断开数据库连接
    rt = dpi_logout(hcon);
    if (!DSQL_SUCCEEDED(rt))
    {
        dpi_err_msg_print(DSQL_HANDLE_DBC, hcon);
        return rt;
    }

    rt = dpi_free_con(hcon);
    rt = dpi_free_env(henv);

    return rt;
}
