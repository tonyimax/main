//
// Created by Hacker X on 2024/9/5.
//
#include <iostream>  //引用头文件
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string> //bzero函数用
#include <unistd.h> //close函数用

using namespace std; //使用命名空间std
int main() {
    cout << "Linux网络编程" << endl;
    cout << "LINUX NETWORK PROGRAMMING" << endl;
    cout << "TCP网络通信客户端(Client):" << endl;

    //创建socket对象
    int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sockfd<0){
        printf("Error: 创建客户端网络套接字失败!!!\n");
    }else{
        printf("===创建客户端网络套接字成功===,sockfd:%d\n",sockfd);
    }

    //初始化地址
    struct sockaddr_in _addr;
    bzero(&_addr, sizeof(_addr));//清零
    _addr.sin_family=AF_INET;//使用IPv4地址通信
    _addr.sin_port= htons(8888);//服务器端口
    //暂时不指定IP地址

    // int connect(int socket, const struct sockaddr *address,socklen_t address_len);
    int conRet = connect(
                    sockfd,//连接的socket句柄
                    (struct sockaddr *)&_addr,//地址结构
                    sizeof(_addr) //地址结构大小
            );
    if(0==conRet){
        printf("连接TCP服务器成功\n");
    }else{
        printf("连接TCP服务器失败\n");
        close(sockfd);
    }

    //循环接收与发送数据
    while (true){
        //向服务端发送字符串
        printf("请输入要发送的字符串然后回车:");
        char buf[512]={0};//最多发512
        //从标准入流获取字符并存储到buf
        fgets(buf,sizeof(buf),stdin);
        send(sockfd,buf, strlen(buf),0);

        //接收服务端的字符串并显示
        //ssize_t recv(int socket, void *buffer, size_t length, int flags);
        char recvbuf[512]={0};//最多接收512
        ssize_t recv_len = recv(sockfd,recvbuf, strlen(recvbuf),0);//接收
        //显示
        printf("接收到服务器发来的字符串: %s ,长度:%zd\n\n",recvbuf,recv_len);
        printf("=========================\n");

        //如果输入q客户端也退出
        if(0== strcmp(buf,"q\n")){
            printf("成功退出!!\n");
            break;
        }

    }
    //关闭Socket
    close(sockfd);
    return 0;//退出程序

}