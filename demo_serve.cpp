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
    if(argc!=2){
        cout<<"Using:./demo_serve 通讯端口\nExample:./demo_serve 5005\n\n"; //端口大于1024,不与其它的重复
        cout<<"注意：如果运行服务端程序的Linux系统的防火墙必须开通5005端口"<<endl;
        cout<<"如果是云服务器，还要开通云平台的访问策略"<<endl;
        return -1;
    }

    //第一步：创建服务端的socket
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    if(listenfd == -1){
        perror("socket");
        return -1;
    }

    //第二步：把服务端用于通信的IP和端口绑定到socket上
    struct sockaddr_in servaddr;    //用于存放服务端IP和端口的数据结构
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;  //指定协议
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   //服务端任意网卡的IP都可以用于通信
    servaddr.sin_port = htons(atoi(argv[1]));   //指定通信端口，普通用户只能用1024以上的端口
    //绑定服务端的IP和端口
    if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0){
        perror("bind");
        close(listenfd);
        return -1;
    }

    //第三步：把socket设置为可连接的状态
    if(listen(listenfd,5) != 0){
        perror("listen");
        close(listenfd);
        return -1;
    }

    //第四步：受理客户端的连接请求，如果没有客户端连上来，accept()函数将阻塞等待
    int clientfd = accept(listenfd,0,0);
    if(clientfd == -1){
        perror("accept");
        close(listenfd);
        return -1;
    }

    cout<<"客户端已连接"<<endl;

    //第五步：与客户端通信，接受客户端发过来的报文后，回复ok
    char buffer[1024];
    while(true){
        int iret;
        memset(buffer,0,sizeof(buffer));
        //接收客户端的请求报文，如果客户端没有发送请求报文，recv()函数将阻塞等待
        //如果客户端已断开连接，recv()函数返回0
        if((iret = recv(clientfd,buffer,sizeof(buffer),0))<=0){
            cout<<"iret = "<<iret<<endl;
            break;
        }
        cout<<"接收"<<buffer<<endl;
        strcpy(buffer,"ok");    //生成回应报文内容
        //向客户端发送回应报文
        if((iret = send(clientfd,buffer,strlen(buffer),0))<=0){
            perror("send");
            break;
        }
        cout<<"发送:"<<buffer<<endl;
    }

    //第六步：关闭socket，释放资源
    close(listenfd);
    close(clientfd);
}