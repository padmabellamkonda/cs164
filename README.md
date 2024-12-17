# Go-Back-N ARQ UDP Server

## Overview

This program implements the **Go-Back-N Automatic Repeat reQuest (ARQ)** protocol as part of a client-server interaction project for **UCR CS 164**. The server simulates reliable packet delivery using UDP, supporting:

- A **three-way handshake** to establish the connection.
- Go-Back-N ARQ for packet transmission with a sliding window mechanism.
- Simulation of **timeouts** and **corrupted packets** based on an input test case file.
- Logging of corrupted packets to a file named `corrupted_packets.log`.

---

## Program Features

1. **Three-Way Handshake**: Ensures the server and client establish a connection before data transfer begins.
2. **Sliding Window**: Implements the Go-Back-N ARQ sliding window for packet management.
3. **Timeout Simulation**: Simulates timeouts for specified packets in the test case file.
4. **Corruption Simulation**: Simulates corrupted packets and logs them.
5. **Graceful Termination**: Sends an RST packet to cleanly terminate the connection.

---

## Prerequisites

Ensure the following tools are installed on your system:

- GCC Compiler (for compiling C programs)
- A Unix-based system (Linux/Mac) or Cygwin on Windows
- Basic knowledge of networking and UDP

---

## File Structure

- **udp_server.c**: The main server program implementing the Go-Back-N ARQ logic.
- **test_case.txt**: Sample test case files to simulate timeout and corrupted packets.
- **corrupted_packets.log**: A file where corrupted packets are logged.

---

## Compilation Instructions

To compile the program, use the **GCC** compiler:

```bash
gcc -o udp_server udp_server.c
