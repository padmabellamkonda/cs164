CS 164 Project: Go-Back-N

Fall 2024 | Submit by 12/17/24 | Demo by 12/17/24

Overview

This project implements a UDP-based file transfer server that mimics TCP's reliability features over an unreliable channel. It uses the Go-Back-N Automatic Repeat reQuest (ARQ) algorithm to ensure reliable packet delivery. The server communicates with a provided client program to:

Establish a connection using a three-way handshake.

Negotiate transmission parameters such as window size (N) and total byte count (S).

Transmit data using Go-Back-N ARQ to handle packet loss and corruption.

Compiling the Server

Open a terminal and navigate to the directory containing udp_server.c.

Compile the server using the GCC compiler:

gcc -o udp_server udp_server.c

Ensure the client program (provided by the TA) is also compiled if not already.

Compiling the Client

Open a terminal and navigate to the directory containing udp_client.c.

Compile the client using the GCC compiler:

gcc -o udp_client udp_client.c

Running the Server

Start the server, specifying a port number and a test case file for the client. For example:

./udp_server 1064 test_case.txt

Port number: The server listens on the specified port (e.g., 1064).

Test case file: Contains client simulation parameters such as window size (N) and byte count (S).

The server will output logs indicating its progress and interactions with the client.

Running the Client

The client program is provided by the TA. Run the client as follows:

./udp_client {PORT_NUMBER} test_case.txt

Replace {PORT_NUMBER} with the port number used by the server (e.g., 1064).

Replace test_case.txt with the test input file.

Functionality Overview

The Go-Back-N project demonstrates reliable data transfer over an unreliable network using the following features:

Three-Way Handshake: Establishes a connection between the client and server using SYN, SYN-ACK, and ACK packets.

Negotiation: The client specifies the maximum window size (N) and total byte count (S) to be transmitted.

Go-Back-N ARQ Protocol:

Implements a sliding window to manage data transmission.

Handles packet loss and corruption by retransmitting unacknowledged packets.

Dynamically adjusts the window size based on network conditions.

Connection Termination: Sends an RST packet to indicate the end of the transmission.

Notes

Test Case File Format:

Specifies window size (N), total byte count (S), and actions (e.g., packet loss or corruption).

Example:

N 2, S 4
1 0 0 0

Debugging:

Use server and client logs to trace packet flows and identify issues.

Timeout Handling:

The server detects unacknowledged packets using a timeout and retransmits them.

Relevant Notes

Ensure the port number matches between the server and client during execution.

The project emphasizes the server's behavior, particularly handling packet loss and window adjustments.

Review Chapter 3 slides on Go-Back-N for additional details.
