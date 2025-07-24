#include <linux/gpio.h>
#include "gpio_1.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <thread>
#include <chrono>
#include <cerrno>

GPIO_Controller::GPIO_Controller() {
    gpio_chip_fd = open("/dev/gpiochip0", O_RDWR);
    if (gpio_chip_fd < 0) {
        std::cerr << "Failed to open GPIO device: " << strerror(errno) << std::endl;
        gpio_chip_fd = -1;
        return;
    }

    gpio_pins = {12, 13, 6, 20, 21, 26};

    for (int pin : gpio_pins) {
        gpio_write(pin, 0);
    }

    stop();
}

GPIO_Controller::~GPIO_Controller() {
    stop();
    if (gpio_chip_fd >= 0) {
        close(gpio_chip_fd);
    }
}

void GPIO_Controller::gpio_write(int offset, uint8_t value) {
    if (gpio_chip_fd < 0) {
        std::cerr << "GPIO device not available, cannot write to pin " << offset << std::endl;
        return;
    }

    struct gpiohandle_request req {};
    struct gpiohandle_data data {};

    strcpy(req.consumer_label, "robot_control");
    req.lineoffsets[0] = offset;
    req.lines = 1;
    req.flags = GPIOHANDLE_REQUEST_OUTPUT;

    if (ioctl(gpio_chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &req) < 0) {
        std::cerr << "Error setting line handle for pin " << offset << ": " << strerror(errno) << std::endl;
        return;
    }

    data.values[0] = value;

    if (ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
        std::cerr << "Error setting line value: " << strerror(errno) << std::endl;
    }

    close(req.fd);
}

void GPIO_Controller::forward(int time_ms) {
    std::cout << "Moving forward for " << time_ms << " ms..." << std::endl;

    gpio_write(6, 1);
    gpio_write(26, 1);
    gpio_write(12, 0);
    gpio_write(13, 1);
    gpio_write(20, 1);
    gpio_write(21, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
    stop();
}

void GPIO_Controller::stop() {
    std::cout << "Stopping..." << std::endl;

    gpio_write(6, 1);
    gpio_write(26, 1);
    gpio_write(12, 0);
    gpio_write(13, 0);
    gpio_write(20, 0);
    gpio_write(21, 0);
}

void GPIO_Controller::right(int time_ms) {
    std::cout << "Turning right for " << time_ms << " ms..." << std::endl;

    gpio_write(6, 1);
    gpio_write(26, 1);
    gpio_write(12, 0);
    gpio_write(13, 0);
    gpio_write(20, 1);
    gpio_write(21, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
    stop();
}

void GPIO_Controller::left(int time_ms) {
    std::cout << "Turning left for " << time_ms << " ms..." << std::endl;

    gpio_write(6, 1);
    gpio_write(26, 1);
    gpio_write(12, 0);
    gpio_write(13, 1);
    gpio_write(20, 0);
    gpio_write(21, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
    stop();
}
