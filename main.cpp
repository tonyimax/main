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

    //字节序认识
    //声明联合类型
    union{
        short _shortVal;
        char _charVal[sizeof(short)];
    }char_union_short;

    //联合类型赋值
    //_shortVal[0]=0x02 第一个字节 ===> 2
    //_shortVal[1]=0x01 第二个字节 ===> 1 << 8 = 256 因为是高位,需要左移1个字节(8bit)
    //256+2 = 258
    char_union_short._shortVal=0x0102;
    //判断字节高低位
    if ((char_union_short._charVal[0]==1) &&
        (char_union_short._charVal[1]==2)){
        printf("大端,高位字节存储于低地址\n");
    }else if((char_union_short._charVal[0]==2) &&
             (char_union_short._charVal[1]==1)){
        printf("小端,低位字节存储于低地址\n");
    }else{
        printf("字节位的值非法");
    }
    printf("char_union_short._charVal[0]==%i\n"
           "char_union_short._charVal[1]==%i\n",
           char_union_short._charVal[0],
           char_union_short._charVal[1]);

    //https://learn.microsoft.com/zh-cn/windows/win32/api/ws2tcpip/nf-ws2tcpip-inet_pton
    //主机字节序转网络字节序
    auto ns = htons(char_union_short._shortVal);//int16
    auto nl = htonl(char_union_short._shortVal);//int32
    auto nll = htonll(char_union_short._shortVal);//int64
    printf("主机字节序转换为网络字节序: %i===> %i , %u , %llu \n",char_union_short._shortVal,ns,nl,nll);

    //网络字节序转主机字节序
    auto hs = ntohs(ns);//int16
    auto hl = ntohl(nl);//int32
    auto hll = ntohll(nll);//int64
    printf("网络字节序转换为主机字节序: ===> ns: %i , nl: %u , nll: %llu \n",hs,hl,hll);

    //网络地址转换
    //点分地十进制转整数
    char myip[] = "192.168.16.108";//点分地十进制
    unsigned int myip_int=0;//保存转换为整数的IP地址
    unsigned char *myip_point=NULL;//指向IP地址的指针
    //转换
    inet_pton(AF_INET,//表示传入的地址为IPv4地址
              myip,//点分地十进制形式的IPv4地址
              &myip_int);//将转换后的整数保存到myip_int,因为要求传入的是指针,所以要传入myip_int的地址
    printf("%s转换为整数为:%d\n",myip,myip_int);

    //通过转换为整数的地址按指针方式来取点分十进制的值
    myip_point=(unsigned char*)&myip_int;//先取地址,后转换为unsigned char类型指针(0~255之间)
    auto a=*myip_point;//解引用 IP地址第1个点分
    auto b=*(myip_point+1);//解引用 IP地址第2个点分
    auto c=*(myip_point+2);//解引用 IP地址第3个点分
    auto d=*(myip_point+3);//解引用 IP地址第4个点分
    printf("通过整数地址转换为unsigned char*后取IP地址的点分十进制位: %d.%d.%d.%d\n",a,b,c,d);

    //整数转点分十进制
    char saveIp[16]={0};//保存IP地址
    inet_ntop(AF_INET,//表示传入的地址为IPv4地址
              myip_point,//要转换为字符串的网络地址
              saveIp,//存储转换后的IP地址
              16);//表示IP地址长度
    printf("整数网络地址:%d转换为IP地址的点分十进制形式: %s\n",myip_int,saveIp);

    //域名查询主机操作
    char url[64]="www.bing.com";
    hostent *ent = gethostbyname(url);
    if (NULL==ent){
        printf("获取网址域名:%s对应主机地址失败\n",url);
    }else{
        printf("域名:%s对应主机名:%s\n",url,ent->h_name);
    }

    //遍历主机别名
    for (auto t = ent->h_aliases;*t!=NULL;t++) {
        printf("主机别名:%s\n",*t);
    }
    char tmpIP[32]={0};
    //遍历地址
    switch (ent->h_addrtype) {
        case AF_INET:
            printf("IPv4网络地址\n");
            for (auto t=ent->h_addr_list;*t!=NULL; t++) {
                inet_ntop(ent->h_addrtype,t,tmpIP,sizeof(tmpIP));
                printf("IPv4地址:%s\n",tmpIP);
            }
            break;
        case AF_INET6:
            printf("IPv6网络地址\n");
            for (auto t=ent->h_addr_list;*t!=NULL; t++) {
                inet_ntop(ent->h_addrtype,t,tmpIP,sizeof(tmpIP));
                printf("IPv6地址:%s\n",tmpIP);
            }
            break;
        default:
            printf("未知网络地址类型\n");
            break;
    }


    //通过IP地址获取主机名
    //转换IP为整数并存储到地址结构中
    char hostip[16]="113.45.191.151";//IP地址 ===> 16777343
    struct sockaddr_in _in_addr;//网络地址结构
    int ret = inet_aton(hostip,//要转换的IP地址
                        &_in_addr.sin_addr);//IP地址转换后保存的位置(无行号32位整形)
    if(ret>0){
        printf("IP地址转换为整数成功,转换结果:ret=%d,转换后的整数IP值: inet_aton(%s)===>%d\n",ret,hostip,_in_addr.sin_addr.s_addr);
    }else{
        printf("IP地址转换为整数失败:ret=%d\n",ret);
    }
    //使用地址结构获取主机信息
    hostent *new_ent = gethostbyaddr(
            &_in_addr.sin_addr,//地址
                   4,//地址长度(指针大小为4)
                   AF_INET//协议类型
            );
    if (NULL==new_ent){
        printf("获取IP地址主机信息:%s对应主机地址失败\n",hostip);
    }else{
        printf("IP地址:%s对应主机名:%s\n",hostip,new_ent->h_name);
    }

    //遍历主机别名
    for (auto t = new_ent->h_aliases;*t!=NULL;t++) {
        printf("主机别名:%s\n",*t);
    }

    memset(tmpIP,0,32);
    //遍历地址
    switch (new_ent->h_addrtype) {
        case AF_INET:
            printf("IPv4网络地址\n");
            for (auto t=new_ent->h_addr_list;*t!=NULL; t++) {
                inet_ntop(new_ent->h_addrtype,t,tmpIP,sizeof(tmpIP));
                printf("IPv4地址:%s\n",tmpIP);
            }
            break;
        case AF_INET6:
            printf("IPv6网络地址\n");
            for (auto t=new_ent->h_addr_list;*t!=NULL; t++) {
                inet_ntop(new_ent->h_addrtype,t,tmpIP,sizeof(tmpIP));
                printf("IPv6地址:%s\n",tmpIP);
            }
            break;
        default:
            printf("未知网络地址类型\n");
            break;
    }


    //根据服务名与服务协议取服务信息
    struct  servent *se = getservbyname("ftp", //服务名
                                        "udp");//协议
    //如果服务信息可用
    if(se){
        printf("ftp服务信息:===>服务名:%s,服务端口:%d,使用协议:%s\n",
                                       se->s_name, //服务名
                                       ntohs(se->s_port),//服务端口
                                       se->s_proto);//通信协议
    }else{
        printf("获取ftp(udp)服务信息失败\n");
    }

    //遍历服务别名列表
    if (se){
        for (auto t = se->s_aliases;*t!=NULL ; t++) {
            printf("ftp服务别名:%s\n",*t);//打印别名
        }
    }

    //根据端口号查找服务信息
    printf("===根据端口号查找服务信息===\n");
    se = getservbyport(
            htons(21),//网络字节序服务端口号,21为FTP服务端口号
            "tcp"
            );
    //如果服务信息可用
    if(se){
        printf("ftp服务信息:===>服务名:%s,服务端口:%d,使用协议:%s\n",
               se->s_name, //服务名
               ntohs(se->s_port),//服务端口
               se->s_proto);//通信协议
    }else{
        printf("获取ftp(udp)服务信息失败\n");
    }

    //遍历服务别名列表
    if (se){
        for (auto t = se->s_aliases;*t!=NULL ; t++) {
            printf("ftp服务别名:%s\n",*t);//打印别名
        }
    }

    //TCP报文: 20字节
    //0~31 32位  前16源端口  后16位目标端口
    //32~63 32位 SEQ
    //64~95 32位 ACK
    //
    //SYN_SENT --->SYN=1--->SEQ=J--->
    //SYN_RCVD --->SYN=1--->ACK=1--->ack=j+1--->SEQ=K--->
    //ESTABLISHED --->ACK=1--->ack=K+1--->ESTABLISHED

    //TCP建立连接的三次握手过程
    //1.发送端 SYN=1,SEQ=J  J为随机生成的SN  ---> 发送给接收端 --->进入SYN_SENT状态 等待接收端确认
    //2.接收端 默认进入SYN_RCVD状态 --->收到发送端的数据(SYN=1,SEQ=J)后 ---> SYN=1,ACK=1(确认发送端连接请求)
    //          SYN=1,ACK=1, ack=J+1,SEQ=K K为随机生成的SN ---> 发送数据(SYN=1,ACK=1, ack=J+1,SEQ=K) 给发送端
    //3.检查接收端发来的确认报文 ack=J+1,ACK=1 如果正确 ---> 构造数据(ACK=1,ack=K+1)再次发给接收端
    //  接收端检查数据(ack=K+1,ACK=1)是否正确,如正确进入ESTABLISHED状态,此时发送端也进入ESTABLISHED状态
    //CLIENT--->SYN=1,SEQ=J--->
    //SERVER--->SYN=1,ACK=1,ack=J+1,SEQ=K
    //CLIENT--->ack=J+1,ACK=1--->ACK=1,ack=K+1
    //SERVER--->ack=K+1,ACK=1--->ESTABLISHED
    //CLIENT--->ESTABLISHED

    //TCP断开连接四次握手过程
    //CLIENT--->FIN_WAIT1--->FIN M
    //SERVER--->CLOSE_WAIT
    //SERVER--->ack M+1
    //CLIENT--->FIN_WAIT2
    //SERVER--->FIN N
    //SERVER--->LAST_ACK
    //CLIENT--->TIME_WAIT
    //CLIENT--->ACK=1 ack=K+1
    //SERVER--->CLOSED
    //
    //


    return 0;
}
