#include <iostream>     // For input and output operations
#include <string>       // For string manipulation
#include <cstring>      // For memory management functions such as memset
#include <sys/socket.h> // For socket programming interfaces
#include <arpa/inet.h>  // For definitions of internet operations (e.g., inet_pton)
#include <unistd.h>     // For POSIX operating system API (e.g., close)

const int BUFFER_SIZE = 1024; // Buffer size for reading server responses
const int MAX_RETRIES = 3;    // Maximum number of retries for sending requests
const int TIMEOUT_SEC = 2;    // Socket timeout in seconds

int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <loan_amount> <term_years> <interest_rate>" << std::endl;
        return 1;
    }

    // Parse command line arguments
    std::string serverIP = argv[1];           // Server IP address
    double loanAmount = std::stod(argv[2]);   // Loan amount
    int termYears = std::stoi(argv[3]);       // Loan term in years
    double interestRate = std::stod(argv[4]); // Annual interest rate

    int clientSocket;                          // Socket file descriptor
    sockaddr_in serverAddress;                 // Server address structure
    memset(&serverAddress, 0, sizeof(serverAddress)); // Zero out the structure
    char buffer[BUFFER_SIZE];                  // Buffer for server response

    // Create a UDP socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Set socket timeout for receiving
    timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC; // Timeout in seconds
    timeout.tv_usec = 0;          // No microseconds
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        std::cerr << "Error setting socket timeout" << std::endl;
        close(clientSocket); // Close the socket before exiting
        return 1;
    }

    // Prepare the server address structure
    serverAddress.sin_family = AF_INET;       // Internet address family
    serverAddress.sin_port = htons(13000);    // Server port, converted to network byte order
    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "Invalid address or address not supported" << std::endl;
        close(clientSocket); // Close the socket before exiting
        return 1;
    }

    // Prepare the request message
    std::string requestMessage = std::to_string(loanAmount) + " " +
                                 std::to_string(termYears) + " " +
                                 std::to_string(interestRate);

    // Attempt to send the request message to the server with retries
    int retries = 0;
    while (retries < MAX_RETRIES) {
        ssize_t bytesSent = sendto(clientSocket, requestMessage.c_str(), requestMessage.length(), 0,
                                   (sockaddr*)&serverAddress, sizeof(serverAddress));
        if (bytesSent < 0) {
            std::cerr << "Error sending request" << std::endl;
            close(clientSocket); // Close the socket before exiting
            return 1;
        }

        // Attempt to receive the response from the server
        memset(buffer, 0, sizeof(buffer)); // Zero out the buffer
        socklen_t serverAddressLength = sizeof(serverAddress);
        ssize_t bytesRead = recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0,
                                     (sockaddr*)&serverAddress, &serverAddressLength);

        // Process the received response
        if (bytesRead > 0) {
            std::string responseMessage(buffer, bytesRead); // Construct a string from buffer
            std::string delimiter = " ";
            size_t pos = responseMessage.find(delimiter);

            if (pos == std::string::npos) {
                std::cerr << "Invalid response format" << std::endl;
            } else {
                // Extract and display the monthly payment and total payment
                std::string status = responseMessage.substr(0, pos);
                responseMessage.erase(0, pos + delimiter.length());

                if (status == "OK") {
                    pos = responseMessage.find(delimiter);
                    if (pos != std::string::npos) {
                        std::string monthlyPayment = responseMessage.substr(0, pos);
                        std::string totalPayment = responseMessage.substr(pos + delimiter.length());
                        std::cout << "Monthly payment: $" << monthlyPayment << std::endl;
                        std::cout << "Total payment: $" << totalPayment << std::endl;
                    } else {
                        std::cerr << "Invalid response format" << std::endl;
                    }
                } else if (status == "ERROR") {
                    // Handle any errors reported by the server
                    std::cerr << "Error: " << responseMessage << std::endl;
                } else {
                    // Handle unexpected status
                    std::cerr << "Invalid response status" << std::endl;
                }
            }
            break; // Exit loop after receiving a response
        } else {
            // If no response, indicate retry
            std::cerr << "No response received from the server. Retrying..." << std::endl;
            retries++;
        }
    }

    // If maximum retries reached without response, indicate failure
    if (retries == MAX_RETRIES) {
        std::cerr << "Maximum retries reached. Unable to get a response from the server." << std::endl;
    }

    // Close the socket before exiting
    close(clientSocket);
    return 0; // Successful program termination
}
