# cs164
CS164 Final Project
Overview
This program implements the Go-Back-N Automatic Repeat reQuest (ARQ) protocol as part of a client-server interaction project for UCR CS 164. The server simulates reliable packet delivery using UDP, supporting:

A three-way handshake to establish the connection.
Go-Back-N ARQ for packet transmission with a sliding window mechanism.
Simulation of timeouts and corrupted packets based on an input test case file.
Logging of corrupted packets to a file named corrupted_packets.log.
Program Features
Three-Way Handshake: Ensures the server and client establish a connection before data transfer begins.
Sliding Window: Implements the Go-Back-N ARQ sliding window for packet management.
Timeout Simulation: Simulates timeouts for specified packets in the test case file.
Corruption Simulation: Simulates corrupted packets and logs them.
Graceful Termination: Sends an RST packet to cleanly terminate the connection.
Prerequisites
Ensure the following tools are installed on your system:

GCC Compiler (for compiling C programs)
A Unix-based system (Linux/Mac) or Cygwin on Windows
Basic knowledge of networking and UDP
File Structure
udp_server.c: The main server program implementing the Go-Back-N ARQ logic.
test_case.txt: Sample test case files to simulate timeout and corrupted packets.
corrupted_packets.log: A file where corrupted packets are logged.
Compilation Instructions
To compile the program, use the GCC compiler:

bash
Copy code
gcc -o udp_server udp_server.c
If the compilation is successful, an executable file named udp_server will be created.

Running the Program
Run the UDP Server program with the required arguments:
bash
Copy code
./udp_server <port> <test_case_file>
port: The port number on which the server will listen (e.g., 1064).
test_case_file: A file specifying actions for packet simulation (timeouts or corruption).
Example:
bash
Copy code
./udp_server 1064 test_5.txt
Test Case File Format
The test case file specifies:

Window size (N) and total number of packets (S).
Actions for each packet (0, 1, or 2).
File Example:
mathematica
Copy code
N 4, S 4
0
0
2
2
0
0
0
0: Normal packet transmission.
1: Simulate timeout (packet is skipped).
2: Simulate corruption (packet is sent with invalid payload and logged).
Program Output
The server will display the following in the terminal:

Sent and received packet details.
Simulated timeouts.
Simulated corrupted packets.
Logs any corrupted packets to corrupted_packets.log.
Sample Output:
bash
Copy code
Server running on port 1064
Received SYN packet
Sent SYN-ACK packet
Received ACK packet, handshake complete
Received window size (N) = 4
Received total packet count (S) = 4
Sent packet seq=1, ack=3
Sent packet seq=2, ack=3
Simulating corrupted packet seq=3
Sent corrupted packet seq=3
Simulating corrupted packet seq=4
Sent corrupted packet seq=4
Received ACK for seq=1
Timeout! Resending packets starting from seq=3
Sent packet seq=3, ack=3
Sent packet seq=4, ack=3
Received ACK for seq=4
Sent RST packet, closing connection
Logs for Corrupted Packets
When corrupted packets are simulated (action == 2), they are logged to the file corrupted_packets.log:

Example Log:

bash
Copy code
Corrupted packet: seq=3, payload=?
Corrupted packet: seq=4, payload=?
Notes
The program assumes a correctly formatted test case file.
Use port numbers greater than 1024 to avoid requiring root permissions.
The server must be started before the client for proper functionality.
Cleanup
After running the server, you can remove the compiled executable and logs:

bash
Copy code
rm udp_server corrupted_packets.log
