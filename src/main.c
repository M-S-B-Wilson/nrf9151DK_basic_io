#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

// Get node identifiers for the LEDs
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#define BUTTON0_NODE DT_ALIAS(button0)
#define BUTTON1_NODE DT_ALIAS(button1)
#define BUTTON2_NODE DT_ALIAS(button2)
#define BUTTON3_NODE DT_ALIAS(button3)

// Check if the LED nodes are defined


// Macro to simplify LED config structure
#define DEFINE_LED(node) \
    { \
        .dev = DEVICE_DT_GET(DT_GPIO_CTLR(node, gpios)), \
        .pin = DT_GPIO_PIN(node, gpios), \
        .flags = DT_GPIO_FLAGS(node, gpios) \
    }

struct led {
    const struct device *dev;
    gpio_pin_t pin;
    gpio_flags_t flags;
};

#define DEFINE_BUTTON(node) \
    { \
        .dev = DEVICE_DT_GET(DT_GPIO_CTLR(node, gpios)), \
        .pin = DT_GPIO_PIN(node, gpios), \
        .flags = DT_GPIO_FLAGS(node, gpios) \
    }

struct button {
    const struct device *dev;
    gpio_pin_t pin;
    gpio_flags_t flags;
};

int main(void)
{
    printk("nrf9151dk_basic_io: Hello, World!\n");
    struct button buttons[4] = {
        DEFINE_BUTTON(BUTTON0_NODE),
        DEFINE_BUTTON(BUTTON1_NODE),
        DEFINE_BUTTON(BUTTON2_NODE),
        DEFINE_BUTTON(BUTTON3_NODE),
    };  
    struct led leds[4] = {
        DEFINE_LED(LED0_NODE),
        DEFINE_LED(LED1_NODE),
        DEFINE_LED(LED2_NODE),
        DEFINE_LED(LED3_NODE),
    };

    for (int i = 0; i < 4; i++) {
        if (!device_is_ready(leds[i].dev)) {
            printk("LED %d device not ready\n", i);
            return 0;
        }

        gpio_pin_configure(leds[i].dev, leds[i].pin, GPIO_OUTPUT_ACTIVE | leds[i].flags);
        gpio_pin_configure(buttons[i].dev, buttons[i].pin, GPIO_INPUT | buttons[i].flags);
    }

    while (1) {
/*        printk("\nLEDs ");
        for (int i = 0; i < 4; i++) {
            // Turn all off first
            for (int j = 0; j < 4; j++) {
                gpio_pin_set(leds[j].dev, leds[j].pin, 0);
            }

            // Turn one on
            printk(" %d ", i);
            gpio_pin_set(leds[i].dev, leds[i].pin, 1);

            //k_sleep(K_SECONDS(1));
            k_sleep(K_MSEC(1000));
//            printk("LED %d off\n", i);
            gpio_pin_set(leds[i].dev, leds[i].pin, 0);

        }    
*/            
        for (int i = 0; i < 4; i++) {
            if (gpio_pin_get(buttons[i].dev, buttons[i].pin)) {
                printk("Button %d pressed\n", i);
                gpio_pin_set(leds[i].dev, leds[i].pin, 1);
            } else {
                gpio_pin_set(leds[i].dev, leds[i].pin, 0);
            }
        }    

    }
    return 0;
}
