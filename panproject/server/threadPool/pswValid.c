#include "head.h"
#include <shadow.h>
#include <crypt.h>

int pswValidation(const char*username,const char*password){
    struct spwd *userinfo=(struct spwd*)calloc(1,sizeof(struct spwd));
    userinfo=getspnam(username);
    if(userinfo==NULL){
        printf("cant find the user\n");
        return -1;

    }
    char pswbuf[1024]={0};
    char temp[128]={0};
    char*p=userinfo->sp_pwdp;
    int i=0;
    int length=0;
    while(*p!='\0'){
        temp[i++]=*p;
        if((*p)=='$'){
            strcat(pswbuf,temp);
            bzero(temp,sizeof(temp));
            length+=i;
            i=0;
        }
        ++p;
    }
    pswbuf[length]='\0';
    char*crypt_passwd=crypt(password,pswbuf);
    printf("in func: userinfo_pwd = %s\n", userinfo->sp_pwdp);
    printf("in func: crypt_pwd = %s\n", crypt_passwd);
    
    if(strcmp(crypt_passwd,userinfo->sp_pwdp)!=0){
        return -1;
    }
    return 0;
}
