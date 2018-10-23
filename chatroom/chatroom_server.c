/*#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#define SERV_PORT        8888  //端口号
#define LISTENQ          30    //accept队列长度
#define BUFSIZE          1024  
#define LEN     sizeof(struct users)
#define LEN1    sizeof(struct message)
#define BUFSIZE          LEN1 
int recv_t;


pthread_mutex_t          mutex;
int                      y=0;  //分配离线消息的结构体
struct user                    //用户名的结构体
{
    char username[10];
};

struct regi_sign               //登录注册用的结构体
{
    char flag;
    char username[10];
    char password[10];
};

struct group                   //群组的结构体
{
    char group_name[10];
    char admin_name[10];
    struct user member[10];
    int  member_num;
};

struct connect_info            //conn_fd登录结构体
{
    int fd;
    char name[10];
}connect_info[30];

struct File
{
    char file_name[30];
    char file_message[500];
    int file_len;
};

struct users                   //用户结构体
{
    struct user user;          //用户名
    char password[10];         //密码
    int  friend_num;           //朋友数
    int  group_num;            //群数
    struct user friend[10];    //朋友信息(朋友昵称)
    struct group group[10];    //群名
    struct users *next;
};

struct leave_message           //离线消息结构体
{
    char  name[10];
    int   number;
}leave_message[30];

struct users*  head = NULL;    //用户信息头指针
struct users*  p;              //指向当前用户的指针

struct message                 //消息结构体
{
    char      type;
    char      from[10];
    char      to[10];
    char      time[30];
    char      changefriname[10];//添加好友和删除
    struct    user name[30]; //附近的人，在线好友。查看好友
    struct    group group;
    char      news[500];
    struct File  file;
};

struct users* read_input();    //将信息读入链表
struct users* apply_account(struct regi_sign,int);  //申请账户
void save();                   //保存信息到链表
char * my_time();              //自定义时间函数
struct users* sign_in(struct regi_sign,int); //用户登录
void quit(struct regi_sign,int);             //用户未登录退出
void near_friend(struct message,int);        //附近的人
void view_friend(struct message,int);        //查看好友
void add_friend(struct message,int);         //添加好友
void out_line(struct message,int);           //下线
void view_online_friend(struct message,int); //查看在线好友
int del_friend(struct message,int);         //删除好友
int pri_chat(struct message,int);            //私聊
void build_group(struct message,int);        //群聊
void view_group(struct message,int);         //查看我的组
void public_chat(struct message,int);        //群聊
int accept_add_friend(struct message,int);
void leave_message_func(struct regi_sign,int);//离线消息
void apply_add_group(struct message,int);
void accept_add_group(struct message,int);
void refuse_add_group(struct message,int);
void my_status(struct message,int);
int my_zone_access(struct message,int);
void my_err(const char *err_string, int line);

int my_recv(int conn_fd, char* data_buf, int len, int flags)
{
    char recv_buf[BUFSIZE];
    char *pread;//指向下一次读取数据的位置
    int len_remin = 0;//自定义缓冲区剩余字数
    int i;

    // 如果自定义缓冲区中没有数据,则从套接字中读取数据
    if(len_remin <=0)
    {
        if((len_remin = recv(conn_fd, recv_buf, sizeof(recv_buf), flags)) <0)
        {
            my_err("recv",__LINE__);
        }
        else if(len_remin == 0)
        {
            return 0;
        }
        pread = recv_buf;//初始化pread指针
    }

    //从自定义缓冲区中读取一次数据
    for(i = 0; *pread != '\n'; i++)
    {
        if(i>len)//防止指针越界
        {
            return -1;
        }
        data_buf[i]=*pread++;
        len_remin--;
    }
    len_remin--;
    pread++;
    return i;
}

int my_zone_access(struct message chat,int i)
{
    struct message near;
    char   near_buf[BUFSIZE];
    int    k;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(near.from,connect_info[k].name))
            break;
    }
    if(near.type == '8')
    {
        memcpy(near_buf,&near,LEN1);
        send(connect_info[k].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    if(near.type == '9')
    {
        memcpy(near_buf,&near,LEN1);
        send(connect_info[k].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    if(near.type == '0')
    {
        memcpy(near_buf,&near,LEN1);
        send(connect_info[k].fd,near_buf,BUFSIZE,0);
        return 0;
    }
}
void my_err(const char* err_string, int line )
{
    fprintf(stderr, "line: %d", line);
    perror(err_string);
    exit(1);
}
void my_status(struct message chat,int i)
{
    struct message near;
    struct users *p1;
    char   near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    p1 = head->next;
    int k,j;

    while(p1)
    {
        if(!strcmp((p1->user).username,near.from))
        {
            for(k = 0;k < 10;k++)
            {
                for(j = 0;j < 30;j++)
                {
                    if(!strcmp((p1->friend)[k].username,connect_info[j].name))
                    {
                        memcpy(near_buf,&near,LEN1);
                        send(connect_info[j].fd,near_buf,BUFSIZE,0);
                        break;
                    }
                }
            }
        }
        p1 = p1->next;
    }
}
void refuse_add_group(struct message chat,int i)
{
    int k;
    struct message near;
    char near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    memset(near_buf,0,LEN1);
    near = chat;
    memcpy(near_buf,&near,LEN1);
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
    }
}
void accept_add_group(struct message chat,int i)
{
    int k,j,t,m,flag = 0;
    struct users *p1,*p2,*p3;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct user  name[10];
    struct message near;
    struct group group;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!strcmp(near.group.admin_name,(p1->user).username))   
        {
            for(k = 0;k < 10;k++)                    
            {
                if(!strcmp((p1->group)[k].group_name,near.to))  //找到指定群
                {
                    for(j = 0;j < 10 ;j++)
                    {
                        memcpy(&name[j].username,&(p1->group)[k].member[j].username,10);
                    }
                }
            }
        }
        p1 = p1->next;
    }
    for(k = 0;k < 10;k++)
    {
        p2 = head->next;
        while(p2)
        {
            if(!strcmp(name[k].username,(p2->user).username))
            {

                for(m = 0;m < 10;m++)                    
                {
                    if(!strcmp((p2->group)[m].group_name,near.to))//找到指定组
                    {
                        memcpy(&(p2->group)[m].member[(p2->group)[m].member_num],&near.from,10);
                        t = (p2->group)[m].member_num;
                        (p2->group)[m].member_num = (p2->group)[m].member_num + 1;
                        flag = 1;
                        break;
                    }
                }
                if(flag == 1)
                    break;
                
            }
            p2 = p2->next;
        }
    }
    memcpy(&group.group_name,&near.to,10);
    memcpy(&group.admin_name,&near.group.admin_name,10);
    for(k = 0;k < t;k++)
    {
        memcpy(&(group.member[k].username),&name[k].username,10);
    }
    memcpy(&(group.member[t]),&near.from,10);
    p3 = head->next;
    while(p3)
    {
        if(!(strcmp(near.from,(p3->user).username)))      //查找到每一个成员，将他们的组直接赋值
        {
            (p3->group)[p3->group_num] = group;
            (p3->group)[p3->group_num].member_num = t+1;
            (p3->group_num)++;
            break;
        }
        p3 = p3->next;
    }
    save();
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
void apply_add_group(struct message chat,int i)
{
    struct message near;
    struct users *p1;
    char near_buf[BUFSIZE];
    int k,flag = 0;
    char name[10];           //用来保存群主的名字
    p1 = head->next;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    memcpy(near_buf,&near,LEN1);
    while(p1)
    {
        for(k = 0;k < 10;k++)
        {
            if(!(strcmp((p1->group)[k].group_name,near.to)))
            {
                memcpy(name,(p1->group)[k].admin_name,10);
                memcpy(near.group.admin_name,name,10);
                flag = 1;
                break;
            }
        }
        if(flag == 1)
            break;
        p1 = p1->next;
    }
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,name))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
void leave_message_func(struct regi_sign account,int i) //离线消息
{
    struct message chat;
    char   near_buf[BUFSIZE];
    int k,j;
    FILE *fp;
    memset(&chat,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    for(k = 0;k < 30;k++)
    {
        if(!(strcmp(account.username,leave_message[k].name)))   //找到用户的离线结构体
        {
            if(leave_message[k].number != 0)                    //如果离线消息数不为0
            {
                if((fp = fopen(leave_message[k].name,"a+")) == NULL) //从文件中读取
                {
                    my_err("fopen",__LINE__);
                    exit(0);
                }
                while((fread(&chat,LEN1,1,fp) != -1) && (!feof(fp)))
                {
                    chat.type = 'x';
                    memcpy(near_buf,&chat,LEN1);
                    if(send(connect_info[i].fd,near_buf,BUFSIZE,0)!=BUFSIZE) //将消息逐条发过去
                    {
                        my_err("send",__LINE__);
                        pthread_exit(0);
                    }
                }
                fclose(fp);
                leave_message[k].number = 0;                    //将离线消息数至为0
                unlink(leave_message[k].name);                  //删除文件
                
            }
        }
    }

}
void public_chat(struct message chat ,int i)                     //群聊
{
    int k,j;                                  
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!strcmp(near.from,(p1->user).username))   //找到用户
        {
            for(k = 0;k < 10;k++)                    
            {
                if(!strcmp((p1->group)[k].group_name,near.to))  //找到指定组
                {
                    for(j = 0;j < 30;j++)
                    {
                        if((!strcmp(connect_info[j].name,(p1->group)[k].member[j].username)) && (connect_info[j].fd != connect_info[i].fd))
                        {
                            memcpy(near_buf,&near,LEN1);
                            if(send(connect_info[j].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                            {
                                my_err("send",__LINE__);
                                pthread_exit(0);
                            }
                        }

                    }
                }
            }
        }
        p1 = p1->next;
    }
}
void view_group(struct message chat,int i)             //查看我的组
{
    int k ;
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!strcmp(near.from,(p1->user).username))     //找到用户
        {
            for(k = 0;k < 10;k++)
            {
               if(!strcmp((p1->group)[k].group_name,near.to)) //找到组
                    near.group = (p1->group)[k];
            }
        }
        p1 = p1->next;
    }
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void build_group(struct message chat,int i)                  //建群
{
    int k ;
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    struct message chat1;
    struct user  name[10];
    memset(&near,0,LEN1);
    memset(&chat1,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    chat1 = chat;
    for(k = 0; k < 10; k++)
    {
        memcpy(name[k].username,near.group.member[k].username,10);   //先将成员保存到name[]数组里
    }
    while(p1)
    {
        for(k = 0;k < 10;k++)
        {
            if(!(strcmp(name[k].username,(p1->user).username)))      //查找到每一个成员，将他们的组直接赋值
            {
                (p1->group)[p1->group_num] = near.group;
                (p1->group_num)++;
            }
        }
        p1 = p1->next;
    }
    save();
    printf("[用户]“%s”创建了群 “%s”\t“%s”\n",near.from,near.group.group_name,my_time());
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)       //给群主通知群建立成功
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
    for(k = 0;k < 30;k++)
    {
        if((!strcmp(name[k].username,connect_info[k].name)) && (strcmp(name[k].username,near.from)))
        {
            chat1.type = 'r';
            memset(&near_buf,0,BUFSIZE);
            memcpy(near_buf,&chat1,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);             //给其他群成员通知有人邀请你加入群
        }
    }
}
int pri_chat(struct message chat,int i)                                //私聊
{
    struct users *p1;
    p1 = head->next;
    FILE *fp;
    int  k;
    struct  message  near;
    char    near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    if(!strcmp(near.to,near.from))
    {
        near.type = '6';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    while(p1)
    {
        if(!strcmp((p1->user).username,near.to))
        {
            for(k = 0;k < 30;k++)
            {
                if(!strcmp(near.to,connect_info[k].name))                      //如果在线直接发送
                {
                    near.type = 'o';
                    memcpy(near_buf,&near,LEN1);
                    if(send(connect_info[k].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                    {
                        my_err("send",__LINE__);
                        pthread_exit(0);
                    }
                    return 0;
                }
            }
            if(k == 30)
            {
                near.type = 'd';                                               //如果不在线就转为离线消息
                memcpy(near_buf,&near,LEN1);
                if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                {
                    my_err("send",__LINE__);
                    pthread_exit(0);
                }
                if((fp = fopen(near.to,"ab+")) == NULL)
                {
                    my_err("fopen",__LINE__);
                    exit(0);
                }
                fwrite(&near,LEN1,1,fp);
                fclose(fp);
                for(k = 0;k < 30;k++)
                {
                    if(!(strcmp(leave_message[k].name,near.to)))
                        leave_message[k].number = leave_message[k].number+1;
                }
                return 0;
            }
        }
        p1 = p1->next;
    }
    if(p1 == NULL)
    {
        near.type = '5';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
    }
}
int del_friend(struct message chat,int i)                     //删除好友
{ 
    struct  users *p1;
    p1 = head->next;
    int k;
    struct message near;
    memset(&near,0,LEN1);
    near = chat;
    char  near_buf[BUFSIZE];
    if(!strcmp(near.to,near.from))
    {
        near.type = '3';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from)))           //找到本人
        {
            for(k = 0;k < 10;k++)
            {
                if(!(strcmp(near.to,(p1->friend)[k].username)))
                {
                    for(k;k < 10;k++)
                    {
                        memcpy((p1->friend)[k].username,(p1->friend)[k+1].username,10);//将数组后的元素依次向前移动
                    }
                    p1->friend_num--;     //将好友总数减一
                    save();
                    printf("[用户]“%s”删除了好友“%s” \t“%s”\n",near.from,near.to,my_time());
                    memset(near_buf,0,BUFSIZE);
                    memcpy(near_buf,&near,LEN1);
                    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                    {
                        my_err("send",__LINE__);
                        pthread_exit(0);
                    }
                    return 0;
                }
            }
            
        }        
        p1 = p1->next;
    }
    if(p1 == NULL)
    {
        near.type = '1';
        memset(near_buf,0,BUFSIZE);
        memcpy(near_buf,&near,LEN1);
        if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
        {
            my_err("send",__LINE__);
            pthread_exit(0);
        }
    }
}

void view_online_friend(struct message chat,int i)             //查看在线好友
{ 
    struct  users *p1;                            
    p1 = head->next;
    int k,j;
    struct message near;
    struct user    name[10];
    memset(&name,0,sizeof(struct user));
    memset(&near,0,LEN1);
    near = chat;
    char  near_buf[BUFSIZE];
    near.type = 'z';
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from)))           //查找到那个人
        {
            for(k = 0;k < 10;k++)
                memcpy(name[k].username,(p1->friend)[k].username,10);//将好友全部保存到name[]数组里
            for(k = 0;k < 10;k++)
            {
                for(j = 0;j < 30;j++)
                {
                    if((!strcmp(name[k].username,connect_info[j].name)) && (connect_info[j].fd != -1))//依次检查各个好友的是否在线
                        {
                            memcpy(near.name[k].username,connect_info[j].name,10);        //再将在线的好友保存到name[]中
                        }
                }
            }
            
        }
        p1 = p1->next;
    }
    memset(near_buf,0,BUFSIZE);
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void out_line(struct message chat,int i)          //下线，结束自己的线程
{
    struct message near;
    memset(&near,0,LEN1);
    near = chat;
    connect_info[i].fd = -1;                       //将信息初始化
    strcpy(connect_info[i].name," ");
    printf("[用户]%s下线了 \t%s\n",near.from,my_time());
    pthread_exit(0);
}
void refuse_add(struct message chat,int i)         //拒绝添加
{
    int k ;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);                    //当被添加好友输入n时，发送标志为e的拒绝状态
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
int accept_add_friend(struct message chat,int i)   //第一次的添加消息的转发
{
    struct users *p1;
    p1 = head->next;
    int k;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    if(!strcmp(near.to,near.from))
    {
        near.type = '2';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from)))           //找到本人
        {
            for(k = 0;k < 10;k++)
            {
                if((!(strcmp(near.to,(p1->friend)[k].username))) && (strlen(near.to) != 0))
                {
                    near.type = '4';
                    memcpy(near_buf,&near,LEN1);
                    send(connect_info[i].fd,near_buf,BUFSIZE,0);
                    return 0;
                }
            }
        }
        p1 = p1->next;
    }
    for(k = 0;k < 30;k++)
    {
        if(!(strcmp(connect_info[k].name,near.to))) //找到被添加的人，然后向他发送添加请求
        {
            memcpy(near_buf,&near,LEN1);
            if(send(connect_info[k].fd,near_buf,BUFSIZE,0)!= BUFSIZE)
            {
                my_err("send",__LINE__);
                pthread_exit(0);
            }
            return 0;
        }
    }
    if(k == 30 || (strlen(near.to) == 0))                                   //未找到此人或者他不在线
    {
        near.type = 'm';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);  
    }
}
void add_friend(struct message chat,int i)        //同意添加
{
    int k;
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!(strcmp(near.from,(p1->user).username)))  //互相将对方添加到自己的朋友中
        {
            memcpy(((p1->friend)[p1->friend_num].username),near.to,10);
            p1->friend_num++;
        }
        if(!(strcmp(near.to,(p1->user).username)))
        {
            memcpy(((p1->friend)[p1->friend_num].username),near.from,10);
            p1->friend_num++;  //增加完之后让他的朋友数加1
        }
        p1 = p1->next;
        save();                                       //保存信息
    }
    printf("[用户]“%s”添加了“%s”为好友 \t%s\n",near.from,near.to,my_time());
    memcpy(near_buf,&near,LEN1);
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))   //给自己发送添加成功提示
        {
            if(send(connect_info[k].fd,near_buf,BUFSIZE,0) != BUFSIZE)
            {
                my_err("send",__LINE__);
                pthread_exit(0);
            }
        }
    }
}
void view_friend(struct message chat,int i)           //查看好友
{
    struct  users *p1;
    p1 = head->next;
    int k;
    struct message near;
    memset(&near,0,LEN1);
    near = chat;
    char  near_buf[BUFSIZE];
    near.type = 'v';                                  //查看自己的全部好友
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from))) //找到自己
        {
            for(k = 0;k < 10;k++)
                memcpy(near.name[k].username,(p1->friend)[k].username,10);//将好友保存到数组里
        }
        p1 = p1->next;
    }
    memset(near_buf,0,BUFSIZE);
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)   //将好友信息发送过去
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void near_friend(struct message chat,int i)                  //查看所有在线的人
{
    int  k;
    struct message near;
    char   near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(connect_info[k].fd != -1)                         //只要在线就输出
            memcpy(near.name[k].username,connect_info[k].name,10);
    }
    
    memset(near_buf,0,BUFSIZE);
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void quit(struct regi_sign account,int i)                  //在登录界面退出
{
    struct regi_sign quit;
    quit = account;
    connect_info[i].fd = -1;
    strcpy(connect_info[i].name," ");
    printf("客户端退出 \t%s\n",my_time());
    pthread_exit(0);
}
struct users* sign_in(struct regi_sign account,int i)
{
    int  k,m = 0;
    struct regi_sign  sign;
    struct users  *p1;
    p1 = head->next;
    sign = account;
    for(k = 0;k < 30; k++)                        //判断是否在线
    {
        if(!strcmp(connect_info[k].name,sign.username))
            m = m+1;
    }
   // printf("m:%d\n",m);
    if(m >= 1)                                    //如果已经在线就拒绝登录
    {
        send(connect_info[i].fd,"w",BUFSIZE,0);
        return NULL;
    }
    if(m == 0)                                     //如果不在线
    {
        while(p1)
        {
            if((!strcmp((p1->user).username,sign.username)) && (!strcmp(p1->password,sign.password))) //校验密码
            {
                if((send(connect_info[i].fd,"y",BUFSIZE,0)) != BUFSIZE)                               //密码正确就登录
                {
                    my_err("login send",__LINE__);
                }
                printf("[用户]%s上线了 \t%s\n",sign.username,my_time());
                memcpy(connect_info[i].name,sign.username,10);
                memcpy(leave_message[y++].name,sign.username,10);
            
                return p1;
            }
            p1 = p1->next; //判断未成功，让p1指向下一个
        }
        if(p1 == NULL)                                                                                //没找到，拒绝登录
        {
            send(connect_info[i].fd,"n",BUFSIZE,0);
            return NULL;
        }
    }
}
char * my_time()               //自定义的获取时间的函数
{
    time_t now;
    time(&now);
    return (ctime(&now));
}
void save()                   //将链表信息保存到文件
{
    FILE *fp;
    struct users *p1;
    p1 = head->next;
    if((fp = fopen("userlist","wb")) == NULL)
    {
        my_err("fopen",__LINE__);
        exit(0);
    }
    while(p1 != NULL)
    {
        fwrite(p1,LEN,1,fp);
        p1 = p1->next;
    }
    fclose(fp);

}
struct users* apply_account(struct regi_sign account,int i) //申请账户
{
    struct    regi_sign apply;
    struct    users   *p1,*p2;
    p1 = head;
    apply = account;
    while(p1->next != NULL)
    {
        if(strcmp((p1->next->user).username,apply.username) == 0) //判断是否有重名
        {
            send(connect_info[i].fd,"n",BUFSIZE,0);
            return NULL;
        }
        p1 = p1->next;
    }
    p2 = (struct users*)malloc(LEN);
    memset(p2,0,LEN);         //对刚malloc的结构体进行清空
    strcpy((p2->user).username,apply.username);                  //为新用户申请空间，并且将用户名和密码赋上
    strcpy(p2->password,apply.password);
    p1->next = p2;
    p2->next = NULL;
    save();
    send(connect_info[i].fd,"y",1024,0);
    memcpy(connect_info[i].name,apply.username,10);
    memcpy(leave_message[y++].name,apply.username,10);
    printf("[用户]%s注册成功 \t%s\n",apply.username,my_time());
    return (p2);
}
void *sign_quit()                                               //服务器退出函数
{
    char choice[20];
    do
    {
        printf("-------退出服务器(请输入quit)--------\n");
        scanf("%s",choice);
    }while(strcmp("quit",choice));
    if(!strcmp("quit",choice))
    {
        exit(0);
    }
}
struct users* read_input()                                      //将文件中的信息读到链表
{
    FILE *fp;
    struct users *p1,*p2;
    if((fp = fopen("userlist","rb")) == NULL)
    {
        fp = fopen("userlist","ab+");
    }
    p1 = (struct users *)malloc(LEN);
    p2 = (struct users *)malloc(LEN);
    head = p1;
    fread(p2,LEN,1,fp);
    while(!feof(fp))
    {
        p1->next = p2;
        p1 = p2;
        p2 = (struct users*)malloc(LEN);
        fread(p2,LEN,1,fp);
    }
    p1->next = NULL;
    free(p2);
    fclose(fp);
    return (head);
}

void file(struct message chat,int i)         //拒绝接收文件和同意接收文件
{
    int k ;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);                    
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
void file_send(struct message chat, int i)//发送消息请求
{
    int k ;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);                    
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.to))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}

void *client_t(void *arg)                                       //线程函数，专门用来处理客户端除过登录申请账户之外的请求
{
    int i = *(int *)arg;
    char      recv_buf[BUFSIZE];
    while(1)
    {
        struct    regi_sign account;
        memset(&account,0,sizeof(struct regi_sign));
        memset(recv_buf,0,sizeof(recv_buf));
        //if(recv(connect_info[i].fd,recv_buf,sizeof(recv_buf),0) != BUFSIZE) 
        if((recv_t = my_recv(connect_info[i].fd,recv_buf,sizeof(recv_buf),0))!=BUFSIZE)        
        {
            connect_info[i].fd = -1;
            printf("[用户]%s异常离线 \t%s\n",connect_info[i].name,my_time());
            strcpy(connect_info[i].name," ");
            pthread_exit(0);
        }
        pthread_mutex_lock(&mutex);
        memcpy(&account,recv_buf,sizeof(recv_buf));
        head = read_input(); //保存头结点（读取旧文件 以往的用户等）
        if(account.flag == 'r')  //注册
        {
            p = apply_account(account,i);//p是指向当前
            pthread_mutex_unlock(&mutex);
            leave_message_func(account,i);  //
            if(p != NULL)
                break;
        }
        if(account.flag == 'l')  //登录
        {
            p = sign_in(account,i);
            pthread_mutex_unlock(&mutex);
            leave_message_func(account,i);
            if(p != NULL)
                break;
        }
        if(account.flag == 'q')  //退出
        {
            pthread_mutex_unlock(&mutex);
            quit(account,i);
        }
    }
    while(1)
    {
        struct    message  chat;
        char      recv_buf[BUFSIZE];
        int       ret;
        memset(&chat,0,LEN1);
        memset(recv_buf,0,BUFSIZE);
       // if((ret = recv(connect_info[i].fd,recv_buf,BUFSIZE,0))!= BUFSIZE)
        if((recv_t = my_recv(connect_info[i].fd,recv_buf,sizeof(recv_buf),0))!=BUFSIZE)
        {
         //   printf("ret : %d\n",ret);
         //   printf("%s\n",recv_buf);
            connect_info[i].fd = -1;
            printf("[用户]%s异常离线 \t%s\n",connect_info[i].name,my_time());
            strcpy(connect_info[i].name," ");
            pthread_exit(0);
        }
        memcpy(&chat,recv_buf,BUFSIZE);
        //printf("this is chat.type:%c\n",chat.type);
        switch(chat.type)
        {
            case 'f':   //查看附近的人
            {
                near_friend(chat,i);
                break;
            }
            case 'v':   //查看所有好友
            {
                view_friend(chat,i);
                break;
            }
            case 't':   //添加好友
            {
                add_friend(chat,i);
                break;
            }
            case 'g':   //下线
            {
                out_line(chat,i);
                break;
            }
            case 'z':   //在线好友
            {
                view_online_friend(chat,i);
                break;
            }
            case 's':   //删除好友
            {
                del_friend(chat,i);   
                break;
            }
            case 'o':   //私聊
            {
                pri_chat(chat,i);
                break;
            }
            case 'j':   //建群
            {
                build_group(chat,i);
                break;
            }
            case 'c':   //查看群信息
            {
                view_group(chat,i);
                break;
            }
            case 'b':   //群聊
            {
                public_chat(chat,i);
                break;
            }
            case 'y':   //接受添加好友
            {
                accept_add_friend(chat,i);
                break;
            }
            case 'e':   //拒绝添加
            {
                refuse_add(chat,i);
                break;
            }
            case 'h':   //申请入群
            {
                apply_add_group(chat,i);
                break;
            }
            case 'a':   //同意入群
            {
                accept_add_group(chat,i);
                break;
            }
            case 'i':
            {
                refuse_add_group(chat,i);
                break;
            }
            case '7':
            {
                my_status(chat,i);
                break;
            }
            case '8':
            {
                my_zone_access(chat,i);
                break;
            }
            case '9':
            {
                my_zone_access(chat,i);
                break;
            }
            case '0':
            {
                my_zone_access(chat,i);
                break;
            }
            case 'p': file_send(chat,i); break;
            case '&': file_send(chat,i); break;
            case '$': file_send(chat,i); break;
            case 'u': file(chat,i); break;
            case 'w': file(chat,i); break;


        }
    }
}
int main(int argc,char *argv[])
{
    int                          sock_fd,conn_fd;
    struct     sockaddr_in       serv_addr,conn_addr;
    socklen_t                    conn_len;
    pid_t                        pid;
    char                         recv_buf[128];
    int                          ret,i;
    int                          optval;
    time_t                       now;
    pthread_t                    thid,quit_thid;
    int                          status;

    pthread_mutex_init(&mutex,NULL);
    if((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0) //创建套接字
    {
        my_err("socket",__LINE__);
    }
    optval = 1;
    
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void*)&optval,sizeof(int)) < 0) //设定端口，要是异常退出還可以在绑定
    {
        my_err("setsockopt",__LINE__);
    }

    memset(&serv_addr,0,sizeof(struct sockaddr_in));                   //初始化服务器端端口，IP信息
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    pthread_create(&quit_thid,NULL,(void*)sign_quit,NULL);
    if(bind(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in)) < 0)//将IP与端口绑定在一起
    {
        my_err("bind",__LINE__);
    }
    if(listen(sock_fd,LISTENQ) < 0)                                    //将套接字转化为接听套接字
    {
        my_err("listen",__LINE__);
    }
    for(i = 0;i < 30;i++)                                             //初始化离线消息和conn_fd结构体信息
    {
        connect_info[i].fd = -1;
        strcpy(connect_info[i].name," ");
        strcpy(leave_message[i].name," ");
        leave_message[i].number = 0;
    }
    conn_len = sizeof(struct sockaddr_in);
    while(1)
    {
        if((conn_fd= accept(sock_fd,(struct sockaddr *)&conn_addr,&conn_len)) < 0) //等待连接状态
        {
            my_err("accept",__LINE__);
        }
        for(i = 0;i < 30;i++)
        {
            if(connect_info[i].fd == -1)
                break;
        }
        connect_info[i].fd = conn_fd;     //将conn_fd记录
        time(&now);
        printf("有新的连接：ip:%s\t%s\n",inet_ntoa(conn_addr.sin_addr),ctime(&now));
        int k = i;              //必须这样才能让传上去的i正确
        pthread_create(&thid,NULL,client_t,(void*)&k); //专门用来处理客户端除过登录申请账户之外的请求
    }
     return 0;
}
//一个用户一个线程

#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#define SERV_PORT        8888  //端口号
#define LISTENQ          30    //accept队列长度
#define BUFSIZE          1024  
#define LEN     sizeof(struct users)
#define LEN1    sizeof(struct message)
//#define BUFSIZE          LEN1 
int recv_t;


pthread_mutex_t          mutex;
int                      y=0;  //分配离线消息的结构体
struct user                    //用户名的结构体
{
    char username[10];
};

struct regi_sign               //登录注册用的结构体
{
    char flag;
    char username[10];
    char password[10];
};

struct group                   //群组的结构体
{
    char group_name[10];
    char admin_name[10];
    struct user member[10];
    int  member_num;
};

struct connect_info            //conn_fd登录结构体
{
    int fd;
    char name[10];
}connect_info[30];

struct File
{
    char file_name[30];
    char file_message[500];
    int file_len;
};

struct users                   //用户结构体
{
    struct user user;          //用户名
    char password[10];         //密码
    int  friend_num;           //朋友数
    int  group_num;            //群数
    struct user friend[10];    //朋友信息(朋友昵称)
    struct group group[10];    //群名
    struct users *next;
};

struct leave_message           //离线消息结构体
{
    char  name[10];
    int   number;
}leave_message[30];

struct users*  head = NULL;    //用户信息头指针
struct users*  p;              //指向当前用户的指针

struct message                 //消息结构体
{
    char      type;
    char      from[10];
    char      to[10];
    char      time[30];
    char      changefriname[10];//添加好友和删除
    struct    user name[30]; //附近的人，在线好友。查看好友
    struct    group group;
    char      news[500];
  //  struct File  file;
};

struct users* read_input();    //将信息读入链表
struct users* apply_account(struct regi_sign,int);  //申请账户
void save();                   //保存信息到链表
char * my_time();              //自定义时间函数
struct users* sign_in(struct regi_sign,int); //用户登录
void quit(struct regi_sign,int);             //用户未登录退出
void near_friend(struct message,int);        //附近的人
void view_friend(struct message,int);        //查看好友
void add_friend(struct message,int);         //添加好友
void out_line(struct message,int);           //下线
void view_online_friend(struct message,int); //查看在线好友
int del_friend(struct message,int);         //删除好友
int pri_chat(struct message,int);            //私聊
void build_group(struct message,int);        //群聊
void view_group(struct message,int);         //查看我的组
void public_chat(struct message,int);        //群聊
int accept_add_friend(struct message,int);
void leave_message_func(struct regi_sign,int);//离线消息
void apply_add_group(struct message,int);
void accept_add_group(struct message,int);
void refuse_add_group(struct message,int);
void my_status(struct message,int);
int my_zone_access(struct message,int);
void my_err(const char *err_string, int line);

int my_recv(int conn_fd, char* data_buf, int len, int flags)
{
    char recv_buf[BUFSIZE];
    char *pread;//指向下一次读取数据的位置
    int len_remin = 0;//自定义缓冲区剩余字数
    int i;

    // 如果自定义缓冲区中没有数据,则从套接字中读取数据
    if(len_remin <=0)
    {
        if((len_remin = recv(conn_fd, recv_buf, sizeof(recv_buf), flags)) <0)
        {
            my_err("recv",__LINE__);
        }
        else if(len_remin == 0)
        {
            return 0;
        }
        pread = recv_buf;//初始化pread指针
    }

    //从自定义缓冲区中读取一次数据
    for(i = 0; *pread != '\n'; i++)
    {
        if(i>len)//防止指针越界
        {
            return -1;
        }
        data_buf[i]=*pread++;
        len_remin--;
    }
    len_remin--;
    pread++;
    return i;
}

int my_zone_access(struct message chat,int i)
{
    struct message near;
    char   near_buf[BUFSIZE];
    int    k;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(near.from,connect_info[k].name))
            break;
    }
    if(near.type == '8')
    {
        memcpy(near_buf,&near,LEN1);
        send(connect_info[k].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    if(near.type == '9')
    {
        memcpy(near_buf,&near,LEN1);
        send(connect_info[k].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    if(near.type == '0')
    {
        memcpy(near_buf,&near,LEN1);
        send(connect_info[k].fd,near_buf,BUFSIZE,0);
        return 0;
    }
}
void my_err(const char* err_string, int line )
{
    fprintf(stderr, "line: %d", line);
    perror(err_string);
    exit(1);
}
void my_status(struct message chat,int i)
{
    struct message near;
    struct users *p1;
    char   near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    p1 = head->next;
    int k,j;

    while(p1)
    {
        if(!strcmp((p1->user).username,near.from))
        {
            for(k = 0;k < 10;k++)
            {
                for(j = 0;j < 30;j++)
                {
                    if(!strcmp((p1->friend)[k].username,connect_info[j].name))
                    {
                        memcpy(near_buf,&near,LEN1);
                        send(connect_info[j].fd,near_buf,BUFSIZE,0);
                        break;
                    }
                }
            }
        }
        p1 = p1->next;
    }
}
void refuse_add_group(struct message chat,int i)
{
    int k;
    struct message near;
    char near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    memset(near_buf,0,LEN1);
    near = chat;
    memcpy(near_buf,&near,LEN1);
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
    }
}
void accept_add_group(struct message chat,int i)
{
    int k,j,t,m,flag = 0;
    struct users *p1,*p2,*p3;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct user  name[10];
    struct message near;
    struct group group;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!strcmp(near.group.admin_name,(p1->user).username))   
        {
            for(k = 0;k < 10;k++)                    
            {
                if(!strcmp((p1->group)[k].group_name,near.to))  //找到指定群
                {
                    for(j = 0;j < 10 ;j++)
                    {
                        memcpy(&name[j].username,&(p1->group)[k].member[j].username,10);
                    }
                }
            }
        }
        p1 = p1->next;
    }
    for(k = 0;k < 10;k++)
    {
        p2 = head->next;
        while(p2)
        {
            if(!strcmp(name[k].username,(p2->user).username))
            {

                for(m = 0;m < 10;m++)                    
                {
                    if(!strcmp((p2->group)[m].group_name,near.to))//找到指定组
                    {
                        memcpy(&(p2->group)[m].member[(p2->group)[m].member_num],&near.from,10);
                        t = (p2->group)[m].member_num;
                        (p2->group)[m].member_num = (p2->group)[m].member_num + 1;
                        flag = 1;
                        break;
                    }
                }
                if(flag == 1)
                    break;
                
            }
            p2 = p2->next;
        }
    }
    memcpy(&group.group_name,&near.to,10);
    memcpy(&group.admin_name,&near.group.admin_name,10);
    for(k = 0;k < t;k++)
    {
        memcpy(&(group.member[k].username),&name[k].username,10);
    }
    memcpy(&(group.member[t]),&near.from,10);
    p3 = head->next;
    while(p3)
    {
        if(!(strcmp(near.from,(p3->user).username)))      //查找到每一个成员，将他们的组直接赋值
        {
            (p3->group)[p3->group_num] = group;
            (p3->group)[p3->group_num].member_num = t+1;
            (p3->group_num)++;
            break;
        }
        p3 = p3->next;
    }
    save();
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
void apply_add_group(struct message chat,int i)
{
    struct message near;
    struct users *p1;
    char near_buf[BUFSIZE];
    int k,flag = 0;
    char name[10];           //用来保存群主的名字
    p1 = head->next;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    memcpy(near_buf,&near,LEN1);
    while(p1)
    {
        for(k = 0;k < 10;k++)
        {
            if(!(strcmp((p1->group)[k].group_name,near.to)))
            {
                memcpy(name,(p1->group)[k].admin_name,10);
                memcpy(near.group.admin_name,name,10);
                flag = 1;
                break;
            }
        }
        if(flag == 1)
            break;
        p1 = p1->next;
    }
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,name))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
void leave_message_func(struct regi_sign account,int i) //离线消息
{
    struct message chat;
    char   near_buf[BUFSIZE];
    int k,j;
    FILE *fp;
    memset(&chat,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    for(k = 0;k < 30;k++)
    {
        if(!(strcmp(account.username,leave_message[k].name)))   //找到用户的离线结构体
        {
            if(leave_message[k].number != 0)                    //如果离线消息数不为0
            {
                if((fp = fopen(leave_message[k].name,"a+")) == NULL) //从文件中读取
                {
                    my_err("fopen",__LINE__);
                    exit(0);
                }
                while((fread(&chat,LEN1,1,fp) != -1) && (!feof(fp)))
                {
                    chat.type = 'x';
                    memcpy(near_buf,&chat,LEN1);
                    if(send(connect_info[i].fd,near_buf,BUFSIZE,0)!=BUFSIZE) //将消息逐条发过去
                    {
                        my_err("send",__LINE__);
                        pthread_exit(0);
                    }
                }
                fclose(fp);
                leave_message[k].number = 0;                    //将离线消息数至为0
                unlink(leave_message[k].name);                  //删除文件
                
            }
        }
    }

}
void public_chat(struct message chat ,int i)                     //群聊
{
    int k,j;                                  
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!strcmp(near.from,(p1->user).username))   //找到用户
        {
            for(k = 0;k < 10;k++)                    
            {
                if(!strcmp((p1->group)[k].group_name,near.to))  //找到指定组
                {
                    for(j = 0;j < 30;j++)
                    {
                        if((!strcmp(connect_info[j].name,(p1->group)[k].member[j].username)) && (connect_info[j].fd != connect_info[i].fd))
                        {
                            memcpy(near_buf,&near,LEN1);
                            if(send(connect_info[j].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                            {
                                my_err("send",__LINE__);
                                pthread_exit(0);
                            }
                        }

                    }
                }
            }
        }
        p1 = p1->next;
    }
}
void view_group(struct message chat,int i)             //查看我的组
{
    int k ;
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!strcmp(near.from,(p1->user).username))     //找到用户
        {
            for(k = 0;k < 10;k++)
            {
               if(!strcmp((p1->group)[k].group_name,near.to)) //找到组
                    near.group = (p1->group)[k];
            }
        }
        p1 = p1->next;
    }
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void build_group(struct message chat,int i)                  //建群
{
    int k ;
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    struct message chat1;
    struct user  name[10];
    memset(&near,0,LEN1);
    memset(&chat1,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    chat1 = chat;
    for(k = 0; k < 10; k++)
    {
        memcpy(name[k].username,near.group.member[k].username,10);   //先将成员保存到name[]数组里
    }
    while(p1)
    {
        for(k = 0;k < 10;k++)
        {
            if(!(strcmp(name[k].username,(p1->user).username)))      //查找到每一个成员，将他们的组直接赋值
            {
                (p1->group)[p1->group_num] = near.group;
                (p1->group_num)++;
            }
        }
        p1 = p1->next;
    }
    save();
    printf("[用户]“%s”创建了群 “%s”\t“%s”\n",near.from,near.group.group_name,my_time());
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)       //给群主通知群建立成功
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
    for(k = 0;k < 30;k++)
    {
        if((!strcmp(name[k].username,connect_info[k].name)) && (strcmp(name[k].username,near.from)))
        {
            chat1.type = 'r';
            memset(&near_buf,0,BUFSIZE);
            memcpy(near_buf,&chat1,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);             //给其他群成员通知有人邀请你加入群
        }
    }
}
int pri_chat(struct message chat,int i)                                //私聊
{
    struct users *p1;
    p1 = head->next;
    FILE *fp;
    int  k;
    struct  message  near;
    char    near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    if(!strcmp(near.to,near.from))
    {
        near.type = '6';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    while(p1)
    {
        if(!strcmp((p1->user).username,near.to))
        {
            for(k = 0;k < 30;k++)
            {
                if(!strcmp(near.to,connect_info[k].name))                      //如果在线直接发送
                {
                    near.type = 'o';
                    memcpy(near_buf,&near,LEN1);
                    if(send(connect_info[k].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                    {
                        my_err("send",__LINE__);
                        pthread_exit(0);
                    }
                    return 0;
                }
            }
            if(k == 30)
            {
                near.type = 'd';                                               //如果不在线就转为离线消息
                memcpy(near_buf,&near,LEN1);
                if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                {
                    my_err("send",__LINE__);
                    pthread_exit(0);
                }
                if((fp = fopen(near.to,"ab+")) == NULL)
                {
                    my_err("fopen",__LINE__);
                    exit(0);
                }
                fwrite(&near,LEN1,1,fp);
                fclose(fp);
                for(k = 0;k < 30;k++)
                {
                    if(!(strcmp(leave_message[k].name,near.to)))
                        leave_message[k].number = leave_message[k].number+1;
                }
                return 0;
            }
        }
        p1 = p1->next;
    }
    if(p1 == NULL)
    {
        near.type = '5';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
    }
}
int del_friend(struct message chat,int i)                     //删除好友
{ 
    struct  users *p1;
    p1 = head->next;
    int k;
    struct message near;
    memset(&near,0,LEN1);
    near = chat;
    char  near_buf[BUFSIZE];
    if(!strcmp(near.to,near.from))
    {
        near.type = '3';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from)))           //找到本人
        {
            for(k = 0;k < 10;k++)
            {
                if(!(strcmp(near.to,(p1->friend)[k].username)))
                {
                    for(k;k < 10;k++)
                    {
                        memcpy((p1->friend)[k].username,(p1->friend)[k+1].username,10);//将数组后的元素依次向前移动
                    }
                    p1->friend_num--;     //将好友总数减一
                    save();
                    printf("[用户]“%s”删除了好友“%s” \t“%s”\n",near.from,near.to,my_time());
                    memset(near_buf,0,BUFSIZE);
                    memcpy(near_buf,&near,LEN1);
                    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                    {
                        my_err("send",__LINE__);
                        pthread_exit(0);
                    }
                    return 0;
                }
            }
            
        }        
        p1 = p1->next;
    }
    if(p1 == NULL)
    {
        near.type = '1';
        memset(near_buf,0,BUFSIZE);
        memcpy(near_buf,&near,LEN1);
        if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
        {
            my_err("send",__LINE__);
            pthread_exit(0);
        }
    }
}

void view_online_friend(struct message chat,int i)             //查看在线好友
{ 
    struct  users *p1;                            
    p1 = head->next;
    int k,j;
    struct message near;
    struct user    name[10];
    memset(&name,0,sizeof(struct user));
    memset(&near,0,LEN1);
    near = chat;
    char  near_buf[BUFSIZE];
    near.type = 'z';
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from)))           //查找到那个人
        {
            for(k = 0;k < 10;k++)
                memcpy(name[k].username,(p1->friend)[k].username,10);//将好友全部保存到name[]数组里
            for(k = 0;k < 10;k++)
            {
                for(j = 0;j < 30;j++)
                {
                    if((!strcmp(name[k].username,connect_info[j].name)) && (connect_info[j].fd != -1))//依次检查各个好友的是否在线
                        {
                            memcpy(near.name[k].username,connect_info[j].name,10);        //再将在线的好友保存到name[]中
                        }
                }
            }
            
        }
        p1 = p1->next;
    }
    memset(near_buf,0,BUFSIZE);
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void out_line(struct message chat,int i)          //下线，结束自己的线程
{
    struct message near;
    memset(&near,0,LEN1);
    near = chat;
    connect_info[i].fd = -1;                       //将信息初始化
    strcpy(connect_info[i].name," ");
    printf("[用户]%s下线了 \t%s\n",near.from,my_time());
    pthread_exit(0);
}
void refuse_add(struct message chat,int i)         //拒绝添加
{
    int k ;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);                    //当被添加好友输入n时，发送标志为e的拒绝状态
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
int accept_add_friend(struct message chat,int i)   //第一次的添加消息的转发
{
    struct users *p1;
    p1 = head->next;
    int k;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    if(!strcmp(near.to,near.from))
    {
        near.type = '2';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from)))           //找到本人
        {
            for(k = 0;k < 10;k++)
            {
                if((!(strcmp(near.to,(p1->friend)[k].username))) && (strlen(near.to) != 0))
                {
                    near.type = '4';
                    memcpy(near_buf,&near,LEN1);
                    send(connect_info[i].fd,near_buf,BUFSIZE,0);
                    return 0;
                }
            }
        }
        p1 = p1->next;
    }
    for(k = 0;k < 30;k++)
    {
        if(!(strcmp(connect_info[k].name,near.to))) //找到被添加的人，然后向他发送添加请求
        {
            memcpy(near_buf,&near,LEN1);
            if(send(connect_info[k].fd,near_buf,BUFSIZE,0)!= BUFSIZE)
            {
                my_err("send",__LINE__);
                pthread_exit(0);
            }
            return 0;
        }
    }
    if(k == 30 || (strlen(near.to) == 0))                                   //未找到此人或者他不在线
    {
        near.type = 'm';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);  
    }
}
void add_friend(struct message chat,int i)        //同意添加
{
    int k;
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!(strcmp(near.from,(p1->user).username)))  //互相将对方添加到自己的朋友中
        {
            memcpy(((p1->friend)[p1->friend_num].username),near.to,10);
            p1->friend_num++;
        }
        if(!(strcmp(near.to,(p1->user).username)))
        {
            memcpy(((p1->friend)[p1->friend_num].username),near.from,10);
            p1->friend_num++;  //增加完之后让他的朋友数加1
        }
        p1 = p1->next;
        save();                                       //保存信息
    }
    printf("[用户]“%s”添加了“%s”为好友 \t%s\n",near.from,near.to,my_time());
    memcpy(near_buf,&near,LEN1);
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))   //给自己发送添加成功提示
        {
            if(send(connect_info[k].fd,near_buf,BUFSIZE,0) != BUFSIZE)
            {
                my_err("send",__LINE__);
                pthread_exit(0);
            }
        }
    }
}
void view_friend(struct message chat,int i)           //查看好友
{
    struct  users *p1;
    p1 = head->next;
    int k;
    struct message near;
    memset(&near,0,LEN1);
    near = chat;
    char  near_buf[BUFSIZE];
    near.type = 'v';                                  //查看自己的全部好友
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from))) //找到自己
        {
            for(k = 0;k < 10;k++)
                memcpy(near.name[k].username,(p1->friend)[k].username,10);//将好友保存到数组里
        }
        p1 = p1->next;
    }
    memset(near_buf,0,BUFSIZE);
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)   //将好友信息发送过去
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void near_friend(struct message chat,int i)                  //查看所有在线的人
{
    int  k;
    struct message near;
    char   near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(connect_info[k].fd != -1)                         //只要在线就输出
            memcpy(near.name[k].username,connect_info[k].name,10);
    }
    
    memset(near_buf,0,BUFSIZE);
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void quit(struct regi_sign account,int i)                  //在登录界面退出
{
    struct regi_sign quit;
    quit = account;
    connect_info[i].fd = -1;
    strcpy(connect_info[i].name," ");
    printf("客户端退出 \t%s\n",my_time());
    pthread_exit(0);
}
struct users* sign_in(struct regi_sign account,int i)
{
    int  k,m = 0;
    struct regi_sign  sign;
    struct users  *p1;
    p1 = head->next;
    sign = account;
    for(k = 0;k < 30; k++)                        //判断是否在线
    {
        if(!strcmp(connect_info[k].name,sign.username))
            m = m+1;
    }
   // printf("m:%d\n",m);
    if(m >= 1)                                    //如果已经在线就拒绝登录
    {
        send(connect_info[i].fd,"w",BUFSIZE,0);
        return NULL;
    }
    if(m == 0)                                     //如果不在线
    {
        while(p1)
        {
            if((!strcmp((p1->user).username,sign.username)) && (!strcmp(p1->password,sign.password))) //校验密码
            {
                if((send(connect_info[i].fd,"y",BUFSIZE,0)) != BUFSIZE)                               //密码正确就登录
                {
                    my_err("login send",__LINE__);
                }
                printf("[用户]%s上线了 \t%s\n",sign.username,my_time());
                memcpy(connect_info[i].name,sign.username,10);
                memcpy(leave_message[y++].name,sign.username,10);
            
                return p1;
            }
            p1 = p1->next; //判断未成功，让p1指向下一个
        }
        if(p1 == NULL)                                                                                //没找到，拒绝登录
        {
            send(connect_info[i].fd,"n",BUFSIZE,0);
            return NULL;
        }
    }
}
char * my_time()               //自定义的获取时间的函数
{
    time_t now;
    time(&now);
    return (ctime(&now));
}
void save()                   //将链表信息保存到文件
{
    FILE *fp;
    struct users *p1;
    p1 = head->next;
    if((fp = fopen("userlist","wb")) == NULL)
    {
        my_err("fopen",__LINE__);
        exit(0);
    }
    while(p1 != NULL)
    {
        fwrite(p1,LEN,1,fp);
        p1 = p1->next;
    }
    fclose(fp);

}
struct users* apply_account(struct regi_sign account,int i) //申请账户
{
    struct    regi_sign apply;
    struct    users   *p1,*p2;
    p1 = head;
    apply = account;
    while(p1->next != NULL)
    {
        if(strcmp((p1->next->user).username,apply.username) == 0) //判断是否有重名
        {
            send(connect_info[i].fd,"n",BUFSIZE,0);
            return NULL;
        }
        p1 = p1->next;
    }
    p2 = (struct users*)malloc(LEN);
    memset(p2,0,LEN);         //对刚malloc的结构体进行清空
    strcpy((p2->user).username,apply.username);                  //为新用户申请空间，并且将用户名和密码赋上
    strcpy(p2->password,apply.password);
    p1->next = p2;
    p2->next = NULL;
    save();
    send(connect_info[i].fd,"y",1024,0);
    memcpy(connect_info[i].name,apply.username,10);
    memcpy(leave_message[y++].name,apply.username,10);
    printf("[用户]%s注册成功 \t%s\n",apply.username,my_time());
    return (p2);
}
void *sign_quit()                                               //服务器退出函数
{
    char choice[20];
    do
    {
        printf("-------退出服务器(请输入quit)--------\n");
        scanf("%s",choice);
    }while(strcmp("quit",choice));
    if(!strcmp("quit",choice))
    {
        exit(0);
    }
}
struct users* read_input()                                      //将文件中的信息读到链表
{
    FILE *fp;
    struct users *p1,*p2;
    if((fp = fopen("userlist","rb")) == NULL)
    {
        fp = fopen("userlist","ab+");
    }
    p1 = (struct users *)malloc(LEN);
    p2 = (struct users *)malloc(LEN);
    head = p1;
    fread(p2,LEN,1,fp);
    while(!feof(fp))
    {
        p1->next = p2;
        p1 = p2;
        p2 = (struct users*)malloc(LEN);
        fread(p2,LEN,1,fp);
    }
    p1->next = NULL;
    free(p2);
    fclose(fp);
    return (head);
}

void file(struct message chat,int i)         //拒绝接收文件和同意接收文件
{
    int k ;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);                    
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
void file_send(struct message chat, int i)//发送消息请求
{
    int k ;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);                    
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.to))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}

void *client_t(void *arg)                                       //线程函数，专门用来处理客户端除过登录申请账户之外的请求
{
    int i = *(int *)arg;
    char      recv_buf[BUFSIZE];
    while(1)
    {
        struct    regi_sign account;
        memset(&account,0,sizeof(struct regi_sign));
        memset(recv_buf,0,sizeof(recv_buf));
        if(recv(connect_info[i].fd,recv_buf,sizeof(recv_buf),0) != BUFSIZE) 
       // if((recv_t = my_recv(connect_info[i].fd,recv_buf,sizeof(recv_buf),0))!=BUFSIZE)        
        {
            connect_info[i].fd = -1;
            printf("[用户]%s异常离线 \t%s\n",connect_info[i].name,my_time());
            strcpy(connect_info[i].name," ");
            pthread_exit(0);
        }
        pthread_mutex_lock(&mutex);
        memcpy(&account,recv_buf,sizeof(recv_buf));
        head = read_input(); //保存头结点（读取旧文件 以往的用户等）
        if(account.flag == 'r')  //注册
        {
            p = apply_account(account,i);//p是指向当前
            pthread_mutex_unlock(&mutex);
            leave_message_func(account,i);  //
            if(p != NULL)
                break;
        }
        if(account.flag == 'l')  //登录
        {
            p = sign_in(account,i);
            pthread_mutex_unlock(&mutex);
            leave_message_func(account,i);
            if(p != NULL)
                break;
        }
        if(account.flag == 'q')  //退出
        {
            pthread_mutex_unlock(&mutex);
            quit(account,i);
        }
    }
    while(1)
    {
        struct    message  chat;
        char      recv_buf[BUFSIZE];
        int       ret;
        memset(&chat,0,LEN1);
        memset(recv_buf,0,BUFSIZE);
        if((ret = recv(connect_info[i].fd,recv_buf,BUFSIZE,0))!= BUFSIZE)
       // if((recv_t = my_recv(connect_info[i].fd,recv_buf,sizeof(recv_buf),0))!=BUFSIZE)
        {
         //   printf("ret : %d\n",ret);
         //   printf("%s\n",recv_buf);
            connect_info[i].fd = -1;
            printf("[用户]%s异常离线 \t%s\n",connect_info[i].name,my_time());
            strcpy(connect_info[i].name," ");
            pthread_exit(0);
        }
        memcpy(&chat,recv_buf,BUFSIZE);
        //printf("this is chat.type:%c\n",chat.type);
        switch(chat.type)
        {
            case 'f':   //查看附近的人
            {
                near_friend(chat,i);
                break;
            }
            case 'v':   //查看所有好友
            {
                view_friend(chat,i);
                break;
            }
            case 't':   //添加好友
            {
                add_friend(chat,i);
                break;
            }
            case 'g':   //下线
            {
                out_line(chat,i);
                break;
            }
            case 'z':   //在线好友
            {
                view_online_friend(chat,i);
                break;
            }
            case 's':   //删除好友
            {
                del_friend(chat,i);   
                break;
            }
            case 'o':   //私聊
            {
                pri_chat(chat,i);
                break;
            }
            case 'j':   //建群
            {
                build_group(chat,i);
                break;
            }
            case 'c':   //查看群信息
            {
                view_group(chat,i);
                break;
            }
            case 'b':   //群聊
            {
                public_chat(chat,i);
                break;
            }
            case 'y':   //接受添加好友
            {
                accept_add_friend(chat,i);
                break;
            }
            case 'e':   //拒绝添加
            {
                refuse_add(chat,i);
                break;
            }
            case 'h':   //申请入群
            {
                apply_add_group(chat,i);
                break;
            }
            case 'a':   //同意入群
            {
                accept_add_group(chat,i);
                break;
            }
            case 'i':
            {
                refuse_add_group(chat,i);
                break;
            }
            case '7':
            {
                my_status(chat,i);
                break;
            }
            case '8':
            {
                my_zone_access(chat,i);
                break;
            }
            case '9':
            {
                my_zone_access(chat,i);
                break;
            }
            case '0':
            {
                my_zone_access(chat,i);
                break;
            }
            case 'p': file_send(chat,i); break;
            case '&': file_send(chat,i); break;
            case '$': file_send(chat,i); break;
            case 'u': file(chat,i); break;
            case 'w': file(chat,i); break;


        }
    }
}
int main(int argc,char *argv[])
{
    int                          sock_fd,conn_fd;
    struct     sockaddr_in       serv_addr,conn_addr;
    socklen_t                    conn_len;
    pid_t                        pid;
    char                         recv_buf[128];
    int                          ret,i;
    int                          optval;
    time_t                       now;
    pthread_t                    thid,quit_thid;
    int                          status;

    pthread_mutex_init(&mutex,NULL);
    if((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0) //创建套接字
    {
        my_err("socket",__LINE__);
    }
    optval = 1;
    
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void*)&optval,sizeof(int)) < 0) //设定端口，要是异常退出還可以在绑定
    {
        my_err("setsockopt",__LINE__);
    }

    memset(&serv_addr,0,sizeof(struct sockaddr_in));                   //初始化服务器端端口，IP信息
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    pthread_create(&quit_thid,NULL,(void*)sign_quit,NULL);
    if(bind(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in)) < 0)//将IP与端口绑定在一起
    {
        my_err("bind",__LINE__);
    }
    if(listen(sock_fd,LISTENQ) < 0)                                    //将套接字转化为接听套接字
    {
        my_err("listen",__LINE__);
    }
    for(i = 0;i < 30;i++)                                             //初始化离线消息和conn_fd结构体信息
    {
        connect_info[i].fd = -1;
        strcpy(connect_info[i].name," ");
        strcpy(leave_message[i].name," ");
        leave_message[i].number = 0;
    }
    conn_len = sizeof(struct sockaddr_in);
    while(1)
    {
        if((conn_fd= accept(sock_fd,(struct sockaddr *)&conn_addr,&conn_len)) < 0) //等待连接状态
        {
            my_err("accept",__LINE__);
        }
        for(i = 0;i < 30;i++)
        {
            if(connect_info[i].fd == -1)
                break;
        }
        connect_info[i].fd = conn_fd;     //将conn_fd记录
        time(&now);
        printf("有新的连接：ip:%s\t%s\n",inet_ntoa(conn_addr.sin_addr),ctime(&now));
        int k = i;              //必须这样才能让传上去的i正确
        pthread_create(&thid,NULL,client_t,(void*)&k); //专门用来处理客户端除过登录申请账户之外的请求
    }
     return 0;
}*/
/*
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#define SERV_PORT        8888  //端口号
#define LISTENQ          30    //accept队列长度
#define BUFSIZE          1024  
#define LEN     sizeof(struct users)
#define LEN1    sizeof(struct message)

pthread_mutex_t          mutex;
int                      y=0;  //分配离线消息的结构体
struct user                    //用户名的结构体
{
    char username[10];
};
struct regi_sign               //登录注册用的结构体
{
    char flag;
    char username[10];
    char password[10];
};
struct group                   //群组的结构体
{
    char group_name[10];
    char admin_name[10];
    struct user member[10];
    int  member_num;
};
struct connect_info            //conn_fd登录结构体
{
    int fd;
    char name[10];
}connect_info[30];

struct users                   //用户结构体
{
    struct user user;          //用户名
    char password[10];         //密码
    int  friend_num;           //朋友数
    int  group_num;            //群数
    struct user friend[10];    //朋友信息
    struct group group[10];    //群
    struct users *next;
};
struct leave_message           //离线消息结构体
{
    char  name[10];
    int   number;
}leave_message[30];

struct users*  head = NULL;    //用户信息头指针
struct users*  p;              //指向当前用户的指针

struct message                 //消息结构体
{
    char      type;
    char      from[10];
    char      to[10];
    char      time[30];
    char      changefriname[10];//添加好友和删除
    struct    user name[30]; //附近的人，在线好友。查看好友
    struct    group group;
    char      news[500];
};
struct users* read_input();    //将信息读入链表
struct users* apply_account(struct regi_sign,int);  //申请账户
void save();                   //保存信息到链表
char * my_time();              //自定义时间函数
struct users* sign_in(struct regi_sign,int); //用户登录
void quit(struct regi_sign,int);             //用户未登录退出
void near_friend(struct message,int);        //附近的人
void view_friend(struct message,int);        //查看好友
void add_friend(struct message,int);         //添加好友
void out_line(struct message,int);           //下线
void view_online_friend(struct message,int); //查看在线好友
int del_friend(struct message,int);         //删除好友
int pri_chat(struct message,int);            //私聊
void build_group(struct message,int);        //群聊
void view_group(struct message,int);         //查看我的组
void public_chat(struct message,int);        //群聊
int accept_add_friend(struct message,int);
void leave_message_func(struct regi_sign,int);//离线消息
void apply_add_group(struct message,int);
void accept_add_group(struct message,int);
void refuse_add_group(struct message,int);
void my_status(struct message,int);
int my_zone_access(struct message,int);
void my_err(const char *err_string, int line);

int my_zone_access(struct message chat,int i)
{
    struct message near;
    char   near_buf[BUFSIZE];
    int    k;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(near.from,connect_info[k].name))
            break;
    }
    if(near.type == '8')
    {
        memcpy(near_buf,&near,LEN1);
        send(connect_info[k].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    if(near.type == '9')
    {
        memcpy(near_buf,&near,LEN1);
        send(connect_info[k].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    if(near.type == '0')
    {
        memcpy(near_buf,&near,LEN1);
        send(connect_info[k].fd,near_buf,BUFSIZE,0);
        return 0;
    }
}
void my_err(const char* err_string, int line )
{
    fprintf(stderr, "line: %d", line);
    perror(err_string);
    exit(1);
}
void my_status(struct message chat,int i)
{
    struct message near;
    struct users *p1;
    char   near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    p1 = head->next;
    int k,j;

    while(p1)
    {
        if(!strcmp((p1->user).username,near.from))
        {
            for(k = 0;k < 10;k++)
            {
                for(j = 0;j < 30;j++)
                {
                    if(!strcmp((p1->friend)[k].username,connect_info[j].name))
                    {
                        memcpy(near_buf,&near,LEN1);
                        send(connect_info[j].fd,near_buf,BUFSIZE,0);
                        break;
                    }
                }
            }
        }
        p1 = p1->next;
    }
}
void refuse_add_group(struct message chat,int i)
{
    int k;
    struct message near;
    char near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    memset(near_buf,0,LEN1);
    near = chat;
    memcpy(near_buf,&near,LEN1);
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
    }
}
void accept_add_group(struct message chat,int i)
{
    int k,j,t,m,flag = 0;
    struct users *p1,*p2,*p3;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct user  name[10];
    struct message near;
    struct group group;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!strcmp(near.group.admin_name,(p1->user).username))   
        {
            for(k = 0;k < 10;k++)                    
            {
                if(!strcmp((p1->group)[k].group_name,near.to))  //找到指定群
                {
                    for(j = 0;j < 10 ;j++)
                    {
                        memcpy(&name[j].username,&(p1->group)[k].member[j].username,10);
                    }
                }
            }
        }
        p1 = p1->next;
    }
    for(k = 0;k < 10;k++)
    {
        p2 = head->next;
        while(p2)
        {
            if(!strcmp(name[k].username,(p2->user).username))
            {

                for(m = 0;m < 10;m++)                    
                {
                    if(!strcmp((p2->group)[m].group_name,near.to))//找到指定组
                    {
                        memcpy(&(p2->group)[m].member[(p2->group)[m].member_num],&near.from,10);
                        t = (p2->group)[m].member_num;
                        (p2->group)[m].member_num = (p2->group)[m].member_num + 1;
                        flag = 1;
                        break;
                    }
                }
                if(flag == 1)
                    break;
                
            }
            p2 = p2->next;
        }
    }
    memcpy(&group.group_name,&near.to,10);
    memcpy(&group.admin_name,&near.group.admin_name,10);
    for(k = 0;k < t;k++)
    {
        memcpy(&(group.member[k].username),&name[k].username,10);
    }
    memcpy(&(group.member[t]),&near.from,10);
    p3 = head->next;
    while(p3)
    {
        if(!(strcmp(near.from,(p3->user).username)))      //查找到每一个成员，将他们的组直接赋值
        {
            (p3->group)[p3->group_num] = group;
            (p3->group)[p3->group_num].member_num = t+1;
            (p3->group_num)++;
            break;
        }
        p3 = p3->next;
    }
    save();
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
void apply_add_group(struct message chat,int i)
{
    struct message near;
    struct users *p1;
    char near_buf[BUFSIZE];
    int k,flag = 0;
    char name[10];           //用来保存群主的名字
    p1 = head->next;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    memcpy(near_buf,&near,LEN1);
    while(p1)
    {
        for(k = 0;k < 10;k++)
        {
            if(!(strcmp((p1->group)[k].group_name,near.to)))
            {
                memcpy(name,(p1->group)[k].admin_name,10);
                memcpy(near.group.admin_name,name,10);
                flag = 1;
                break;
            }
        }
        if(flag == 1)
            break;
        p1 = p1->next;
    }
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,name))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
void leave_message_func(struct regi_sign account,int i) //离线消息
{
    struct message chat;
    char   near_buf[BUFSIZE];
    int k,j;
    FILE *fp;
    memset(&chat,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    for(k = 0;k < 30;k++)
    {
        if(!(strcmp(account.username,leave_message[k].name)))   //找到用户的离线结构体
        {
            if(leave_message[k].number != 0)                    //如果离线消息数不为0
            {
                if((fp = fopen(leave_message[k].name,"a+")) == NULL) //从文件中读取
                {
                    my_err("fopen",__LINE__);
                    exit(0);
                }
                while((fread(&chat,LEN1,1,fp) != -1) && (!feof(fp)))
                {
                    chat.type = 'x';
                    memcpy(near_buf,&chat,LEN1);
                    if(send(connect_info[i].fd,near_buf,BUFSIZE,0)!=BUFSIZE) //将消息逐条发过去
                    {
                        my_err("send",__LINE__);
                        pthread_exit(0);
                    }
                }
                fclose(fp);
                leave_message[k].number = 0;                    //将离线消息数至为0
                unlink(leave_message[k].name);                  //删除文件
                
            }
        }
    }

}
void public_chat(struct message chat ,int i)                     //群聊
{
    int k,j;                                  
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!strcmp(near.from,(p1->user).username))   //找到用户
        {
            for(k = 0;k < 10;k++)                    
            {
                if(!strcmp((p1->group)[k].group_name,near.to))  //找到指定组
                {
                    for(j = 0;j < 30;j++)
                    {
                        if((!strcmp(connect_info[j].name,(p1->group)[k].member[j].username)) && (connect_info[j].fd != connect_info[i].fd))
                        {
                            memcpy(near_buf,&near,LEN1);
                            if(send(connect_info[j].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                            {
                                my_err("send",__LINE__);
                                pthread_exit(0);
                            }
                        }

                    }
                }
            }
        }
        p1 = p1->next;
    }
}
void view_group(struct message chat,int i)             //查看我的组
{
    int k ;
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!strcmp(near.from,(p1->user).username))     //找到用户
        {
            for(k = 0;k < 10;k++)
            {
               if(!strcmp((p1->group)[k].group_name,near.to)) //找到组
                    near.group = (p1->group)[k];
            }
        }
        p1 = p1->next;
    }
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void build_group(struct message chat,int i)                  //建群
{
    int k ;
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    struct message chat1;
    struct user  name[10];
    memset(&near,0,LEN1);
    memset(&chat1,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    chat1 = chat;
    for(k = 0; k < 10; k++)
    {
        memcpy(name[k].username,near.group.member[k].username,10);   //先将成员保存到name[]数组里
    }
    while(p1)
    {
        for(k = 0;k < 10;k++)
        {
            if(!(strcmp(name[k].username,(p1->user).username)))      //查找到每一个成员，将他们的组直接赋值
            {
                (p1->group)[p1->group_num] = near.group;
                (p1->group_num)++;
            }
        }
        p1 = p1->next;
    }
    save();
    printf("[用户]“%s”创建了群 “%s”\t“%s”\n",near.from,near.group.group_name,my_time());
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)       //给群主通知群建立成功
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
    for(k = 0;k < 30;k++)
    {
        if((!strcmp(name[k].username,connect_info[k].name)) && (strcmp(name[k].username,near.from)))
        {
            chat1.type = 'r';
            memset(&near_buf,0,BUFSIZE);
            memcpy(near_buf,&chat1,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);             //给其他群成员通知有人邀请你加入群
        }
    }
}
int pri_chat(struct message chat,int i)                                //私聊
{
    struct users *p1;
    p1 = head->next;
    FILE *fp;
    int  k;
    struct  message  near;
    char    near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    if(!strcmp(near.to,near.from))
    {
        near.type = '6';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    while(p1)
    {
        if(!strcmp((p1->user).username,near.to))
        {
            for(k = 0;k < 30;k++)
            {
                if(!strcmp(near.to,connect_info[k].name))                      //如果在线直接发送
                {
                    near.type = 'o';
                    memcpy(near_buf,&near,LEN1);
                    if(send(connect_info[k].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                    {
                        my_err("send",__LINE__);
                        pthread_exit(0);
                    }
                    return 0;
                }
            }
            if(k == 30)
            {
                near.type = 'd';                                               //如果不在线就转为离线消息
                memcpy(near_buf,&near,LEN1);
                if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                {
                    my_err("send",__LINE__);
                    pthread_exit(0);
                }
                if((fp = fopen(near.to,"ab+")) == NULL)
                {
                    my_err("fopen",__LINE__);
                    exit(0);
                }
                fwrite(&near,LEN1,1,fp);
                fclose(fp);
                for(k = 0;k < 30;k++)
                {
                    if(!(strcmp(leave_message[k].name,near.to)))
                        leave_message[k].number = leave_message[k].number+1;
                }
                return 0;
            }
        }
        p1 = p1->next;
    }
    if(p1 == NULL)
    {
        near.type = '5';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
    }
}
int del_friend(struct message chat,int i)                     //删除好友
{ 
    struct  users *p1;
    p1 = head->next;
    int k;
    struct message near;
    memset(&near,0,LEN1);
    near = chat;
    char  near_buf[BUFSIZE];
    if(!strcmp(near.to,near.from))
    {
        near.type = '3';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from)))           //找到本人
        {
            for(k = 0;k < 10;k++)
            {
                if(!(strcmp(near.to,(p1->friend)[k].username)))
                {
                    for(k;k < 10;k++)
                    {
                        memcpy((p1->friend)[k].username,(p1->friend)[k+1].username,10);//将数组后的元素依次向前移动
                    }
                    p1->friend_num--;     //将好友总数减一
                    save();
                    printf("[用户]“%s”删除了好友“%s” \t“%s”\n",near.from,near.to,my_time());
                    memset(near_buf,0,BUFSIZE);
                    memcpy(near_buf,&near,LEN1);
                    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                    {
                        my_err("send",__LINE__);
                        pthread_exit(0);
                    }
                    return 0;
                }
            }
            
        }        
        p1 = p1->next;
    }
    if(p1 == NULL)
    {
        near.type = '1';
        memset(near_buf,0,BUFSIZE);
        memcpy(near_buf,&near,LEN1);
        if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
        {
            my_err("send",__LINE__);
            pthread_exit(0);
        }
    }
}

void view_online_friend(struct message chat,int i)             //查看在线好友
{ 
    struct  users *p1;                            
    p1 = head->next;
    int k,j;
    struct message near;
    struct user    name[10];
    memset(&name,0,sizeof(struct user));
    memset(&near,0,LEN1);
    near = chat;
    char  near_buf[BUFSIZE];
    near.type = 'z';
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from)))           //查找到那个人
        {
            for(k = 0;k < 10;k++)
                memcpy(name[k].username,(p1->friend)[k].username,10);//将好友全部保存到name[]数组里
            for(k = 0;k < 10;k++)
            {
                for(j = 0;j < 30;j++)
                {
                    if((!strcmp(name[k].username,connect_info[j].name)) && (connect_info[j].fd != -1))//依次检查各个好友的是否在线
                        {
                            memcpy(near.name[k].username,connect_info[j].name,10);        //再将在线的好友保存到name[]中
                        }
                }
            }
            
        }
        p1 = p1->next;
    }
    memset(near_buf,0,BUFSIZE);
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void out_line(struct message chat,int i)          //下线，结束自己的线程
{
    struct message near;
    memset(&near,0,LEN1);
    near = chat;
    connect_info[i].fd = -1;                       //将信息初始化
    strcpy(connect_info[i].name," ");
    printf("[用户]%s下线了 \t%s\n",near.from,my_time());
    pthread_exit(0);
}
void refuse_add(struct message chat,int i)         //拒绝添加
{
    int k ;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);                    //当被添加好友输入n时，发送标志为e的拒绝状态
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
int accept_add_friend(struct message chat,int i)   //第一次的添加消息的转发
{
    struct users *p1;
    p1 = head->next;
    int k;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    if(!strcmp(near.to,near.from))
    {
        near.type = '2';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from)))           //找到本人
        {
            for(k = 0;k < 10;k++)
            {
                if((!(strcmp(near.to,(p1->friend)[k].username))) && (strlen(near.to) != 0))
                {
                    near.type = '4';
                    memcpy(near_buf,&near,LEN1);
                    send(connect_info[i].fd,near_buf,BUFSIZE,0);
                    return 0;
                }
            }
        }
        p1 = p1->next;
    }
    for(k = 0;k < 30;k++)
    {
        if(!(strcmp(connect_info[k].name,near.to))) //找到被添加的人，然后向他发送添加请求
        {
            memcpy(near_buf,&near,LEN1);
            if(send(connect_info[k].fd,near_buf,BUFSIZE,0)!= BUFSIZE)
            {
                my_err("send",__LINE__);
                pthread_exit(0);
            }
            return 0;
        }
    }
    if(k == 30 || (strlen(near.to) == 0))                                   //未找到此人或者他不在线
    {
        near.type = 'm';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);  
    }
}
void add_friend(struct message chat,int i)        //同意添加
{
    int k;
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!(strcmp(near.from,(p1->user).username)))  //互相将对方添加到自己的朋友中
        {
            memcpy(((p1->friend)[p1->friend_num].username),near.to,10);
            p1->friend_num++;
        }
        if(!(strcmp(near.to,(p1->user).username)))
        {
            memcpy(((p1->friend)[p1->friend_num].username),near.from,10);
            p1->friend_num++;  //增加完之后让他的朋友数加1
        }
        p1 = p1->next;
        save();                                       //保存信息
    }
    printf("[用户]“%s”添加了“%s”为好友 \t%s\n",near.from,near.to,my_time());
    memcpy(near_buf,&near,LEN1);
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))   //给自己发送添加成功提示
        {
            if(send(connect_info[k].fd,near_buf,BUFSIZE,0) != BUFSIZE)
            {
                my_err("send",__LINE__);
                pthread_exit(0);
            }
        }
    }
}
void view_friend(struct message chat,int i)           //查看好友
{
    struct  users *p1;
    p1 = head->next;
    int k;
    struct message near;
    memset(&near,0,LEN1);
    near = chat;
    char  near_buf[BUFSIZE];
    near.type = 'v';                                  //查看自己的全部好友
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from))) //找到自己
        {
            for(k = 0;k < 10;k++)
                memcpy(near.name[k].username,(p1->friend)[k].username,10);//将好友保存到数组里
        }
        p1 = p1->next;
    }
    memset(near_buf,0,BUFSIZE);
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)   //将好友信息发送过去
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void near_friend(struct message chat,int i)                  //查看所有在线的人
{
    int  k;
    struct message near;
    char   near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(connect_info[k].fd != -1)                         //只要在线就输出
            memcpy(near.name[k].username,connect_info[k].name,10);
    }
    
    memset(near_buf,0,BUFSIZE);
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void quit(struct regi_sign account,int i)                  //在登录界面退出
{
    struct regi_sign quit;
    quit = account;
    connect_info[i].fd = -1;
    strcpy(connect_info[i].name," ");
    printf("客户端退出 \t%s\n",my_time());
    pthread_exit(0);
}
struct users* sign_in(struct regi_sign account,int i)
{
    int  k,m = 0;
    struct regi_sign  sign;
    struct users  *p1;
    p1 = head->next;
    sign = account;
    for(k = 0;k < 30; k++)                        //判断是否在线
    {
        if(!strcmp(connect_info[k].name,sign.username))
            m = m+1;
    }
   // printf("m:%d\n",m);
    if(m >= 1)                                    //如果已经在线就拒绝登录
    {
        send(connect_info[i].fd,"w",BUFSIZE,0);
        return NULL;
    }
    if(m == 0)                                     //如果不在线
    {
        while(p1)
        {
            if((!strcmp((p1->user).username,sign.username)) && (!strcmp(p1->password,sign.password))) //校验密码
            {
                if((send(connect_info[i].fd,"y",BUFSIZE,0)) != BUFSIZE)                               //密码正确就登录
                {
                    my_err("login send",__LINE__);
                }
                printf("[用户]%s上线了 \t%s\n",sign.username,my_time());
                memcpy(connect_info[i].name,sign.username,10);
                memcpy(leave_message[y++].name,sign.username,10);
            
                return p1;
            }
            p1 = p1->next; //判断未成功，让p1指向下一个
        }
        if(p1 == NULL)                                                                                //没找到，拒绝登录
        {
            send(connect_info[i].fd,"n",BUFSIZE,0);
            return NULL;
        }
    }
}
char * my_time()               //自定义的获取时间的函数
{
    time_t now;
    time(&now);
    return (ctime(&now));
}
void save()                   //将链表信息保存到文件
{
    FILE *fp;
    struct users *p1;
    p1 = head->next;
    if((fp = fopen("userlist","wb")) == NULL)
    {
        my_err("fopen",__LINE__);
        exit(0);
    }
    while(p1 != NULL)
    {
        fwrite(p1,LEN,1,fp);
        p1 = p1->next;
    }
    fclose(fp);

}
struct users* apply_account(struct regi_sign account,int i) //申请账户
{
    struct    regi_sign apply;
    struct    users   *p1,*p2;
    p1 = head;
    apply = account;
    while(p1->next != NULL)
    {
        if(strcmp((p1->next->user).username,apply.username) == 0) //判断是否有重名
        {
            send(connect_info[i].fd,"n",BUFSIZE,0);
            return NULL;
        }
        p1 = p1->next;
    }
    p2 = (struct users*)malloc(LEN);
    memset(p2,0,LEN);         //对刚malloc的结构体进行清空
    strcpy((p2->user).username,apply.username);                  //为新用户申请空间，并且将用户名和密码赋上
    strcpy(p2->password,apply.password);
    p1->next = p2;
    p2->next = NULL;
    save();
    send(connect_info[i].fd,"y",1024,0);
    memcpy(connect_info[i].name,apply.username,10);
    memcpy(leave_message[y++].name,apply.username,10);
    printf("[用户]%s注册成功 \t%s\n",apply.username,my_time());
    return (p2);
}
void *sign_quit()                                               //服务器退出函数
{
    char choice[20];
    do
    {
        printf("-------退出服务器(请输入quit)--------\n");
        scanf("%s",choice);
    }while(strcmp("quit",choice));
    if(!strcmp("quit",choice))
    {
        exit(0);
    }
}
struct users* read_input()                                      //将文件中的信息读到链表
{
    FILE *fp;
    struct users *p1,*p2;
    if((fp = fopen("userlist","rb")) == NULL)
    {
        fp = fopen("userlist","ab+");
    }
    p1 = (struct users *)malloc(LEN);
    p2 = (struct users *)malloc(LEN);
    head = p1;
    fread(p2,LEN,1,fp);
    while(!feof(fp))
    {
        p1->next = p2;
        p1 = p2;
        p2 = (struct users*)malloc(LEN);
        fread(p2,LEN,1,fp);
    }
    p1->next = NULL;
    free(p2);
    fclose(fp);
    return (head);
}
void *client_t(void *arg)                                       //线程函数，专门用来处理客户端除过登录申请账户之外的请求
{
    int i = *(int *)arg;
    char      recv_buf[BUFSIZE];
    while(1)
    {
        struct    regi_sign account;
        memset(&account,0,sizeof(struct regi_sign));
        memset(recv_buf,0,sizeof(recv_buf));
        if(recv(connect_info[i].fd,recv_buf,sizeof(recv_buf),0) != BUFSIZE)
        {
            connect_info[i].fd = -1;
            printf("[用户]%s异常离线 \t%s\n",connect_info[i].name,my_time());
            strcpy(connect_info[i].name," ");
            pthread_exit(0);
        }
        pthread_mutex_lock(&mutex);
        memcpy(&account,recv_buf,sizeof(recv_buf));
        head = read_input(); //保存头结点
        if(account.flag == 'r')  //注册
        {
            p = apply_account(account,i);
            pthread_mutex_unlock(&mutex);
            leave_message_func(account,i);
            if(p != NULL)
                break;
        }
        if(account.flag == 'l')  //登录
        {
            p = sign_in(account,i);
            pthread_mutex_unlock(&mutex);
            leave_message_func(account,i);
            if(p != NULL)
                break;
        }
        if(account.flag == 'q')  //退出
        {
            pthread_mutex_unlock(&mutex);
            quit(account,i);
        }
    }
    while(1)
    {
        struct    message  chat;
        char      recv_buf[BUFSIZE];
        int       ret;
        memset(&chat,0,LEN1);
        memset(recv_buf,0,BUFSIZE);
        if((ret = recv(connect_info[i].fd,recv_buf,BUFSIZE,0))!= BUFSIZE)
        {
         //   printf("ret : %d\n",ret);
         //   printf("%s\n",recv_buf);
            connect_info[i].fd = -1;
            printf("[用户]%s异常离线 \t%s\n",connect_info[i].name,my_time());
            strcpy(connect_info[i].name," ");
            pthread_exit(0);
        }
        memcpy(&chat,recv_buf,BUFSIZE);
        //printf("this is chat.type:%c\n",chat.type);
        switch(chat.type)
        {
            case 'f':   //查看附近的人
            {
                near_friend(chat,i);
                break;
            }
            case 'v':   //查看所有好友
            {
                view_friend(chat,i);
                break;
            }
            case 't':   //添加好友
            {
                add_friend(chat,i);
                break;
            }
            case 'g':   //下线
            {
                out_line(chat,i);
                break;
            }
            case 'z':   //在线好友
            {
                view_online_friend(chat,i);
                break;
            }
            case 's':   //删除好友
            {
                del_friend(chat,i);   
                break;
            }
            case 'o':   //私聊
            {
                pri_chat(chat,i);
                break;
            }
            case 'j':   //建群
            {
                build_group(chat,i);
                break;
            }
            case 'c':   //查看群信息
            {
                view_group(chat,i);
                break;
            }
            case 'b':   //群聊
            {
                public_chat(chat,i);
                break;
            }
            case 'y':   //接受添加好友
            {
                accept_add_friend(chat,i);
                break;
            }
            case 'e':   //拒绝添加
            {
                refuse_add(chat,i);
                break;
            }
            case 'h':   //申请入群
            {
                apply_add_group(chat,i);
                break;
            }
            case 'a':   //同意入群
            {
                accept_add_group(chat,i);
                break;
            }
            case 'i':
            {
                refuse_add_group(chat,i);
                break;
            }
            case '7':
            {
                my_status(chat,i);
                break;
            }
            case '8':
            {
                my_zone_access(chat,i);
                break;
            }
            case '9':
            {
                my_zone_access(chat,i);
                break;
            }
            case '0':
            {
                my_zone_access(chat,i);
                break;
            }
        }
    }
}
int main(int argc,char *argv[])
{
    int                          sock_fd,conn_fd;
    struct     sockaddr_in       serv_addr,conn_addr;
    socklen_t                    conn_len;
    pid_t                        pid;
    char                         recv_buf[128];
    int                          ret,i;
    int                          optval;
    time_t                       now;
    pthread_t                    thid,quit_thid;
    int                          status;
    pthread_mutex_init(&mutex,NULL);
    if((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0) //创建套接字
    {
        my_err("socket",__LINE__);
    }
    optval = 1;
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void*)&optval,sizeof(int)) < 0) //设定端口，要是异常退出還可以在绑定
    {
        my_err("setsockopt",__LINE__);
    }

    memset(&serv_addr,0,sizeof(struct sockaddr_in));                   //初始化服务器端端口，IP信息
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    pthread_create(&quit_thid,NULL,(void*)sign_quit,NULL);
    if(bind(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in)) < 0)//将IP与端口绑定在一起
    {
        my_err("bind",__LINE__);
    }
    if(listen(sock_fd,LISTENQ) < 0)                                    //将套接字转化为接听套接字
    {
        my_err("listen",__LINE__);
    }
    for(i = 0;i < 30;i++)                                             //初始化离线消息和conn_fd结构体信息
    {
        connect_info[i].fd = -1;
        strcpy(connect_info[i].name," ");
        strcpy(leave_message[i].name," ");
        leave_message[i].number = 0;
    }
    conn_len = sizeof(struct sockaddr_in);
    while(1)
    {
        if((conn_fd= accept(sock_fd,(struct sockaddr *)&conn_addr,&conn_len)) < 0) //等待连接状态
        {
            my_err("accept",__LINE__);
        }
        for(i = 0;i < 30;i++)
        {
            if(connect_info[i].fd == -1)
                break;
        }
        connect_info[i].fd = conn_fd;     //将conn_fd记录
        time(&now);
        printf("有新的连接：ip:%s\t%s\n",inet_ntoa(conn_addr.sin_addr),ctime(&now));
        int f = i;              //必须这样才能让传上去的i正确
        pthread_create(&thid,NULL,client_t,(void*)&f); //专门用来处理客户端除过登录申请账户之外的请求
    }
     return 0;
}*/

