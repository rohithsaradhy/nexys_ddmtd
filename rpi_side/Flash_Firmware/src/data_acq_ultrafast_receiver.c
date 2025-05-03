#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#pragma pack(push, 1)
typedef struct {
    long long t;
    float tie;
} TIEdata;
#pragma pack(pop)

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = inet_addr("192.168.22.98/17");;
    // addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all interfaces


    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    TIEdata data;
    while(1) {
        recv(sockfd, &data, sizeof(TIEdata), 0);
        printf("Received: %lld, %.2f\n", data.t,data.tie*6.25*/100000*1000-11); //picoseconds
    }
    
    close(sockfd);
    return 0;
}