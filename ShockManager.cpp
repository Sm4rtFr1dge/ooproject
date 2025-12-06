#ifndef SHOCKMANAGER_H
#define SHOCKMANAGER_H

#include <iostream>
#include <string>
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <unistd.h>     // UNIX standard function definitions (write, close)
#include <cstring>      // For strerror

class ShockManager {
private:
    int serial_fd; // File descriptor for the serial port
    bool connected;

public:
    // Constructor: Takes the port name ("/dev/ttyACM0" for COM3)
    ShockManager(const std::string& portName, int baudRate = B9600) {
        connected = false;

        // 1. Open the Port
        // O_RDWR: Read/Write access
        // O_NOCTTY: No terminal will control the process
        // O_NDELAY: Non-blocking open
        serial_fd = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

        if (serial_fd == -1) {
            std::cerr << "[ShockManager] Error opening " << portName << ": " << strerror(errno) << std::endl;
            std::cerr << "[ShockManager] Hint: Did you run 'sudo chmod 666 " << portName << "'?" << std::endl;
            return;
        }

        // 2. Configure the Port (termios)
        struct termios options;
        if (tcgetattr(serial_fd, &options) != 0) {
            std::cerr << "[ShockManager] Error getting current serial options: " << strerror(errno) << std::endl;
            // We continue anyway, as we might be able to set them from scratch
        }

        // Set Baud Rate
        cfsetispeed(&options, baudRate);
        cfsetospeed(&options, baudRate);

        // Control Flags
        options.c_cflag |= (CLOCAL | CREAD); // Enable receiver, ignore modem status lines
        options.c_cflag &= ~PARENB;          // No parity
        options.c_cflag &= ~CSTOPB;          // 1 stop bit
        options.c_cflag &= ~CSIZE;           // Mask the character size bits
        options.c_cflag |= CS8;              // 8 data bits

        // Input/Output Flags (Raw mode - essential for data transmission)
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Disable canonical mode, echo, signals
        options.c_iflag &= ~(IXON | IXOFF | IXANY);         // Disable software flow control
        options.c_oflag &= ~OPOST;                          // Disable output processing


        if (tcsetattr(serial_fd, TCSANOW, &options) != 0) {
            std::cerr << "[ShockManager] Warning: tcsetattr failed: " << strerror(errno) << std::endl;
            std::cerr << "[ShockManager] Attempting to continue anyway..." << std::endl;
        }

        // Clear any garbage data in buffer
        tcflush(serial_fd, TCIOFLUSH);

        // Reset file status flags to blocking mode for writing stability
        fcntl(serial_fd, F_SETFL, 0);

        connected = true;
        std::cout << "[ShockManager] Successfully connected to " << portName << std::endl;
    }

    ~ShockManager() {
        if (connected && serial_fd != -1) {
            close(serial_fd);
            std::cout << "[ShockManager] Port closed." << std::endl;
        }
    }

    void sendShock(int player) {
        if (!connected) {
            std::cerr << "[ShockManager] Cannot send: Serial port not connected." << std::endl;
            return;
        }

        char dataToSend = '0'; // Default safety

        if (player == 1) {
            dataToSend = '1';
        } else if (player == 2) {
            dataToSend = '2';
        } else {
            std::cerr << "[ShockManager] Invalid player ID" << std::endl;
            return;
        }

        // Write the byte to the serial port
        int bytesWritten = write(serial_fd, &dataToSend, 1);

        if (bytesWritten < 0) {
            std::cerr << "[ShockManager] Error writing to serial port: " << strerror(errno) << std::endl;
        } else {
            std::cout << ">>> SHOCK TRIGGERED! Player " << player << " (Sent: " << dataToSend << ") <<<" << std::endl;
        }
    }
};

#endif
