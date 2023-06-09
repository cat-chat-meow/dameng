#ifndef _DFLDR_H
#define _DFLDR_H

#include "fldr_parse.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )
#else
#define DllImport
#define DllExport
#endif

typedef int         FLDR_RETURN;
typedef void*       fhinstance;
typedef void*       fpointer;
typedef signed char fchar;
typedef short       fsint2;
typedef int         fsint4;

#if defined(WIN32) || defined(WINDOWS)
typedef __int64    fsint8;
typedef unsigned __int64 fuint8;
#else
typedef long long int fsint8;
typedef unsigned long long int fuint8;
#endif //DWINDOWS

#define FLDR_SUCCESS            0
#define FLDR_SUCCESS_WITH_INFO  1
#define FLDR_ERROR              (-1)
#define FLDR_INVALID_HANDLE     (-2)
#define FLDR_NO_DATA            100


#define FLDR_SUCCEEDED(rt)    (((rt)&(~1))==0)

#define FLDR_UNINITILIAZE_COMMIT        (0)
#define FLDR_UNINITILIAZE_ROLLBACK      (1)


#define FLDR_TYPE_BIND                  (1)
#define FLDR_TYPE_TEXT_STREAM           (2)
#define FLDR_TYPE_CTRL                  (3)
#define FLDR_TYPE_JNI                   (4)
#define FLDR_TYPE_JNI_FILE              (5)
#define FLDR_TYPE_FODBC                 (6)
#define FLDR_TYPE_DIMP                  (7)
#define FLDR_TYPE_DEFAULT               (8)

#define FLDR_DATA_NULL                  (-1)
#define FLDR_DATA_PUT                   (-2)
#define FLDR_DATA_NTS                   (-3)
#define FLDR_DATA_PUT_L                 (-4)

//binding input c type
#define FLDR_C_BINARY       1
#define FLDR_C_CHAR         2
#define FLDR_C_TINYINT      3
#define FLDR_C_SHORT        4
#define FLDR_C_INT          5
#define FLDR_C_BIGINT       6
#define FLDR_C_FLOAT        7
#define FLDR_C_DOUBLE       8


