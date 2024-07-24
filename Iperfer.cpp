#include<iostream>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<time.h>
#include<iomanip>
#include<arpa/inet.h>

int main(int argc ,char*argv[]) //client
{
    if(argv[1] == std::string("-c"))
    {
        if(argc != 8)
        {
            std::cout<<"Error: missing or additional arguments"<<std::endl;
            return 0;
        }
        else if(atoi(argv[5])>65535 || atoi(argv[5])<1024)
        {
            std::cout<<"Error: port number must be in the range 1024 to 65535"<<std::endl;
            return 0;
        }

        int S_sockfd = 0;
        S_sockfd = socket(AF_INET ,SOCK_STREAM ,0);

        struct sockaddr_in S_info;
        S_info.sin_family = AF_INET;
        S_info.sin_addr.s_addr = inet_addr(argv[3]);
        S_info.sin_port = htons(atoi(argv[5]));

        time_t start_t ,waste_t;
        char send_Data[1000];
        memset(&send_Data ,'0' ,sizeof(send_Data));

        int count = 0;

        std::cout<<"connection state: ";
        int err = connect(S_sockfd ,(struct sockaddr *)&S_info ,sizeof(S_info));
        if(err != -1)   std::cout<<"success\n";
        else
        {
            std::cout<<"fail\n";
            return 0;
        }
        
        int previous ,check = 1;
        char ack_buffer[1] = {};
        std::cout<<"~~start send data~~\n\n";
        start_t = time(NULL);
        
        while(time(NULL)-start_t<=atoi(argv[7]))
        {

            if(check==0)    continue;
            
            if(time(NULL)-start_t > previous)
            {
                //std::cout<<"seconds: "<<previous<<std::endl;
                std::cout<<"running🚶...\n";
                previous = time(NULL)-start_t;
            }
            
            send(S_sockfd ,send_Data ,sizeof(send_Data), 0);
            count++; //number of Datas that client send
            check = recv(S_sockfd ,ack_buffer ,sizeof(ack_buffer) ,0);
        }
        
        waste_t = time(NULL) - start_t - 1;
        //std::cout<<waste_t<<std::endl;
        double rate_Mbps = count*0.008/waste_t;
        std::cout<<"\nevaluate result: "<<"send="<<count<<" KB rate=";
        std::cout<<std::fixed<<std::setprecision(3)<<rate_Mbps<<" Mbps"<<std::endl;

        return 0;

    }

    else if(argv[1] == std::string("-s"))   //server
    {
        if(argc != 4)
        {
            std::cout<<"Error: missing or additional arguments"<<std::endl;
            return 0;
        }
        else if(atoi(argv[3])>65535 || atoi(argv[3])<1024)
        {
            std::cout<<"Error: port number must be in the range 1024 to 65535"<<std::endl;
            return 0;
        }

        int S_sockfd = 0,C_sockfd;
        S_sockfd = socket(AF_INET ,SOCK_STREAM, 0);

        struct sockaddr_in S_info,C_info;
        memset(&S_info, 0 ,sizeof(S_info));
        S_info.sin_family = AF_INET;
        S_info.sin_addr.s_addr = INADDR_ANY;
        S_info.sin_port = htons(atoi(argv[3]));
        socklen_t addrlen = sizeof(C_info);

        bind(S_sockfd ,(struct sockaddr*)&S_info ,sizeof(S_info));
        listen(S_sockfd ,1);

        time_t start_t,end_t;
        char recv_Data[1000] = {};
        char ack[1] = {};
        memset(&ack ,'1' ,sizeof(ack));
        int numkbytes = 0;

        std::cout<<"~~Ready to serve~~\n";

        C_sockfd = accept(S_sockfd ,(struct sockaddr*)&C_info ,&addrlen);
        start_t = time(NULL);

        while(true)
        {
            int check = recv(C_sockfd ,recv_Data ,sizeof(recv_Data) ,0);
            numkbytes += check*0.001;
            send(C_sockfd ,ack ,sizeof(ack) ,0);
            std::cout<<"receive now: "<<numkbytes<<" kilobytes"<<std::endl;

            if(check == 0)
            {
                end_t = time(NULL);
                break;
            }
        }
        close(C_sockfd);
        close(S_sockfd);

        //std::cout<<(end_t - start_t)<<std::endl;
        double rate_Mbps = numkbytes*0.008/(end_t - start_t - 1);
        std::cout<<"\nevaluate result: "<<"receive="<<numkbytes<<" KB rate=";
        std::cout<<std::fixed<<std::setprecision(3)<<rate_Mbps<<" Mbps"<<std::endl;

        return 0;
        
    }
    
    else
    {
        std::cout<<"Error: invalid syntax"<<std::endl;
        return 0;
    }
}
