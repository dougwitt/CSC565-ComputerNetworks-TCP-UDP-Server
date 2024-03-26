# Socket Programming – Assignment 1
###Personal Information
Name: Douglas Witt
Email: dw673s@missouristate.edu

###Course Details
Class Name: Computer Networks
Date: 03/25/2024

###Files Submitted
TCPClient.cpp: Implements the TCP client that sends loan calculation requests to the TCP server.
TCPServer.cpp: Implements the TCP server that calculates loan payments and responds to the TCP client.
UDPClient.cpp (Optional): Implements the UDP client for sending loan calculation requests to the UDP server.
UDPServer.cpp (Optional): Implements the UDP server for calculating loan payments and responding to UDP client requests.
Compilation and Running Instructions
Ensure you have a C++ compiler installed (e.g., GCC). Follow these instructions to compile and run the TCP and UDP versions of the client and server programs.

###TCP Server

#### Compiling the TCP Server
g++ TCPServer.cpp -o TCPServer

#### Running the TCP Server
./TCPServer

###TCP Client
Replace <server_address>, <loan_amount>, <term_years>, and <interest_rate> with actual values.

# Compiling the TCP Client
g++ TCPClient.cpp -o Cal

# Running the TCP Client
./Cal <server_address> <loan_amount> <term_years> <interest_rate>
UDP Versions (Optional)
Similar instructions apply for compiling and running UDPClient and UDPServer, if applicable.

# Compiling and running UDP Server
g++ UDPServer.cpp -o UDPServer
./UDPServer

# Compiling and running UDP Client
g++ UDPClient.cpp -o UDPClient
./UDPClient <server_address> <loan_amount> <term_years> <interest_rate>
Application Protocol Description
Request Format
The client sends a request in the format: <loan_amount> <term_years> <interest_rate>.

Response Format
The server responds with: Monthly Payment: $<monthly_payment>, Total Payment: $<total_payment>.

Known Bugs and Limitations
The server does not validate input formats; incorrect formats may lead to undefined behavior.
UDP implementations lack packet loss handling or retries.
The server is not designed for concurrency and handles one request at a time.
Additional Notes
This project demonstrates basic socket programming skills for TCP and UDP communications in a client-server architecture, highlighting fundamental network programming concepts.
