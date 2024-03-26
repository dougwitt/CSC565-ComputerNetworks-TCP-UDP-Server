#include <iostream> // For input and output operations.
#include <string>   // For using the string class.
#include <cstring>  // For using C-style string functions like memset.
#include <sys/socket.h> // For socket operations.
#include <arpa/inet.h>  // For internet operations, converting addresses.
#include <unistd.h> // For read, write, and close operations on file descriptors.

const int BUFFER_SIZE = 1024; // Define a constant for the buffer size.

int main(int argc, char* argv[]) {
    // Ensure the correct number of arguments are passed.
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <loan_amount> <term_years> <interest_rate>" << std::endl;
        return 1;
    }

    // Parse command line arguments.
    std::string serverIP = argv[1]; // Server IP address.
    double loanAmount = std::stod(argv[2]); // Loan amount.
    int termYears = std::stoi(argv[3]); // Loan term in years.
    double interestRate = std::stod(argv[4]); // Annual interest rate.

    int clientSocket; // Socket descriptor for the client.
    sockaddr_in serverAddress; // Structure for storing the server address.
    memset(&serverAddress, 0, sizeof(serverAddress)); // Initialize the serverAddress structure with zeros.
    char buffer[BUFFER_SIZE]; // Buffer for storing server responses.

    // Create socket.
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Prepare the sockaddr_in structure.
    serverAddress.sin_family = AF_INET; // Address family (IPv4).
    serverAddress.sin_port = htons(13000); // Server port, converted to network byte order.
    // Convert IP address from text to binary form.
    if (inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "Invalid address or address not supported" << std::endl;
        return 1;
    }

    // Connect to the server.
    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Connection failed" << std::endl;
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
    ssize_t bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1);
    if (bytesRead < 0) {
        std::cerr << "Error receiving response" << std::endl;
        close(clientSocket);
        return 1;
    }

    // Null-terminate the received data.
    buffer[bytesRead] = '\0';

    // Process the response message.
    std::string responseMessage(buffer);
    std::string delimiter = " ";
    size_t pos = responseMessage.find(delimiter);

    // Handle the server's response.
    if (pos == std::string::npos) {
        std::cerr << "Invalid response format" << std::endl;
    } else {
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
            std::cerr << "Error: " << responseMessage << std::endl;
        } else {
            std::cerr << "Invalid response status" << std::endl;
        }
    }

    // Close the socket before terminating the program.
    close(clientSocket);
   

    return 0;
}
