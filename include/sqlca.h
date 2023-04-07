#ifndef _DM_SQLCA_H
#define _DM_SQLCA_H

//#include "dpc_dll.h"

typedef struct sqlca_struct		sqlca_t;
struct sqlca_struct {
    char    sqlcaid[8];
    int     sqlcabc;
    int     sqlcode;
    struct {
        unsigned short  sqlerrml;
        char            sqlerrmc[70];       //同oracle保持一致，错误信息过长时，会截断，只显示前70个字符
    } sqlerrm;
    char    sqlerrp[8];
    int     sqlerrd[6];
    char    sqlwarn[11];
    char    sqlstate[6];
};

sqlca_t sqlca;

#define SQLCA_HAS_WARN		(sqlca.sqlcode > 0)
#define SQLCA_NOT_FOUND		(sqlca.sqlcode == 100)
#define SQLCA_HAS_ERROR		(sqlca.sqlcode < 0)


#endif //_DM_SQLCA_H
