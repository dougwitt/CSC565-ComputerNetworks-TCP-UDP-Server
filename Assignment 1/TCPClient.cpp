#include <iostream> // Include for input/output streaming.
#include <string>   // Include for string manipulation.
#include <cstring>  // Include for string manipulation in C style.
#include <sys/socket.h> // Include for socket programming.
#include <arpa/inet.h>  // Include for internet operations, e.g., htonl.
#include <unistd.h>     // Include for POSIX operating system API, e.g., read/write.
#include <netdb.h>      // Include for network database operations, e.g., gethostbyname.

const int BUFFER_SIZE = 1024; // Define the buffer size for message exchange.

// Main function with command-line arguments for server address, loan amount, term in years, and interest rate.
int main(int argc, char* argv[]) {
    // Ensure correct number of arguments.
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <server_address> <loan_amount> <term_years> <interest_rate>" << std::endl;
        return 1; // Return error if arguments are incorrect.
    }

    // Parsing command-line arguments.
    std::string serverAddress = argv[1];
    double loanAmount = std::stod(argv[2]); // Convert string to double for loan amount.
    int termYears = std::stoi(argv[3]); // Convert string to integer for term in years.
    double interestRate = std::stod(argv[4]); // Convert string to double for interest rate.

    int clientSocket;
    sockaddr_in serverSocketAddress;
    hostent* server;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Resolve server address.
    server = gethostbyname(serverAddress.c_str());
    if (server == nullptr) {
        std::cerr << "Error: Unable to resolve server address" << std::endl;
        close(clientSocket);
        return 1;
    }

    // Prepare the sockaddr_in structure.
    memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_port = htons(13000); // Server port number.
    memcpy(&serverSocketAddress.sin_addr, server->h_addr, server->h_length);

    // Connect to the server.
    if (connect(clientSocket, (sockaddr*)&serverSocketAddress, sizeof(serverSocketAddress)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        close(clientSocket);
        return 1;
    }

    // Prepare the request message.
    std::string requestMessage = std::to_string(loanAmount) + " " +
                                 std::to_string(termYears) + " " +
                                 std::to_string(interestRate);

    // Send the request message to the server.
    ssize_t bytesSent = write(clientSocket, requestMessage.c_str(), requestMessage.length());
    if (bytesSent < 0) {
        std::cerr << "Error sending request" << std::endl;
        close(clientSocket);
        return 1;
    }

    // Read the response message from the server.
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1);
    if (bytesRead < 0) {
        std::cerr << "Error receiving response" << std::endl;
        close(clientSocket);
        return 1;
    }

    // Null-terminate the received data and print.
    buffer[bytesRead] = '\0';
    std::cout << "Server response: " << buffer << std::endl;

    close(clientSocket); // Close the socket.
    return 0; // Exit the program.
}