#define     FLDR_ATTR_SERVER                1
#define     FLDR_ATTR_UID                   2       
#define     FLDR_ATTR_PWD                   3
#define     FLDR_ATTR_PORT                  4
#define     FLDR_ATTR_TABLE                 5
#define     FLDR_ATTR_SET_INDENTITY         6
//#define     FLDR_ATTR_GENERATE_LOG          7     
#define     FLDR_ATTR_DATA_SORTED           8   
#define     FLDR_ATTR_INDEX_OPTION          9      
// #define     FLDR_ATTR_DATEFORMAT            10   
#define     FLDR_ATTR_DATA_SIZE             11  
#define     FLDR_ATTR_INSERT_ROWS           12    
#define     FLDR_ATTR_COMMIT_ROWS           13  
#define     FLDR_ATTR_PROCESS_ROWS          14
#define     FLDR_ATTR_SEND_NODE_NUM         15       
#define     FLDR_ATTR_ERROR_ROWS            16
#define     FLDR_ATTR_BAD_FILE              17  
// #define     FLDR_ATTR_EXACT_ERROR_ROW       18         
#define     FLDR_ATTR_DATA_CHAR_SET         19       
// #define     FLDR_ATTR_FIELD_DELEMITER       20         
// #define     FLDR_ATTR_ROW_TERMINATOR        21        
#define     FLDR_ATTR_CTRL_FILE             22   
// #define     FLDR_ATTR_ASYN_COMMIT           23
#define     FLDR_ATTR_LOG_FILE              24
#define     FLDR_ATTR_SKIP_ROWS             25
#define     FLDR_ATTR_LOAD_ROWS             26
#define     FLDR_ATTR_READ_ROWS             27
#define     FLDR_ATTR_DIRECT_MODE           28
#define     FLDR_ATTR_ROWS_COMMIT           29
#define     FLDR_ATTR_ERRORS_PERMIT         30
#define     FLDR_ATTR_LOAD_MODE             31
#define     FLDR_ATTR_SILENT                32
#define     FLDR_ATTR_MPP_NODES             33
#define     FLDR_ATTR_MPP_CLIENT            34
#define     FLDR_ATTR_SSL_PATH              35
#define     FLDR_ATTR_LOB_DIRECTORY         36
#define     FLDR_ATTR_MPP_LOCAL_FLAG        37
#define     FLDR_ATTR_BUFFER_SIZE           38
#define     FLDR_ATTR_NULL_MODE             39
#define     FLDR_ATTR_LOCAL_CODE            40
#define     FLDR_ATTR_SSL_PWD               41
#define     FLDR_ATTR_TASK_THREAD_NUM       42
#define     FLDR_ATTR_DATA_PATH             43
//#define     FLDR_ATTR_BUFFER_NUM            43
// #define     FLDR_ATTR_SERVER_PARAL          44
// #define     FLDR_ATTR_SERVER_BUFFER_SIZE    45
// #define     FLDR_ATTR_SERVER_BUFFER_NUM     46
#define     FLDR_ATTR_SERVER_BLDR_NUM       47
#define     FLDR_ATTR_BDTA_SIZE             48
#define     FLDR_ATTR_COMPRESS_FLAG         49
#define     FLDR_ATTR_BLOB_TYPE             50
#define     FLDR_ATTR_OCI_DIRECTORY         51
#define     FLDR_ATTR_ENABLE_CLASS_TYPE     52
#define     FLDR_ATTR_CONFLICT_FLAG         53
#define     FLDR_ATTR_LOB_FIL_NAME          54
#define     FLDR_ATTR_CLIENT_LOB            55
#define     FLDR_ATTR_FLUSH_FLAG            56
#define     FLDR_ATTR_UDPFLAG               57  // 1:UDP单发单收;2:UDP多发多收
#define     FLDR_ATTR_IGNORE_BATCH_ERRORS   58
#define     FLDR_ATTR_NULL_STR              59
#define     FLDR_ATTR_SINGLE_HLDR_HP        60
#define     FLDR_ATTR_INET_TYPE             61  // 连接类型：TCP/UDP/IPC
#define     FLDR_ATTR_EP                    62  //MPP环境设置要发送的站点
#define     FLDR_ATTR_SINGLE_FILE           63  //MPP导出数据时是否只生成单个数据文件
#define     FLDR_ATTR_LAN_MODE              64  //MPP导出数据时是否使用内网(如果服务器配置了内网)
#define     FLDR_ATTR_CNVT_MODE             65  //字符集转换遇到错误时处理策略
#define     FLDR_ATTR_PARALLEL              66  //是否开启并行装载
#define     FLDR_ATTR_SQL                   67  //SQL查询语句
#define     FLDR_ATTR_OSAUTH_TYPE           68  //os认证登录类型
#define     FLDR_ATTR_UID_EXT               69
#define     FLDR_ATTR_LOG_SIZE              70  //日志结点大小参数
#define     FLDR_ATTR_COMMIT_OPTION         71  //提交选项
#define     FLDR_ATTR_APPEND_OPTION         72  //追加选项
#define     FLDR_ATTR_COLNAME_HEADING       73  //列名展示
#define     FLDR_ATTR_IGNORE_AIMLESS_DATA   74  //忽略数据
#define     FLDR_ATTR_LOB_AS_VARCHAR        75  //将lob看做字符串来进行导入导出
#define     FLDR_ATTR_LOB_AS_VARCHAR_SIZE   76  //最大的lob的大小
#define     FLDR_ATTR_LOG_LEVEL             77  //日志记录级别
#define     FLDR_ATTR_RECONN                78  //重连次数
#define     FLDR_ATTR_RECONN_TIME           79  //重连等待时间
#define     FLDR_ATTR_WIDTH                 80  //设置导出列列宽
#define     FLDR_ATTR_SEDF                  81  //转换前的字符列表
#define     FLDR_ATTR_SEDT                  82  //转换后的字符列表
#define     FLDR_ATTR_ESCAPE                83  //转义字符
#define     FLDR_ATTR_SQLFILE               84  //SQL文件

#define     FLDR_CTRL_ROW_ARRAY_SIZE        25

#define     FLDR_DEFAULT_ROW_NUM            5000

