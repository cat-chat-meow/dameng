#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_KEY_LENGTH 256
#define MAX_VALUE_LENGTH 256

#define CONFIG_INI "../etc/config.ini"

int read_config_(void)
{
    FILE *fp;
    char buffer[MAX_LINE_LENGTH];
    char key[MAX_KEY_LENGTH], value[MAX_VALUE_LENGTH];

    fp = fopen(CONFIG_INI, "r");
    if (fp == NULL)
    {
        printf("Can not open config file!\n");
        return 1;
    }

    while (fgets(buffer, MAX_LINE_LENGTH, fp))
    {
        if (strlen(buffer) <= 1) // 忽略空行
            continue;

        sscanf(buffer, "%s = %s", key, value);
        printf("key: %s, value: %s\n", key, value);
    }

    fclose(fp);

    return 0;
}

void trim_newline(char *str)
{
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0'; // 替换\n为字符串结束符\0
    }
}

typedef struct
{
    char db_name[100];
    char db_server[100];
    char db_user[100];
    char db_pwd[100];
} db_config;

db_config read_config(void)
{
    db_config config;
    FILE *fp = fopen(CONFIG_INI, "r");
    if (fp == NULL)
    {
        printf("Can not open config file! %s \n", CONFIG_INI);
        return config;
    }
    char buf[1024];
    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        char *token = strtok(buf, "=");
        if (strcmp(token, "DB_NAME") == 0)
        {
            token = strtok(NULL, "=");
            trim_newline(token);
            strcpy(config.db_name, token);
        }
        if (strcmp(token, "DB_SVR") == 0)
        {
            token = strtok(NULL, "=");
            trim_newline(token);
            strcpy(config.db_server, token);
        }
        if (strcmp(token, "UID") == 0)
        {
            token = strtok(NULL, "=");
            trim_newline(token);
            strcpy(config.db_user, token);
        }
        if (strcmp(token, "PWD") == 0)
        {
            token = strtok(NULL, "=");
            trim_newline(token);
            strcpy(config.db_pwd, token);
        }
    }
    fclose(fp);
    printf("parser config, db_name %s db_server %s db_user %s db_pwd %s \n",
           config.db_name, config.db_server, config.db_user, config.db_pwd);
    return config;
}
