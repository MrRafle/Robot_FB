#ifndef GPIO_CONTROLLER_1_H_
#define GPIO_CONTROLLER_1_H_

#include <string>
#include <vector>
#include <linux/gpio.h>

class GPIO_Controller {
public:
    GPIO_Controller();
    ~GPIO_Controller();

    void forward(int time_ms);
    void stop();
    void right(int time_ms);
    void left(int time_ms);

private:
    int gpio_chip_fd;
    std::vector<int> gpio_pins;

    void gpio_write(int offset, uint8_t value);
};

#endif // _GPIO_H_