#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include"my_recv.c"
#define SERV_PORT        8888  //端口号
#define LISTENQ          30    //accept队列长度
#define BUFSIZE          1024  
#define LEN     sizeof(struct users)
#define LEN1    sizeof(struct message)

pthread_mutex_t          mutex;
int                      y=0;  //分配离线消息的结构体
struct user                    //用户名的结构体
{
    char username[10];
};
struct regi_sign               //登录注册用的结构体
{
    char flag;
    char username[10];
    char password[10];
};
struct group                   //群组的结构体
{
    char group_name[10];
    char admin_name[10];
    struct user member[10];
    int  member_num;
};
struct connect_info            //conn_fd登录结构体
{
    int fd;
    char name[10];
}connect_info[30];

struct users                   //用户结构体
{
    struct user user;          //用户名
    char password[10];         //密码
    int  friend_num;           //朋友数
    int  group_num;            //群数
    struct user friend[10];    //朋友信息
    struct group group[10];    //群
    struct users *next;
};
struct leave_message           //离线消息结构体
{
    char  name[10];
    int   number;
}leave_message[30];

struct users*  head = NULL;    //用户信息头指针
struct users*  p;              //指向当前用户的指针

struct message                 //消息结构体
{
    char      type;
    char      from[10];
    char      to[10];
    char      time[30];
    char      changefriname[10];//添加好友和删除
    struct    user name[30]; //附近的人，在线好友。查看好友
    struct    group group;
    char      news[500];
};
struct users* read_input();    //将信息读入链表
struct users* apply_account(struct regi_sign,int);  //申请账户
void save();                   //保存信息到链表
char * my_time();              //自定义时间函数
struct users* sign_in(struct regi_sign,int); //用户登录
void quit(struct regi_sign,int);             //用户未登录退出
void near_friend(struct message,int);        //附近的人
void view_friend(struct message,int);        //查看好友
void add_friend(struct message,int);         //添加好友
void out_line(struct message,int);           //下线
void view_online_friend(struct message,int); //查看在线好友
int del_friend(struct message,int);         //删除好友
int pri_chat(struct message,int);            //私聊
void build_group(struct message,int);        //群聊
void view_group(struct message,int);         //查看我的组
void public_chat(struct message,int);        //群聊
int accept_add_friend(struct message,int);
void leave_message_func(struct regi_sign,int);//离线消息
void apply_add_group(struct message,int);
void accept_add_group(struct message,int);
void refuse_add_group(struct message,int);
void my_status(struct message,int);
int my_zone_access(struct message,int);

