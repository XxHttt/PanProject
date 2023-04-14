#include"client.h"

int main(int argc,char * argv[])
{
    // ./client x.x.x.x 4321
    ARGS_CHECK(argc,3);
    int sockfd=TCPinit(argv);

    int count_login=0;
    while(1)
    {
        int ret_login=login(sockfd);
        if(ret_login==-1)++count_login;
        else
        {
            while(1)
            {
                char buf[1024]={0};
                fgets(buf,1024,stdin);
                buf[strlen(buf)-1]='\0';
                char *shell=strtok(buf," ");
                shell_type type=judge_shell(shell,strlen(shell));




                if(type==TYPE_NO_ONE)
                {
                    printf("The shell is wrong,please try again\n");
                    continue;
                }
                else if(type==TYPE_EXIT)
                {
                    goto ends;
                }
                train_t train;
                int length=sizeof(type);
                send(sockfd,&length,sizeof(int),MSG_NOSIGNAL);
                send(sockfd,&type,length,MSG_NOSIGNAL);

                shell=strtok(NULL," ");
                if(shell == NULL)
                {
                    length = 0;
                }
                else
                    length=(int)strlen(shell);

                send(sockfd,&length,sizeof(int),MSG_NOSIGNAL);
                send(sockfd,shell,length,MSG_NOSIGNAL);
                switch(type)
                {
                    case TYPE_CD: get_cd(sockfd);break;
                    case TYPE_REMOVE: get_remove(sockfd);break;
                    case TYPE_LS: get_ls(sockfd);break;
                    case TYPE_PWD: get_pwd(sockfd);break;
                    case TYPE_MKDIR: get_mkdir(sockfd);break;
                    case TYPE_RMDIR: get_rmdir(sockfd);break;
                    case TYPE_GETS: {

                                        recvFile(sockfd, shell);
                                        break;
                                    }
                    case TYPE_PUTS: {
                                        sendFile(sockfd,shell);
                                        break;
                                    }
                }

            }
        }

        if(count_login==5)
        {
            printf("Too many failures !\n");
            break;
        }

    }
ends:
    printf("client is close! \n");
    close(sockfd);
    return 0;
}

