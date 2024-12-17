/* UCR CS 164 Go-Back-N Project
 * UDP server sample
 * Implements the server logic for the client-server interaction
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h> // For timeout handling

#define MAX_PKT_NUM 255
#define SYN 1
#define SYN_ACK 2
#define ACK 3
#define RST 4

typedef struct {
    int seq;      /* Sequence number */
    int ack;      /* Acknowledgement number */
    int flag;     /* Control flag */
    char payload; /* Data payload */
} Packet;

int load_actions(const char *filename, int *actions, int max_actions) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening test case file");
        return -1;
    }

    int window_size, byte_request;
    if (fscanf(file, "N %d, S %d", &window_size, &byte_request) != 2) {
        printf("Error reading window size and byte count\n");
        fclose(file);
        return -1;
    }

    int count = 0;
    while (count < max_actions && fscanf(file, "%d", &actions[count]) != EOF) {
        count++;
    }
    fclose(file);
    return count;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <port> <test_case_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int port = atoi(argv[1]);

    int packet_actions[MAX_PKT_NUM];
    int action_count = load_actions(argv[2], packet_actions, MAX_PKT_NUM);
    if (action_count < 0) {
        fprintf(stderr, "Failed to load test case file\n");
        exit(EXIT_FAILURE);
    }

    FILE *log_file; // Log file for corrupted packets
    log_file = fopen("corrupted_packets.log", "w");
    if (!log_file) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error on binding");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server running on port %d\n", port);

    Packet recv_packet, send_packet;
    int cur_seq = 1; // Start sequence number for server data packets
    int cur_ack = 3; // Start ack at 3 after handshake and setup

    /* Step 1: Three-Way Handshake */
    while (1) {
        recvfrom(sockfd, &recv_packet, sizeof(Packet), 0, 
                 (struct sockaddr *)&client_addr, &addrlen);

        if (recv_packet.flag == SYN) {
            printf("Received SYN packet\n");
            send_packet.seq = cur_seq;
            send_packet.ack = 0;
            send_packet.flag = SYN_ACK;
            sendto(sockfd, &send_packet, sizeof(Packet), 0, 
                   (struct sockaddr *)&client_addr, addrlen);
            printf("Sent SYN-ACK packet\n");
        } else if (recv_packet.flag == ACK) {
            printf("Received ACK packet, handshake complete\n");
            break;
        }
    }

    /* Step 2: Receive window size (N) and total packets (S) */
    int window_size, total_packets;
    recvfrom(sockfd, &recv_packet, sizeof(Packet), 0, (struct sockaddr *)&client_addr, &addrlen);
    window_size = (int)recv_packet.payload;
    printf("Received window size (N) = %d\n", window_size);

    recvfrom(sockfd, &recv_packet, sizeof(Packet), 0, (struct sockaddr *)&client_addr, &addrlen);
    total_packets = (int)recv_packet.payload;
    printf("Received total packet count (S) = %d\n", total_packets);

    /* Step 3: Go-Back-N ARQ Implementation */
    int base = 1;
    int next_seq = 1;
    int action_index = 0;
    int retries = 0;
    const int max_retries = 3;
    fd_set readfds;
    struct timeval timeout;

    while (base <= total_packets) {
        // Send packets within the window
        while (next_seq < base + window_size && next_seq <= total_packets) {
            send_packet.seq = next_seq;
            send_packet.ack = cur_ack;
            send_packet.flag = ACK;
            send_packet.payload = 'A' + ((next_seq - 1) % 26);

            int action = (action_index < action_count) ? packet_actions[action_index++] : 0;

            if (action == 1) { // Simulate timeout
                printf("Simulating timeout for seq=%d\n", next_seq);
                retries++;
                if (retries >= max_retries) {
                    printf("Max retries reached for seq=%d. Sliding window forward.\n", base);
                    base++;
                    retries = 0;
                }
                break;
            } else if (action == 2) { // Simulate corrupted packet
                printf("Simulating corrupted packet seq=%d\n", next_seq);
                fprintf(log_file, "Corrupted packet: seq=%d, payload=%c\n", next_seq, send_packet.payload);
                fflush(log_file);
                send_packet.payload = '?'; // Corrupt the payload
                sendto(sockfd, &send_packet, sizeof(Packet), 0, 
                       (struct sockaddr *)&client_addr, addrlen);
                printf("Sent corrupted packet seq=%d\n", next_seq);
                next_seq++; // Move to the next sequence number
                continue;
            }

            sendto(sockfd, &send_packet, sizeof(Packet), 0, 
                   (struct sockaddr *)&client_addr, addrlen);
            printf("Sent packet seq=%d, ack=%d\n", next_seq, send_packet.ack);
            next_seq++;
        }

        // Wait for ACKs with timeout
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        int activity = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
        if (activity == 0) { // Timeout occurred
            printf("Timeout! Resending packets starting from seq=%d\n", base);
            next_seq = base;
            retries++;
            if (retries >= max_retries) {
                printf("Max retries reached for seq=%d. Sliding window forward.\n", base);
                base++;
                retries = 0;
            }
        } else {
            recvfrom(sockfd, &recv_packet, sizeof(Packet), 0, 
                     (struct sockaddr *)&client_addr, &addrlen);
            if (recv_packet.flag == ACK) {
                printf("Received ACK for seq=%d\n", recv_packet.ack);
                if (recv_packet.ack >= base) {
                    base = recv_packet.ack + 1;
                    retries = 0;
                }
            }
        }
    }

    /* Step 4: Send RST to terminate connection */
    send_packet.seq = cur_seq;
    send_packet.ack = cur_ack;
    send_packet.flag = RST;
    sendto(sockfd, &send_packet, sizeof(Packet), 0, 
           (struct sockaddr *)&client_addr, addrlen);
    printf("Sent RST packet, closing connection\n");

    fclose(log_file);
    close(sockfd);
    return 0;
}
