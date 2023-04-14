#include "threadPool.h"
#include "head.h"
typedef enum
{
    //cd       remove
    TYPE_CD,TYPE_REMOVE,
    //ls    pwd
    TYPE_LS,TYPE_PWD,
    //mkdir       rmdir
    TYPE_MKDIR,TYPE_RMDIR,
    //gets     puts
    TYPE_GETS,TYPE_PUTS

}shell_type;


void make_vtpath(char *virtual_path, char arg[])
{
    if(arg[0] == '/'|| arg[0] == '~')
    {
        strcpy(virtual_path, arg);
        return;
    }
    char *p;
    p = strtok(arg, "/");
    while(p != NULL)
    {
        if(strcmp(p, ".") == 0)
        {
        }
        else if(strcmp(p, "..") == 0)
        {
            int len = strlen(virtual_path);
            int i = len - 1;
            virtual_path[i - 1] = '\0'; 
            --i;
            for(; (virtual_path)[i] != '/'; i--)
            {
                (virtual_path)[i] = '\0';
            }
        }
        else
        {
            strcat(virtual_path, p);
            strcat(virtual_path, "/");
        }
        p = strtok(NULL, "/");
    }

}

int sendcmd(int netfd){
    char virtual_path[256] = "./";
    char concat_path[256] = "./";
    train_t train;
    int ret;
    int username_len;
    char username[128];
    int password_len;
    char password[128];
    while(1)
    {
        recv(netfd, &username_len,sizeof(int), MSG_WAITALL);
        recv(netfd, username, username_len, MSG_WAITALL);
        recv(netfd, &password_len,sizeof(int), MSG_WAITALL);
        recv(netfd, password, password_len, MSG_WAITALL);
        ret = pswValidation((const char*)username,
                            (const char*)password);
        send(netfd, &ret, sizeof(int), MSG_NOSIGNAL);
        printf("name = %s\n", username);
        printf("length of name = %d\n", (int)strlen(username));
        printf("pswd = %s\n", password);
        printf("ret = %d\n", ret);
        if(ret == 0)
        {
            LOG_LOGIN(username);
            printf("login succeed!\n");
            break;
        }
        printf("login failed!\n");
    }
    while(1)
    {
        int cmd;
        int cmd_len;
        
        int testlen;
        recv(netfd, &cmd_len, sizeof(int), MSG_WAITALL);
        
        recv(netfd, &cmd, sizeof(cmd),MSG_WAITALL);
        printf("cmd = %d\n", cmd);
        int arg_len;
        char *arg;
        char tmp[128];
        strcpy(tmp, virtual_path);
        recv(netfd, &arg_len, sizeof(int),  MSG_WAITALL);
        if(arg_len != 0)
        {
            
        
            printf("arg_len = %d\n", arg_len);
            arg = (char *)calloc(1, arg_len + 1);
            arg[arg_len] = '\0';
            recv(netfd, arg, arg_len, MSG_WAITALL);
            printf("arg = %s\n", arg);
        }
        else
        {
            arg = (char *)calloc(1, 1);
        }
        switch(cmd)
        {
        case(TYPE_CD):
            make_vtpath(virtual_path, arg);
            printf("arg = %s\n", arg);
            ret = changeDir(virtual_path);
            printf("ret = %d\n", ret);
            printf("vpath = %s\n", virtual_path);
            send(netfd, &ret, sizeof(int), MSG_NOSIGNAL);
            if(ret < 0)
            {
                printf("the tmp = %s\n", tmp);
                strcpy(virtual_path, tmp);
                LOG_ERROR(username, "cd");
            }   
            else
            {
                LOG_MSG(username, "cd");
            }
            //strcpy(concat_path, virtual_path);
            break;

        case(TYPE_REMOVE):
            make_vtpath(concat_path, arg);
            printf("concat path = %s\n", concat_path);
            printf("ret = %d\n", ret);
            send(netfd, &ret, sizeof(int), MSG_NOSIGNAL);
            testlen = strlen(concat_path);
            concat_path[testlen - 1] = '\0';

            ret = rmFile(concat_path);   
            if(ret < 0)
            {
                printf("the tmp = %s\n", tmp);
                LOG_ERROR(username, "rmFile");
            }   
            else
            {
                LOG_MSG(username, "rmFile");
            }
            //strcpy(concat_path, virtual_path);
            break;

        case(TYPE_LS):
            make_vtpath(concat_path, arg);
            ls(netfd, (const char*)concat_path);
            LOG_MSG(username, "ls");
            //strcpy(concat_path, virtual_path);
            break;

        case(TYPE_PWD):
            train.length = (int)strlen(virtual_path);
            send(netfd, &train.length, sizeof(int), MSG_NOSIGNAL);
            send(netfd, virtual_path, train.length, MSG_NOSIGNAL);
            //strcpy(concat_path, virtual_path);
            break;

        case(TYPE_MKDIR):
            make_vtpath(concat_path, arg);
            testlen = (int)strlen(concat_path);
            printf("tln = %d\n", testlen);
            concat_path[testlen - 1] = '\0';
            printf("catpath = %s, arg = %s\n", concat_path,
                   arg);
            ret = mkDir((const char *)concat_path);
            send(netfd, &ret, sizeof(int), MSG_NOSIGNAL);
            //strcpy(concat_path, virtual_path);
            if(ret < 0)
            {
                printf("the tmp = %s\n", tmp);
                LOG_ERROR(username, "mkDir");
            }   
            else
            {
                LOG_MSG(username, "mdDir");
            }
            break;

        case(TYPE_RMDIR):
            make_vtpath(concat_path, arg);
            ret = rmDir((const char *)concat_path);
            send(netfd, &ret, sizeof(int), MSG_NOSIGNAL);
            //strcpy(concat_path, virtual_path);
            if(ret < 0)
            {
                printf("the tmp = %s\n", tmp);
                LOG_ERROR(username, "rmDir");
            }   
            else
            {
                LOG_MSG(username, "rmDir");
            }
            break;

        case(TYPE_GETS):
            make_vtpath(concat_path, arg);
            testlen = (int)strlen(concat_path);
            printf("tln = %d\n", testlen);
            concat_path[testlen - 1] = '\0';
            printf("catpath = %s\n", concat_path);
            printf("arg = %s\n", arg);
            sendFile((const int )netfd,(const char *)concat_path);
            //strcpy(concat_path, virtual_path);
            break;

        case(TYPE_PUTS):
            make_vtpath(concat_path, arg);
            testlen = (int)strlen(concat_path);
            printf("tln = %d\n", testlen);
            concat_path[testlen - 1] = '\0';
            printf("catpath = %s\n", concat_path);
            printf("arg = %s\n", arg);
            recvFile((const int)netfd, (const char*)concat_path);
            //strcpy(concat_path, virtual_path);
            break;
        }
        strcpy(concat_path, virtual_path);
        free(arg);
    }

    return 0;
}