int my_zone_access(struct message chat,int i)
{
    struct message near;
    char   near_buf[BUFSIZE];
    int    k;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(near.from,connect_info[k].name))
            break;
    }
    if(near.type == '8')
    {
        memcpy(near_buf,&near,LEN1);
        send(connect_info[k].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    if(near.type == '9')
    {
        memcpy(near_buf,&near,LEN1);
        send(connect_info[k].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    if(near.type == '0')
    {
        memcpy(near_buf,&near,LEN1);
        send(connect_info[k].fd,near_buf,BUFSIZE,0);
        return 0;
    }
}
void my_status(struct message chat,int i)
{
    struct message near;
    struct users *p1;
    char   near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    p1 = head->next;
    int k,j;

    while(p1)
    {
        if(!strcmp((p1->user).username,near.from))
        {
            for(k = 0;k < 10;k++)
            {
                for(j = 0;j < 30;j++)
                {
                    if(!strcmp((p1->friend)[k].username,connect_info[j].name))
                    {
                        memcpy(near_buf,&near,LEN1);
                        send(connect_info[j].fd,near_buf,BUFSIZE,0);
                        break;
                    }
                }
            }
        }
        p1 = p1->next;
    }
}
void refuse_add_group(struct message chat,int i)
{
    int k;
    struct message near;
    char near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    memset(near_buf,0,LEN1);
    near = chat;
    memcpy(near_buf,&near,LEN1);
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
    }
}
void accept_add_group(struct message chat,int i)
{
    int k,j,t,m,flag = 0;
    struct users *p1,*p2,*p3;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct user  name[10];
    struct message near;
    struct group group;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!strcmp(near.group.admin_name,(p1->user).username))   
        {
            for(k = 0;k < 10;k++)                    
            {
                if(!strcmp((p1->group)[k].group_name,near.to))  //找到指定群
                {
                    for(j = 0;j < 10 ;j++)
                    {
                        memcpy(&name[j].username,&(p1->group)[k].member[j].username,10);
                    }
                }
            }
        }
        p1 = p1->next;
    }
    for(k = 0;k < 10;k++)
    {
        p2 = head->next;
        while(p2)
        {
            if(!strcmp(name[k].username,(p2->user).username))
            {

                for(m = 0;m < 10;m++)                    
                {
                    if(!strcmp((p2->group)[m].group_name,near.to))//找到指定组
                    {
                        memcpy(&(p2->group)[m].member[(p2->group)[m].member_num],&near.from,10);
                        t = (p2->group)[m].member_num;
                        (p2->group)[m].member_num = (p2->group)[m].member_num + 1;
                        flag = 1;
                        break;
                    }
                }
                if(flag == 1)
                    break;
                
            }
            p2 = p2->next;
        }
    }
    memcpy(&group.group_name,&near.to,10);
    memcpy(&group.admin_name,&near.group.admin_name,10);
    for(k = 0;k < t;k++)
    {
        memcpy(&(group.member[k].username),&name[k].username,10);
    }
    memcpy(&(group.member[t]),&near.from,10);
    p3 = head->next;
    while(p3)
    {
        if(!(strcmp(near.from,(p3->user).username)))      //查找到每一个成员，将他们的组直接赋值
        {
            (p3->group)[p3->group_num] = group;
            (p3->group)[p3->group_num].member_num = t+1;
            (p3->group_num)++;
            break;
        }
        p3 = p3->next;
    }
    save();
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
void apply_add_group(struct message chat,int i)
{
    struct message near;
    struct users *p1;
    char near_buf[BUFSIZE];
    int k,flag = 0;
    char name[10];           //用来保存群主的名字
    p1 = head->next;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    memcpy(near_buf,&near,LEN1);
    while(p1)
    {
        for(k = 0;k < 10;k++)
        {
            if(!(strcmp((p1->group)[k].group_name,near.to)))
            {
                memcpy(name,(p1->group)[k].admin_name,10);
                memcpy(near.group.admin_name,name,10);
                flag = 1;
                break;
            }
        }
        if(flag == 1)
            break;
        p1 = p1->next;
    }
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,name))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
void leave_message_func(struct regi_sign account,int i) //离线消息
{
    struct message chat;
    char   near_buf[BUFSIZE];
    int k,j;
    FILE *fp;
    memset(&chat,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    for(k = 0;k < 30;k++)
    {
        if(!(strcmp(account.username,leave_message[k].name)))   //找到用户的离线结构体
        {
            if(leave_message[k].number != 0)                    //如果离线消息数不为0
            {
                if((fp = fopen(leave_message[k].name,"a+")) == NULL) //从文件中读取
                {
                    my_err("fopen",__LINE__);
                    exit(0);
                }
                while((fread(&chat,LEN1,1,fp) != -1) && (!feof(fp)))
                {
                    chat.type = 'x';
                    memcpy(near_buf,&chat,LEN1);
                    if(send(connect_info[i].fd,near_buf,BUFSIZE,0)!=BUFSIZE) //将消息逐条发过去
                    {
                        my_err("send",__LINE__);
                        pthread_exit(0);
                    }
                }
                fclose(fp);
                leave_message[k].number = 0;                    //将离线消息数至为0
                unlink(leave_message[k].name);                  //删除文件
                
            }
        }
    }

}
void public_chat(struct message chat ,int i)                     //群聊
{
    int k,j;                                  
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!strcmp(near.from,(p1->user).username))   //找到用户
        {
            for(k = 0;k < 10;k++)                    
            {
                if(!strcmp((p1->group)[k].group_name,near.to))  //找到指定组
                {
                    for(j = 0;j < 30;j++)
                    {
                        if((!strcmp(connect_info[j].name,(p1->group)[k].member[j].username)) && (connect_info[j].fd != connect_info[i].fd))
                        {
                            memcpy(near_buf,&near,LEN1);
                            if(send(connect_info[j].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                            {
                                my_err("send",__LINE__);
                                pthread_exit(0);
                            }
                        }

                    }
                }
            }
        }
        p1 = p1->next;
    }
}
void view_group(struct message chat,int i)             //查看我的组
{
    int k ;
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!strcmp(near.from,(p1->user).username))     //找到用户
        {
            for(k = 0;k < 10;k++)
            {
               if(!strcmp((p1->group)[k].group_name,near.to)) //找到组
                    near.group = (p1->group)[k];
            }
        }
        p1 = p1->next;
    }
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void build_group(struct message chat,int i)                  //建群
{
    int k ;
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    struct message chat1;
    struct user  name[10];
    memset(&near,0,LEN1);
    memset(&chat1,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    chat1 = chat;
    for(k = 0; k < 10; k++)
    {
        memcpy(name[k].username,near.group.member[k].username,10);   //先将成员保存到name[]数组里
    }
    while(p1)
    {
        for(k = 0;k < 10;k++)
        {
            if(!(strcmp(name[k].username,(p1->user).username)))      //查找到每一个成员，将他们的组直接赋值
            {
                (p1->group)[p1->group_num] = near.group;
                (p1->group_num)++;
            }
        }
        p1 = p1->next;
    }
    save();
    printf("[用户]“%s”创建了群 “%s”\t“%s”\n",near.from,near.group.group_name,my_time());
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)       //给群主通知群建立成功
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
    for(k = 0;k < 30;k++)
    {
        if((!strcmp(name[k].username,connect_info[k].name)) && (strcmp(name[k].username,near.from)))
        {
            chat1.type = 'r';
            memset(&near_buf,0,BUFSIZE);
            memcpy(near_buf,&chat1,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);             //给其他群成员通知有人邀请你加入群
        }
    }
}
int pri_chat(struct message chat,int i)                                //私聊
{
    struct users *p1;
    p1 = head->next;
    FILE *fp;
    int  k;
    struct  message  near;
    char    near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    if(!strcmp(near.to,near.from))
    {
        near.type = '6';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    while(p1)
    {
        if(!strcmp((p1->user).username,near.to))
        {
            for(k = 0;k < 30;k++)
            {
                if(!strcmp(near.to,connect_info[k].name))                      //如果在线直接发送
                {
                    near.type = 'o';
                    memcpy(near_buf,&near,LEN1);
                    if(send(connect_info[k].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                    {
                        my_err("send",__LINE__);
                        pthread_exit(0);
                    }
                    return 0;
                }
            }
            if(k == 30)
            {
                near.type = 'd';                                               //如果不在线就转为离线消息
                memcpy(near_buf,&near,LEN1);
                if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                {
                    my_err("send",__LINE__);
                    pthread_exit(0);
                }
                if((fp = fopen(near.to,"ab+")) == NULL)
                {
                    my_err("fopen",__LINE__);
                    exit(0);
                }
                fwrite(&near,LEN1,1,fp);
                fclose(fp);
                for(k = 0;k < 30;k++)
                {
                    if(!(strcmp(leave_message[k].name,near.to)))
                        leave_message[k].number = leave_message[k].number+1;
                }
                return 0;
            }
        }
        p1 = p1->next;
    }
    if(p1 == NULL)
    {
        near.type = '5';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
    }
}
int del_friend(struct message chat,int i)                     //删除好友
{ 
    struct  users *p1;
    p1 = head->next;
    int k;
    struct message near;
    memset(&near,0,LEN1);
    near = chat;
    char  near_buf[BUFSIZE];
    if(!strcmp(near.to,near.from))
    {
        near.type = '3';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from)))           //找到本人
        {
            for(k = 0;k < 10;k++)
            {
                if(!(strcmp(near.to,(p1->friend)[k].username)))
                {
                    for(k;k < 10;k++)
                    {
                        memcpy((p1->friend)[k].username,(p1->friend)[k+1].username,10);//将数组后的元素依次向前移动
                    }
                    p1->friend_num--;     //将好友总数减一
                    save();
                    printf("[用户]“%s”删除了好友“%s” \t“%s”\n",near.from,near.to,my_time());
                    memset(near_buf,0,BUFSIZE);
                    memcpy(near_buf,&near,LEN1);
                    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
                    {
                        my_err("send",__LINE__);
                        pthread_exit(0);
                    }
                    return 0;
                }
            }
            
        }        
        p1 = p1->next;
    }
    if(p1 == NULL)
    {
        near.type = '1';
        memset(near_buf,0,BUFSIZE);
        memcpy(near_buf,&near,LEN1);
        if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
        {
            my_err("send",__LINE__);
            pthread_exit(0);
        }
    }
}