#define     FLDR_CTRL_CONFLICT_IGNORE       0
#define     FLDR_CTRL_CONFLICT_OVERWRITE    1

#define     FLDR_UDP_SINGLE                 1UL
#define     FLDR_UDP_MULTI                  2UL
#define     FLDR_UDP_DEFAULT                FLDR_UDP_MULTI

#define     FLDR_INET_TCP                   0UL
#define     FLDR_INET_UDP                   1UL
#define     FLDR_INET_IPC                   2UL
#define     FLDR_INET_UNIXSOCKET            4UL //unixsocket [UIPC -des-3]
#define     FLDR_INET_DEFAULT               FLDR_INET_TCP 

#define     FLDR_OSAUTH_OFF                 0
#define     FLDR_OSAUTH_SYSDBA              1
#define     FLDR_OSAUTH_SYSSSO              2
#define     FLDR_OSAUTH_SYSAUDITOR          3
#define     FLDR_OSAUTH_AUTO                4
#define     FLDR_OSAUTH_USERS               5

#define    FLDR_ARG_FIELDS  "FIELDS"
#define    FLDR_ARG_RECORDS "RECORDS"
#define    FLDR_ARG_BADFILE "BADFILE"
#define    FLDR_ARG_LOG     "LOG"
#define    FLDR_ARG_SKIP    "SKIP"
#define    FLDR_ARG_CHARSET  "CHARACTER_CODE" 
#define    FLDR_ARG_NSTR    "NULL_STR" 
#define    FLDR_ARG_ERRORS  "ERRORS" 

/************************************************************************
purpose:
fldr设置环境变量
/************************************************************************/
DllExport
FLDR_RETURN 
fldr_init_env();

DllExport
FLDR_RETURN 
fldr_alloc(
    fhinstance*    instance
);

/*********************************************************************
Purpose :
    fldr初始化
*********************************************************************/
DllExport
void
fldr_init();

/*********************************************************************
Purpose :
    fldr结束初始化
*********************************************************************/
DllExport
void
fldr_deinit();

/*********************************************************************
Purpose :
    ldr申请instance
*********************************************************************/
DllExport
FLDR_RETURN 
ldr_alloc(
    fhinstance*         instance,       /* IN: 实例信息 */
    ulint               role            /* IN: 角色信息 */
);

DllExport
FLDR_RETURN 
fldr_free(
    fhinstance      instance
);

DllExport
FLDR_RETURN 
fldr_set_attr(
    fhinstance      instance, 
    fsint4          attrid, 
    fpointer        value, 
    fsint4          length
);

/***********************************************************************
Purpose:
    从instance上获取参数值
***********************************************************************/
DllExport
FLDR_RETURN     /*返回码*/
fldr_get_attr(
    fhinstance      instance,   /*IN:装载实例*/
    fsint4          attrid,     /*IN:属性编号*/
    fpointer        buffer,     /*IN:参数数据接收缓冲区*/
    fsint4          buf_sz,     /*IN:缓冲区大小*/
    fsint4*         length      /*OUT:实际参数数据长度*/
);

DllExport
FLDR_RETURN 
fldr_initialize(
    fhinstance      instance, 
    fsint4          type,
    fpointer        conn,
    fchar*          server, 
    fchar*          uid, 
    fchar*          pwd,
    fchar*          table
);

DllExport
FLDR_RETURN 
fldr_uninitialize(
    fhinstance      instance,
    fsint4          flag
);

DllExport
FLDR_RETURN 
fldr_bind(
    fhinstance      instance,
    fsint2          col_idx,
    fsint2          type,
    fchar*          date_fmt,
    fpointer        data,
    fsint4          data_len,
    fsint4*         ind_len
);

DllExport
FLDR_RETURN 
fldr_bind_nth(
    fhinstance      instance,
    fsint2          col_idx,
    fsint2          type,
    fchar*          date_fmt,
    fpointer        str_binds,
    fpointer        data,
    fsint4          col_len,
    fsint4          data_len,
    fsint4*         ind_len,
    fsint4          nth
);

DllExport
FLDR_RETURN 
fldr_sendrows(
    fhinstance      instance,
    fsint4          rows
);

