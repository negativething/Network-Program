#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc,char *argv[]){
    if(argc!=3){
        cout<<"Using:./demo_supply 用户端的IP 服务端的端口 IP\nExample:./demo1 192.168.101.138 5005\n\n";
        return -1;
    }
    
    //第一步：创建客户端的socket
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1){
        perror("socket");
        return -1;
    }

    //第二步：向服务器发起连接请求
    struct hostent* h;  //用于存放服务端IP的数据结构
    if((h = gethostbyname(argv[1])) == 0){  //将字符串格式的IP地址转化成结构体
        cout<<"gethostbyname failed.\n"<<endl;
        close(sockfd);
        return -1;
    }

    struct sockaddr_in servaddr;    //用于存放服务端IP和端口的结构体
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));   //指定服务端的通信端口
    memcpy(&servaddr.sin_addr,h->h_addr,h->h_length);   //指定服务端的IP地址
    //IP:192.168.101.139 端口:5005
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) !=0){  //向服务端发起连接请求
        perror("connect");
        close(sockfd);
        return -1;
    }

    //第三步：与服务端通信，发送一个请求报文后等待回复，然后再发下一个请求报文
    char buffer[1024];
    for(int i=0;i<3;i++){   //循环3次，与服务端进行三次通讯
        int iret;
        memset(buffer,0,sizeof(buffer));
        sprintf(buffer,"这是第%d个超级女生，编号%03d。",i+1,i+1);   //生成请求报文内容
        //向服务端发送请求报文
        if((iret = send(sockfd,buffer,strlen(buffer),0))<=0){
            perror("send");
            break;
        }
        cout<<"发送:"<<buffer<<endl;

        memset(buffer,0,sizeof(buffer));
        //接收服务的回应报文，如果服务端没有发送回应报文，recv()函数将阻塞等待
        if((iret = recv(sockfd,buffer,sizeof(buffer),0))<=0){
            cout<<"iret = "<<iret<<endl;
            break;
        }
        cout<<"接收:"<<buffer<<endl;

        sleep(1);
    }

    //第四步：关闭socket，释放资源
    close(sockfd);
}