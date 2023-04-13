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

/* 入口函数 */
int main(int argc, char *argv[])
{

    db_config config = read_config();

    // 连接数据库
    /* 申请环境句柄 */
    rt = dpi_alloc_env(&henv);

    /* 申请连接句柄 */
    rt = dpi_alloc_con(henv, &hcon);

    /* 连接数据库服务器 */
    rt = dpi_login(hcon,
                   (sdbyte *)config.db_server,
                   (sdbyte *)config.db_user,
                   (sdbyte *)config.db_pwd);
    if (!DSQL_SUCCEEDED(rt))
    {
        dpi_err_msg_print(DSQL_HANDLE_DBC, hcon);
        return rt;
    }
    printf("dpi: connect to server success!\n");

    // 断开数据库连接
    /* 断开连接 */
    rt = dpi_logout(hcon);
    if (!DSQL_SUCCEEDED(rt))
    {
        dpi_err_msg_print(DSQL_HANDLE_DBC, hcon);
        return rt;
    }
    printf("dpi: disconnect from server success!\n");

    /* 释放连接句柄和环境句柄 */
    rt = dpi_free_con(hcon);
    rt = dpi_free_env(henv);

    return rt;
}
