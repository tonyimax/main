//
// Created by Hacker X on 2024/9/7.
//

#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <pthread.h>

using namespace std;
void *recv_send_client(void *arg);
void *handler_connect(void *arg);

int activeConnect=0;//当前活动连接客户端
std::vector<int> sockets(90,-1);

int main(int argc,char* argv[])
{
    cout << "Linux网络编程" << endl;
    cout << "LINUX NETWORK PROGRAMMING" << endl;
    cout << "TCP网络通信服务端(Server):" << endl;

    //使用单独线程处理每个连接的数据发送与接收
    pthread_t tid;
    pthread_create(&tid,NULL,handler_connect,(void *)NULL);
    pthread_detach(tid);

    while (true){

    }

    return 0;
}

void *handler_connect(void *arg){
    //创建socket
    int serv_sock_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(serv_sock_fd<0){
        printf("创建socket失败!\n");
    }else{
        printf("===创建socket成功===\n");
    }

    //初始化地址
    struct sockaddr_in servAddr;
    bzero(&servAddr,sizeof(servAddr));
    servAddr.sin_family =AF_INET;
    servAddr.sin_port = htons(9999);
    servAddr.sin_addr.s_addr = htons(INADDR_ANY);

    //绑定socket
    int bindResult = ::bind(serv_sock_fd,(struct sockaddr*)&servAddr,sizeof(servAddr));
    if(bindResult==0){
        printf("===绑定socket成功===\n");
    }else{
        printf("绑定socket失败\n");
        close(serv_sock_fd);
    }

    //监听socket
    int listenResult = listen(serv_sock_fd,100);
    if(listenResult==0){
        printf("===监听socket成功===\n");
    }else{
        printf("监听socket失败\n");
        close(serv_sock_fd);
    }
    printf("TCP服务已成功启动并监听端口:9999\n");

    //循环接收与发送数据
    while (true){
        if(activeConnect>=90){
            printf("当前服务器连接忙,请稍后再试...\n");
            continue;
        }else{
            //接受客户端连接
            struct sockaddr_in clientAddr;
            socklen_t clentAddrLen = sizeof(clientAddr);
            int client_sock_fd = accept(serv_sock_fd,(struct sockaddr*)&clientAddr,&clentAddrLen);
            if(client_sock_fd<0){
                printf("接收客户端连接失败,client_sock_fd:%d\n",client_sock_fd);
                continue;//处理下一个连接
            }else{
                activeConnect +=1;
                printf("当前已连接客户端数量:%d\n",activeConnect);
                //输出客户端连接信息
                char clientIP[INET_ADDRSTRLEN]={0};
                inet_ntop(AF_INET,&clientAddr.sin_addr,clientIP,INET_ADDRSTRLEN);
                printf("接收客户端连接成功,client_sock_fd:%d,客户端IP地址:%s,端口:%d\n",
                       client_sock_fd,clientIP,ntohs(clientAddr.sin_port));
                sockets.push_back(client_sock_fd);//添加已连接成功的socket到数组中

                //使用单独线程处理每个连接的数据发送与接收
                pthread_t tid1;
                int intPid = pthread_create(&tid1,NULL,recv_send_client,(void *)client_sock_fd);
                printf("已启动线程:%d来处理socket:%d的消息接收与发送\n",intPid,client_sock_fd);
            }
        }
    }

    //关闭socket
    close(serv_sock_fd);
    printf("服务端已成功关闭所有客户端连接并安全退出!");
}

void *recv_send_client(void *arg){
    int curSock = *(int*)&arg;
    printf("===子线程recv_send_client已启动,使用线程来处理socket:%d===\n",curSock);
    while (true){
            if(curSock>0){
                //接收客户端消息
                char recvBuf[2048]={0};
                ssize_t recvLen = recv(curSock,recvBuf,sizeof(recvBuf),0);
                if(-1==recvLen){
                    printf("===当前无任何socket消息:%d\n",curSock);
                    continue;//当前没有接收到任何数据
                }
                printf("接收到客户端%d的消息: \n长度:%zd\n内容:%s\n",curSock,recvLen,recvBuf);

                //向客户端发送消息
                char sendBuf[2048]="服务端消息:===this message send by ServerMulti===\n";
                ssize_t sendLen = send(curSock,sendBuf,sizeof(sendBuf),0);
                printf("向客户端发送消息: \n长度:%zd\n内容:%s\n",sendLen,sendBuf);

                if((0==strcmp(recvBuf,"q\n"))||(0==strcmp(recvBuf,"q\r\n"))){
                    printf("接收到客户输入q,请求退出socket:%d\n",curSock);
                    close(curSock);
                    activeConnect -=1;
                    printf("当前已连接客户端数量:%d\n",activeConnect);
                    continue;//继续处理下一连接
                }
            }
    }
}