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

/*
入口函数
*/
int main(int argc, char *argv[])
{
    db_config config = read_config("config.ini");

    sdbyte sql[] = "insert into PRODUCTION.PRODUCT_CATEGORY(NAME) values(?)";
    sdbyte in_c1[20] = {0};
    slength in_c1_ind_ptr;

    memcpy(in_c1, "物理", 8);
    in_c1_ind_ptr = 8;

    sdint4 out_c1 = 0;
    sdbyte out_c2[20] = {0};
    slength out_c1_ind = 0;
    slength out_c2_ind = 0;
    ulength row_num;

    // 连接数据库
    rt = dpi_alloc_env(&henv);
    rt = dpi_alloc_con(henv, &hcon);
    rt = dpi_login(hcon,
                   (sdbyte *)config.db_server,
                   (sdbyte *)config.db_user,
                   (sdbyte *)config.db_pwd);
    if (!DSQL_SUCCEEDED(rt))
    {
        dpi_err_msg_print(DSQL_HANDLE_DBC, hcon);
        return rt;
    }

    rt = dpi_alloc_stmt(hcon, &hstmt);

    // 清空表，初始化测试环境
    dpi_exec_direct(hstmt, (sdbyte *)"delete from PRODUCTION.PRODUCT_CATEGORY");

    // 绑定参数方式插入数据
    printf("insert with bind..\nsql: %s\npara: %s\n", (char *)sql, (char *)in_c1);
    rt = dpi_prepare(hstmt, sql);
    rt = dpi_bind_param(hstmt, 1, DSQL_PARAM_INPUT, DSQL_C_NCHAR, DSQL_VARCHAR, sizeof(in_c1), 0, in_c1, sizeof(in_c1), &in_c1_ind_ptr);
    rt = dpi_exec(hstmt);
    if (!DSQL_SUCCEEDED(rt))
    {
        dpi_err_msg_print(DSQL_HANDLE_DBC, hcon);
        return rt;
    }
    printf("dpi: insert into table with bind success!\n");

    // 查询数据
    dpi_exec_direct(hstmt, (sdbyte *)"select * from PRODUCTION.PRODUCT_CATEGORY");
    dpi_bind_col(hstmt, 1, DSQL_C_SLONG, &out_c1, sizeof(out_c1), &out_c1_ind);
    dpi_bind_col(hstmt, 2, DSQL_C_NCHAR, &out_c2, sizeof(out_c2), &out_c2_ind);

    printf("\ndpi: select from table......\n");
    while (dpi_fetch(hstmt, &row_num) != DSQL_NO_DATA)
    {
        printf("c1 = %d, c2 = %s ,\n", out_c1, out_c2);
    }
    printf("dpi: select from table success\n\n");

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