DllExport
FLDR_RETURN 
fldr_sendrows_nth(
    fhinstance      instance,
    fsint4          rows,
    fsint4          nth,
    fsint4          seq_no
);

DllExport
FLDR_RETURN 
fldr_sendrows_nth_batch(
    fhinstance      instance,
    fsint4          rows,
    fsint4          nth,
    fsint4          seq_no,
    void**          task_node_io,
    fsint4*         is_send
);

DllExport
FLDR_RETURN
fldr_sendrows_nth_2_over(
    fhinstance      instance,
    fsint4          seq_no,
    void*           task_node
);

DllExport
FLDR_RETURN 
fldr_control(
    fhinstance      instance,
    fsint4          attrid,
    fpointer        value, 
    fsint4          length
);

DllExport
FLDR_RETURN 
fldr_col_fmt(
    fhinstance      instance,
    fsint2          col_idx,
    fpointer        delemiter,
    fsint2          delemiter_len,
    fpointer        encloser,
    fsint2          encloser_len,
    fpointer        line_ender,
    fsint2          line_ender_len
);

DllExport
FLDR_RETURN 
fldr_batch(
    fhinstance      instance,
    fsint8          *rows
);

DllExport
FLDR_RETURN 
fldr_finish(
    fhinstance      instance
);

DllExport
FLDR_RETURN 
fldr_get_diag(
    fhinstance      instance,
    fsint4          rec_num,
    fsint4*         err_code,
    fchar*          err_msg,
    fsint4          buf_sz,
    fsint4*         msg_len
);

DllExport
FLDR_RETURN
fldr_exec_ctl(
    fhinstance      instance,
    fchar*          ctl_path
);

/*********************************************************************
purpose :
    无控制文件的情况下导入/出数据
*********************************************************************/
DllExport
FLDR_RETURN /*返回值*/
fldr_exec_without_ctl_init(
    fhinstance      instance        /* IN: 装载实例 */
);

DllExport
FLDR_RETURN
fldr_exec_without_ctl(
    fhinstance      instance
);

/*********************************************************************
purpose :
    根据控制文件执行装载，调用会被dmfldr和dts调用，需要传入fldr_type区别
*********************************************************************/
DllExport
FLDR_RETURN /*返回值*/
fldr_exec_ctl_low(
    fhinstance      instance,       /*IN:装载实例*/
    fchar*          ctl_buf,        /*IN:控制文件内容*/
    fsint4          fldr_type,      /*IN:装载类型*/
    fsint8*         row_count       /*OUT:提交数据行数*/
);

DllExport
fsint8
fldr_get_inst_send_rows(
    fhinstance      instance
);

DllExport
FLDR_RETURN
fldr_jni_add_diag(
    fhinstance      instance,
    fsint4          err_code,
    fchar*          err_msg
);

DllExport
FLDR_RETURN
fldr_put_lob(
    fhinstance      instance,
    fsint2          colid,
    fsint2          type,
    fpointer        data,
    fsint4          data_len,
    fsint4          is_last_piece,
    fchar*          blob_head
);

/************************************************************************
purpose:
fldr处理配置文件
/************************************************************************/
DllExport
dmcode_t
fldr_resolve_ini(
    dmenv_t         env,    /* IN：环境句柄 */
    void*           args    /* IN：启动参数 */
);

/************************************************************************
purpose:
fldr读取配置ini文件, 并进行语法分析
/************************************************************************/
dmcode_t
fldr_read_ini(
    schar*          inifile,    /* IN: 配置文件路径 */
    fargs_t*        fargs   /* OUT: 配置文件参数 */
);

/*********************************************************************
Purpose :
    ldrp初始化内存监控。
*********************************************************************/
DllExport
dmcode_t
fldr_mon_sys_init(
    dmenv_t         env,    /* IN：环境句柄 */
    schar*          pro_name /* IN: 监控系统名 */
);

/*********************************************************************
Purpose :
    ldr清理内存监控。
*********************************************************************/
DllExport
void
fldr_mon_sys_destory(
    dmenv_t     env     /* IN：环境句柄 */
);

#ifdef __cplusplus
}
#endif



#endif //_DFLDR_H
