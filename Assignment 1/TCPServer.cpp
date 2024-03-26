#include <iostream>      // Include for input/output streaming.
#include <cstring>       // Include for memory manipulation functions.
#include <unistd.h>      // Include for POSIX operating system API, e.g., read, write, close.
#include <netinet/in.h>  // Include for Internet addresses and htons function.

const int PORT = 13000;          // Server port number.
const int BUFFER_SIZE = 1024;    // Size of buffer for incoming messages.

// Function to calculate monthly payment based on loan amount, term, and interest rate.
double calculateMonthlyPayment(double loanAmount, int termYears, double interestRate) {
    double monthlyInterestRate = interestRate / 12 / 100;  // Convert annual interest rate to a monthly fraction.
    int totalPayments = termYears * 12;                    // Total number of monthly payments.
    // Calculate monthly payment using the given formula.
    return loanAmount * monthlyInterestRate / (1 - std::pow(1 + monthlyInterestRate, -totalPayments));
}

int main() {
    int server_fd, new_socket;               // Socket file descriptors.
    struct sockaddr_in address;              // Structure for storing address information.
    int opt = 1;                             // Option for setsockopt to reuse the address/port.
    int addrlen = sizeof(address);           // Length of address structure.
    char buffer[BUFFER_SIZE] = {0};          // Buffer for storing incoming messages.
    
    // Create a socket file descriptor for the server.
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");             // Print error message if socket creation fails.
        exit(EXIT_FAILURE);
    }
    
    // Attach socket to the server port.
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");                // Print error message if setsockopt fails.
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;            // Address family (IPv4).
    address.sin_addr.s_addr = INADDR_ANY;    // Accept connections on any IP.
    address.sin_port = htons(PORT);          // Convert port number to network byte order.
    
    // Bind the socket to the specified port.
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");               // Print error message if binding fails.
        exit(EXIT_FAILURE);
    }
    // Listen for incoming connections.
    if (listen(server_fd, 3) < 0) {
        perror("listen");                    // Print error message if listen fails.
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while(true) {
        // Accept a new connection.
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");                // Print error message if accept fails.
            exit(EXIT_FAILURE);
        }
        
        // Read the loan calculation request from client.
        read(new_socket, buffer, BUFFER_SIZE);
        double loanAmount;
        int termYears;
        double interestRate;
        // Parse the loan details from the request.
        sscanf(buffer, "%lf %d %lf", &loanAmount, &termYears, &interestRate);
        
        // Calculate the monthly and total payments.
        double monthlyPayment = calculateMonthlyPayment(loanAmount, termYears, interestRate);
        double totalPayment = monthlyPayment * termYears * 12;
        
        // Send the calculated payments back to the client.
        char response[BUFFER_SIZE];
        snprintf(response, BUFFER_SIZE, "Monthly Payment: $%.2f, Total Payment: $%.2f", monthlyPayment, totalPayment);
        write(new_socket, response, strlen(response));
        
        close(new_socket);  // Close the connection to the client.
    }

    return 0;
}
