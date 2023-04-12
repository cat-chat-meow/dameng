#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DPI.h"
#include "DPIext.h"
#include "DPItypes.h"

#include <config_parser.h>

#define IN_FILE "/data/c_test_code/DM8_SQL.pdf"
#define OUT_FILE "/data/c_test_code/DM8_SQL_1.pdf"
#define CHARS 80 * 1024 // 一次读取和写入的字节数 80 KB

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

    FILE *pfile = NULL;
    sdbyte tmpbuf[CHARS];
    slength len = 0;
    ulength row_num = 0;
    slength val_len = 0;

    sdint4 c1 = 1;
    sdint4 c2 = DSQL_DATA_AT_EXEC;
    slength c1_ind_ptr = 0;
    slength c2_ind_ptr = DSQL_DATA_AT_EXEC;
    dpointer c2_val_ptr;

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
    rt = dpi_exec_direct(hstmt, (sdbyte *)"drop table PRODUCTION.BIG_DATA");
    rt = dpi_exec_direct(hstmt, (sdbyte *)"create table PRODUCTION.BIG_DATA(c1 int, c2 blob)");

    // 读取文件数据，插入LOB列
    pfile = fopen(IN_FILE, "rb");
    if (pfile == NULL)
    {
        printf("open %s fail\n", IN_FILE);
        return DSQL_ERROR;
    }

    rt = dpi_prepare(hstmt, (sdbyte *)"insert into PRODUCTION.BIG_DATA(c1,c2) values(?,?)");
    rt = dpi_bind_param(hstmt, 1, DSQL_PARAM_INPUT, DSQL_C_SLONG, DSQL_INT, sizeof(c1), 0, &c1, sizeof(c1), NULL);
    rt = dpi_bind_param(hstmt, 2, DSQL_PARAM_INPUT, DSQL_C_BINARY, DSQL_BLOB, sizeof(c2), 0, &c2, sizeof(c2), &c2_ind_ptr);
    if (dpi_exec(hstmt) == DSQL_NEED_DATA)
    {
        if (dpi_param_data(hstmt, &c2_val_ptr) == DSQL_NEED_DATA) /* 绑定数据 */
        {
            while (!feof(pfile))
            {
                len = fread(tmpbuf, sizeof(char), CHARS, pfile);
                if (len <= 0)
                {
                    return DSQL_ERROR;
                }
                dpi_put_data(hstmt, tmpbuf, len);
            }
        }
        dpi_param_data(hstmt, &c2_val_ptr); /* 绑定数据 */
    }
    printf("dpi: insert data into col of lob success\n");
    fclose(pfile);

    // 读取 LOB 列数据，写入文件
    pfile = fopen((const char *)OUT_FILE, "wb");
    if (pfile == NULL)
    {
        printf("open %s fail\n", OUT_FILE);
        return DSQL_ERROR;
    }

    dpi_exec_direct(hstmt, (sdbyte *)"select c1, c2 from PRODUCTION.BIG_DATA");
    dpi_bind_col(hstmt, 1, DSQL_C_SLONG, &c1, sizeof(c1), &c1_ind_ptr);
    while (dpi_fetch(hstmt, &row_num) != DSQL_NO_DATA)
    {
        while (DSQL_SUCCEEDED(dpi_get_data(hstmt, 2, DSQL_C_BINARY, tmpbuf, CHARS, &val_len)))
        {
            len = val_len > CHARS ? CHARS : val_len;
            fwrite(tmpbuf, sizeof(char), len, pfile);
        }
    }

    fclose(pfile);
    printf("dpi: get data from col of lob success\n");

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
