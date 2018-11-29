#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int sock;
    struct sockaddr_in addr;
    int yes = 1;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes));
    sendto(sock, "HELLO", 5, 0, (struct sockaddr *)&addr, sizeof(addr));

    close(sock);

    return 0;
}
