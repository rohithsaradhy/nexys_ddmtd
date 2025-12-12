#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#pragma pack(push, 1)
typedef struct {
    long long t;
    float tie_rise;
    float tie_fall;
} TIEdata;
#pragma pack(pop)

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1235);
    addr.sin_addr.s_addr = inet_addr("192.168.22.98/17");;
    // addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all interfaces


    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));



    FILE *file_pointer;
    char *filename = "farside_nexys4.txt";
    file_pointer = fopen(filename, "a");
    // Check if the file was opened successfully
    if (file_pointer == NULL) {
        perror("Error opening file");
        return 1; // Return an error code
    }

    
    
    TIEdata data;
    while(1) {
        recv(sockfd, &data, sizeof(TIEdata), 0);
        fprintf(file_pointer,"%lld, %0.4f,%0.4f\n",data.t,data.tie_rise*6.25/100000*1000,data.tie_fall*6.25/100000*1000);
        fflush(file_pointer);
        printf("Received: %lld, %.4f,%0.4f\n", data.t,data.tie_rise*6.25/100000*1000,data.tie_fall*6.25/100000*1000); //picoseconds
    }
    
    close(sockfd);
    fclose(file_pointer);
    return 0;
}