void view_online_friend(struct message chat,int i)             //查看在线好友
{ 
    struct  users *p1;                            
    p1 = head->next;
    int k,j;
    struct message near;
    struct user    name[10];
    memset(&name,0,sizeof(struct user));
    memset(&near,0,LEN1);
    near = chat;
    char  near_buf[BUFSIZE];
    near.type = 'z';
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from)))           //查找到那个人
        {
            for(k = 0;k < 10;k++)
                memcpy(name[k].username,(p1->friend)[k].username,10);//将好友全部保存到name[]数组里
            for(k = 0;k < 10;k++)
            {
                for(j = 0;j < 30;j++)
                {
                    if((!strcmp(name[k].username,connect_info[j].name)) && (connect_info[j].fd != -1))//依次检查各个好友的是否在线
                        {
                            memcpy(near.name[k].username,connect_info[j].name,10);        //再将在线的好友保存到name[]中
                        }
                }
            }
            
        }
        p1 = p1->next;
    }
    memset(near_buf,0,BUFSIZE);
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void out_line(struct message chat,int i)          //下线，结束自己的线程
{
    struct message near;
    memset(&near,0,LEN1);
    near = chat;
    connect_info[i].fd = -1;                       //将信息初始化
    strcpy(connect_info[i].name," ");
    printf("[用户]%s下线了 \t%s\n",near.from,my_time());
    pthread_exit(0);
}
void refuse_add(struct message chat,int i)         //拒绝添加
{
    int k ;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);                    //当被添加好友输入n时，发送标志为e的拒绝状态
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))
        {
            memcpy(near_buf,&near,LEN1);
            send(connect_info[k].fd,near_buf,BUFSIZE,0);
        }
    }
}
int accept_add_friend(struct message chat,int i)   //第一次的添加消息的转发
{
    struct users *p1;
    p1 = head->next;
    int k;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    if(!strcmp(near.to,near.from))
    {
        near.type = '2';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);
        return 0;
    }
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from)))           //找到本人
        {
            for(k = 0;k < 10;k++)
            {
                if((!(strcmp(near.to,(p1->friend)[k].username))) && (strlen(near.to) != 0))
                {
                    near.type = '4';
                    memcpy(near_buf,&near,LEN1);
                    send(connect_info[i].fd,near_buf,BUFSIZE,0);
                    return 0;
                }
            }
        }
        p1 = p1->next;
    }
    for(k = 0;k < 30;k++)
    {
        if(!(strcmp(connect_info[k].name,near.to))) //找到被添加的人，然后向他发送添加请求
        {
            memcpy(near_buf,&near,LEN1);
            if(send(connect_info[k].fd,near_buf,BUFSIZE,0)!= BUFSIZE)
            {
                my_err("send",__LINE__);
                pthread_exit(0);
            }
            return 0;
        }
    }
    if(k == 30 || (strlen(near.to) == 0))                                   //未找到此人或者他不在线
    {
        near.type = 'm';
        memcpy(near_buf,&near,LEN1);
        send(connect_info[i].fd,near_buf,BUFSIZE,0);  
    }
}
void add_friend(struct message chat,int i)        //同意添加
{
    int k;
    struct users *p1;
    p1 = head->next;
    char  near_buf[BUFSIZE];
    struct message near;
    memset(&near,0,LEN1);
    memset(near_buf,0,BUFSIZE);
    near = chat;
    while(p1)
    {
        if(!(strcmp(near.from,(p1->user).username)))  //互相将对方添加到自己的朋友中
        {
            memcpy(((p1->friend)[p1->friend_num].username),near.to,10);
            p1->friend_num++;
        }
        if(!(strcmp(near.to,(p1->user).username)))
        {
            memcpy(((p1->friend)[p1->friend_num].username),near.from,10);
            p1->friend_num++;  //增加完之后让他的朋友数加1
        }
        p1 = p1->next;
        save();                                       //保存信息
    }
    printf("[用户]“%s”添加了“%s”为好友 \t%s\n",near.from,near.to,my_time());
    memcpy(near_buf,&near,LEN1);
    for(k = 0;k < 30;k++)
    {
        if(!strcmp(connect_info[k].name,near.from))   //给自己发送添加成功提示
        {
            if(send(connect_info[k].fd,near_buf,BUFSIZE,0) != BUFSIZE)
            {
                my_err("send",__LINE__);
                pthread_exit(0);
            }
        }
    }
}
void view_friend(struct message chat,int i)           //查看好友
{
    struct  users *p1;
    p1 = head->next;
    int k;
    struct message near;
    memset(&near,0,LEN1);
    near = chat;
    char  near_buf[BUFSIZE];
    near.type = 'v';                                  //查看自己的全部好友
    while(p1)
    {
        if(!(strcmp((p1->user).username,near.from))) //找到自己
        {
            for(k = 0;k < 10;k++)
                memcpy(near.name[k].username,(p1->friend)[k].username,10);//将好友保存到数组里
        }
        p1 = p1->next;
    }
    memset(near_buf,0,BUFSIZE);
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)   //将好友信息发送过去
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void near_friend(struct message chat,int i)                  //查看所有在线的人
{
    int  k;
    struct message near;
    char   near_buf[BUFSIZE];
    memset(&near,0,LEN1);
    near = chat;
    for(k = 0;k < 30;k++)
    {
        if(connect_info[k].fd != -1)                         //只要在线就输出
            memcpy(near.name[k].username,connect_info[k].name,10);
    }
    
    memset(near_buf,0,BUFSIZE);
    memcpy(near_buf,&near,LEN1);
    if(send(connect_info[i].fd,near_buf,BUFSIZE,0) != BUFSIZE)
    {
        my_err("send",__LINE__);
        pthread_exit(0);
    }
}
void quit(struct regi_sign account,int i)                  //在登录界面退出
{
    struct regi_sign quit;
    quit = account;
    connect_info[i].fd = -1;
    strcpy(connect_info[i].name," ");
    printf("客户端退出 \t%s\n",my_time());
    pthread_exit(0);
}
struct users* sign_in(struct regi_sign account,int i)
{
    int  k,m = 0;
    struct regi_sign  sign;
    struct users  *p1;
    p1 = head->next;
    sign = account;
    for(k = 0;k < 30; k++)                        //判断是否在线
    {
        if(!strcmp(connect_info[k].name,sign.username))
            m = m+1;
    }
   // printf("m:%d\n",m);
    if(m >= 1)                                    //如果已经在线就拒绝登录
    {
        send(connect_info[i].fd,"w",BUFSIZE,0);
        return NULL;
    }
    if(m == 0)                                     //如果不在线
    {
        while(p1)
        {
            if((!strcmp((p1->user).username,sign.username)) && (!strcmp(p1->password,sign.password))) //校验密码
            {
                if((send(connect_info[i].fd,"y",BUFSIZE,0)) != BUFSIZE)                               //密码正确就登录
                {
                    my_err("login send",__LINE__);
                }
                printf("[用户]%s上线了 \t%s\n",sign.username,my_time());
                memcpy(connect_info[i].name,sign.username,10);
                memcpy(leave_message[y++].name,sign.username,10);
            
                return p1;
            }
            p1 = p1->next; //判断未成功，让p1指向下一个
        }
        if(p1 == NULL)                                                                                //没找到，拒绝登录
        {
            send(connect_info[i].fd,"n",BUFSIZE,0);
            return NULL;
        }
    }
}
char * my_time()               //自定义的获取时间的函数
{
    time_t now;
    time(&now);
    return (ctime(&now));
}
void save()                   //将链表信息保存到文件
{
    FILE *fp;
    struct users *p1;
    p1 = head->next;
    if((fp = fopen("userlist","wb")) == NULL)
    {
        my_err("fopen",__LINE__);
        exit(0);
    }
    while(p1 != NULL)
    {
        fwrite(p1,LEN,1,fp);
        p1 = p1->next;
    }
    fclose(fp);

}
struct users* apply_account(struct regi_sign account,int i) //申请账户
{
    struct    regi_sign apply;
    struct    users   *p1,*p2;
    p1 = head;
    apply = account;
    while(p1->next != NULL)
    {
        if(strcmp((p1->next->user).username,apply.username) == 0) //判断是否有重名
        {
            send(connect_info[i].fd,"n",BUFSIZE,0);
            return NULL;
        }
        p1 = p1->next;
    }
    p2 = (struct users*)malloc(LEN);
    memset(p2,0,LEN);         //对刚malloc的结构体进行清空
    strcpy((p2->user).username,apply.username);                  //为新用户申请空间，并且将用户名和密码赋上
    strcpy(p2->password,apply.password);
    p1->next = p2;
    p2->next = NULL;
    save();
    send(connect_info[i].fd,"y",1024,0);
    memcpy(connect_info[i].name,apply.username,10);
    memcpy(leave_message[y++].name,apply.username,10);
    printf("[用户]%s注册成功 \t%s\n",apply.username,my_time());
    return (p2);
}
void *sign_quit()                                               //服务器退出函数
{
    char choice[20];
    do
    {
        printf("-------退出服务器(请输入quit)--------\n");
        scanf("%s",choice);
    }while(strcmp("quit",choice));
    if(!strcmp("quit",choice))
    {
        exit(0);
    }
}
struct users* read_input()                                      //将文件中的信息读到链表
{
    FILE *fp;
    struct users *p1,*p2;
    if((fp = fopen("userlist","rb")) == NULL)
    {
        fp = fopen("userlist","ab+");
    }
    p1 = (struct users *)malloc(LEN);
    p2 = (struct users *)malloc(LEN);
    head = p1;
    fread(p2,LEN,1,fp);
    while(!feof(fp))
    {
        p1->next = p2;
        p1 = p2;
        p2 = (struct users*)malloc(LEN);
        fread(p2,LEN,1,fp);
    }
    p1->next = NULL;
    free(p2);
    fclose(fp);
    return (head);
}
void *client_t(void *arg)                                       //线程函数，专门用来处理客户端除过登录申请账户之外的请求
{
    int i = *(int *)arg;
    char      recv_buf[BUFSIZE];
    while(1)
    {
        struct    regi_sign account;
        memset(&account,0,sizeof(struct regi_sign));
        memset(recv_buf,0,sizeof(recv_buf));
        if(recv(connect_info[i].fd,recv_buf,sizeof(recv_buf),0) != BUFSIZE)
        {
            connect_info[i].fd = -1;
            printf("[用户]%s异常离线 \t%s\n",connect_info[i].name,my_time());
            strcpy(connect_info[i].name," ");
            pthread_exit(0);
        }
        pthread_mutex_lock(&mutex);
        memcpy(&account,recv_buf,sizeof(recv_buf));
        head = read_input(); //保存头结点
        if(account.flag == 'r')  //注册
        {
            p = apply_account(account,i);
            pthread_mutex_unlock(&mutex);
            leave_message_func(account,i);
            if(p != NULL)
                break;
        }
        if(account.flag == 'l')  //登录
        {
            p = sign_in(account,i);
            pthread_mutex_unlock(&mutex);
            leave_message_func(account,i);
            if(p != NULL)
                break;
        }
        if(account.flag == 'q')  //退出
        {
            pthread_mutex_unlock(&mutex);
            quit(account,i);
        }
    }
    while(1)
    {
        struct    message  chat;
        char      recv_buf[BUFSIZE];
        int       ret;
        memset(&chat,0,LEN1);
        memset(recv_buf,0,BUFSIZE);
        if((ret = recv(connect_info[i].fd,recv_buf,BUFSIZE,0))!= BUFSIZE)
        {
         //   printf("ret : %d\n",ret);
         //   printf("%s\n",recv_buf);
            connect_info[i].fd = -1;
            printf("[用户]%s异常离线 \t%s\n",connect_info[i].name,my_time());
            strcpy(connect_info[i].name," ");
            pthread_exit(0);
        }
        memcpy(&chat,recv_buf,BUFSIZE);
        //printf("this is chat.type:%c\n",chat.type);
        switch(chat.type)
        {
            case 'f':   //查看附近的人
            {
                near_friend(chat,i);
                break;
            }
            case 'v':   //查看所有好友
            {
                view_friend(chat,i);
                break;
            }
            case 't':   //添加好友
            {
                add_friend(chat,i);
                break;
            }
            case 'g':   //下线
            {
                out_line(chat,i);
                break;
            }
            case 'z':   //在线好友
            {
                view_online_friend(chat,i);
                break;
            }
            case 's':   //删除好友
            {
                del_friend(chat,i);   
                break;
            }
            case 'o':   //私聊
            {
                pri_chat(chat,i);
                break;
            }
            case 'j':   //建群
            {
                build_group(chat,i);
                break;
            }
            case 'c':   //查看群信息
            {
                view_group(chat,i);
                break;
            }
            case 'b':   //群聊
            {
                public_chat(chat,i);
                break;
            }
            case 'y':   //接受添加好友
            {
                accept_add_friend(chat,i);
                break;
            }
            case 'e':   //拒绝添加
            {
                refuse_add(chat,i);
                break;
            }
            case 'h':   //申请入群
            {
                apply_add_group(chat,i);
                break;
            }
            case 'a':   //同意入群
            {
                accept_add_group(chat,i);
                break;
            }
            case 'i':
            {
                refuse_add_group(chat,i);
                break;
            }
            case '7':
            {
                my_status(chat,i);
                break;
            }
            case '8':
            {
                my_zone_access(chat,i);
                break;
            }
            case '9':
            {
                my_zone_access(chat,i);
                break;
            }
            case '0':
            {
                my_zone_access(chat,i);
                break;
            }
        }
    }
}
int main(int argc,char *argv[])
{
    int                          sock_fd,conn_fd;
    struct     sockaddr_in       serv_addr,conn_addr;
    socklen_t                    conn_len;
    pid_t                        pid;
    char                         recv_buf[128];
    int                          ret,i;
    int                          optval;
    time_t                       now;
    pthread_t                    thid,quit_thid;
    int                          status;
    pthread_mutex_init(&mutex,NULL);
    if((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0) //创建套接字
    {
        my_err("socket",__LINE__);
    }
    optval = 1;
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void*)&optval,sizeof(int)) < 0) //设定端口，要是异常退出還可以在绑定
    {
        my_err("setsockopt",__LINE__);
    }

    memset(&serv_addr,0,sizeof(struct sockaddr_in));                   //初始化服务器端端口，IP信息
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    pthread_create(&quit_thid,NULL,(void*)sign_quit,NULL);
    if(bind(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in)) < 0)//将IP与端口绑定在一起
    {
        my_err("bind",__LINE__);
    }
    if(listen(sock_fd,LISTENQ) < 0)                                    //将套接字转化为接听套接字
    {
        my_err("bind",__LINE__);
    }
    for(i = 0;i < 30;i++)                                             //初始化离线消息和conn_fd结构体信息
    {
        connect_info[i].fd = -1;
        strcpy(connect_info[i].name," ");
        strcpy(leave_message[i].name," ");
        leave_message[i].number = 0;
    }
    conn_len = sizeof(struct sockaddr_in);
    while(1)
    {
        if((conn_fd= accept(sock_fd,(struct sockaddr *)&conn_addr,&conn_len)) < 0) //等待连接状态
        {
            my_err("accept",__LINE__);
        }
        for(i = 0;i < 30;i++)
        {
            if(connect_info[i].fd == -1)
                break;
        }
        connect_info[i].fd = conn_fd;     //将conn_fd记录
        time(&now);
        printf("有新的连接：ip:%s\t%s\n",inet_ntoa(conn_addr.sin_addr),ctime(&now));
        int f = i;              //必须这样才能让传上去的i正确
        pthread_create(&thid,NULL,client_t,(void*)&f); //专门用来处理客户端除过登录申请账户之外的请求
    }
     return 0;
}
