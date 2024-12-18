#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_PKT_NUM 255

/* Define control flags */
#define SYN 1
#define SYN_ACK 2
#define ACK 3
#define RST 4

/* Packet data structure */
typedef struct {
    int seq;      /* Sequence number */
    int ack;      /* Acknowledgement number */
    int flag;     /* Control flag */
    char payload; /* Data payload */
} Packet;

/* Read window size, byte count, and packet actions from input file */
int load_input(const char *filename, int *window_size, int *byte_request, int *buf, int max_bufsz) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    if (fscanf(file, "N %d, S %d", window_size, byte_request) != 2) {
        printf("Error reading window size and byte count\n");
        fclose(file);
        return -1;
    }

    int count = 0;
    while (fscanf(file, "%d", &buf[count]) != EOF && count < max_bufsz) {
        count++;
    }
    fclose(file);
    return count;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_port> <test_cases>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);
    int port = atoi(argv[1]);

    int packet_actions[MAX_PKT_NUM];
    int window_size, byte_request;
    int num_actions = load_input(argv[2], &window_size, &byte_request, packet_actions, MAX_PKT_NUM);

    if (num_actions < 0) {
        printf("No test case read from file\n");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port);

    Packet send_packet, recv_packet;

    /* Step 1: Perform the three-way handshake */
    send_packet.seq = 0;
    send_packet.ack = 0;
    send_packet.flag = SYN;
    sendto(sockfd, &send_packet, sizeof(Packet), 0, (struct sockaddr *)&server_addr, addrlen);
    printf("Sent SYN packet\n");

    while (1) {
        recvfrom(sockfd, &recv_packet, sizeof(Packet), 0, (struct sockaddr *)&server_addr, &addrlen);
        if (recv_packet.flag == SYN_ACK && recv_packet.ack == 0) {
            printf("Received SYN-ACK packet\n");
            break;
        }
        printf("Expected SYN-ACK packet! Received %d instead, ignoring\n", recv_packet.flag);
    }

    send_packet.seq = 1;
    send_packet.ack = recv_packet.seq;
    send_packet.flag = ACK;
    sendto(sockfd, &send_packet, sizeof(Packet), 0, (struct sockaddr *)&server_addr, addrlen);
    printf("Sent ACK packet, handshake complete\n");

    /* Step 2: Send window size (N) and byte count (S) */
    send_packet.seq++;
    send_packet.payload = window_size;
    sendto(sockfd, &send_packet, sizeof(Packet), 0, (struct sockaddr *)&server_addr, addrlen);
    printf("Sent window size (N) = %d\n", window_size);

    send_packet.seq++;
    send_packet.payload = byte_request;
    sendto(sockfd, &send_packet, sizeof(Packet), 0, (struct sockaddr *)&server_addr, addrlen);
    printf("Sent byte request (S) = %d\n", byte_request);

    /* Step 3: Start receiving packets from the server */
    int cur_ack = send_packet.ack;
    int good_count = 0, recv_count = 0;
    int conn_alive = 1, action_index = 0;

    while (conn_alive) {
        printf("(Waiting for seq %d) ", cur_ack + 1);
        recvfrom(sockfd, &recv_packet, sizeof(Packet), 0, (struct sockaddr *)&server_addr, &addrlen);
        recv_count++;

        printf("Received: seq=%d, ack=%d", recv_packet.seq, recv_packet.ack);

        if (recv_packet.flag == RST) {
            printf(" - RST, ending transmission\n");
            if (recv_count == num_actions) {
                printf("Test passed! Delivered %d packets successfully.\n", good_count+1);
            } else {
                printf("Test failed! Delivered %d out of %d packets.\n", good_count, byte_request);
            }
            conn_alive = 0;
            continue;
        }

        if (recv_packet.seq == cur_ack + 1) {
            printf(" - ACK, Matched\n");
            cur_ack++;
            good_count++;
            send_packet.seq++;
            send_packet.ack = cur_ack;
            send_packet.flag = ACK;
            sendto(sockfd, &send_packet, sizeof(Packet), 0, (struct sockaddr *)&server_addr, addrlen);

            if (good_count == byte_request) {
                printf("Test passed! Delivered %d packets successfully.\n", good_count);
                conn_alive = 0;
            }
        } else {
            printf(" - Unexpected packet, ignoring\n");
        }

        action_index++;
    }

    close(sockfd);
    return 0;
}
