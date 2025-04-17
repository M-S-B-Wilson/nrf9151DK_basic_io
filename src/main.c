#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>


#define CONFIG_MODBUS_BUFFER_SIZE 64
LOG_MODULE_REGISTER(uart_async);
//#define UART_DEVICE_NODE DT_NODELABEL(uart2) // Use UART2
const struct device *uart_dev;

static uint8_t tx_buffer[] = "Hello, UART_ASYNC_API with DMA!";
static uint8_t rx_buffer[64]; // Buffer for receiving data


// Get node identifiers for the LEDs
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
//#define LED3_NODE DT_ALIAS(led3)

#define BUTTON0_NODE DT_ALIAS(sw0)
#define BUTTON1_NODE DT_ALIAS(sw1)
#define BUTTON2_NODE DT_ALIAS(sw2)
//#define BUTTON3_NODE DT_ALIAS(sw3)

// --- Direction Control Nodes ---
#define RS4851_NODE DT_NODELABEL(RS485_tx_enable_1)
#define RS4852_NODE DT_NODELABEL(RS485_tx_enable_2)

// Extract GPIO device, pin, and flags for RS485-1 TX enable
#define TXEN1_NODE DT_NODELABEL(rs485_tx_enable_1)
#define TXEN1_PIN DT_GPIO_PIN(TXEN1_NODE, gpios)
#define TXEN1_FLAGS DT_GPIO_FLAGS(TXEN1_NODE, gpios)

// Extract GPIO device, pin, and flags for RS485-2 TX enable
#define TXEN2_NODE DT_NODELABEL(rs485_tx_enable_2)
#define TXEN2_PIN DT_GPIO_PIN(TXEN2_NODE, gpios)
#define TXEN2_FLAGS DT_GPIO_FLAGS(TXEN2_NODE, gpios)

// --- UART Devices ---
const struct device *rs485_1; // RS485 port 1
const struct device *rs485_2; // RS485 port 2
const struct device *txen_1; // TX enable pin for RS485 port 1
const struct device *txen_2; // TX enable pin for RS485 port 2

void uart_callback(const struct device *dev, struct uart_event *evt, void *user_data) {
    switch (evt->type) {
    case UART_TX_DONE:
        LOG_INF("Transmission complete");
        break;
    case UART_RX_RDY:
        LOG_INF("Received data: %.*s", evt->data.rx.len, evt->data.rx.buf);
        break;
    case UART_RX_BUF_REQUEST:
        // Provide a new buffer for receiving data
        uart_rx_buf_rsp(dev, rx_buffer, sizeof(rx_buffer));
        break;
    case UART_RX_BUF_RELEASED:
        LOG_INF("RX buffer released");
        break;
    case UART_RX_DISABLED:
        LOG_INF("RX disabled");
        break;
    case UART_RX_STOPPED:
        LOG_ERR("RX stopped due to error");
        break;
    default:
        break;
    }
}


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

#define DEFINE_TXEN(node) \
    { \
        .dev = DEVICE_DT_GET(DT_GPIO_CTLR(node, gpios)), \
        .pin = DT_GPIO_PIN(node, gpios), \
        .flags = DT_GPIO_FLAGS(node, gpios) \
    }

struct txen {
    const struct device *dev;
    gpio_pin_t pin;
    gpio_flags_t flags;
};


int main(void)
{
    printk("nrf9151dk_basic_io test: LEDs, Buttons, UARTS 2 & 3 v1\n");
    struct button buttons[3] = {
        DEFINE_BUTTON(BUTTON0_NODE),
        DEFINE_BUTTON(BUTTON1_NODE),
        DEFINE_BUTTON(BUTTON2_NODE),
//        DEFINE_BUTTON(BUTTON3_NODE),
    };  
    struct led leds[3] = {
        DEFINE_LED(LED0_NODE),
        DEFINE_LED(LED1_NODE),
        DEFINE_LED(LED2_NODE),
//        DEFINE_LED(LED3_NODE),
    };

    rs485_1 = DEVICE_DT_GET(DT_NODELABEL(uart2));
    rs485_2 = DEVICE_DT_GET(DT_NODELABEL(uart3));
    
    // Configure RS485 TX enable pins
/*    gpio_pin_configure_dt(&txen_1, GPIO_OUTPUT_ACTIVE);
    gpio_pin_set_dt(&txen_1,  0); // Set to RX mode

    gpio_pin_configure_dt(&txen_2, GPIO_OUTPUT_ACTIVE);
    gpio_pin_set_dt(&txen_2,  0); // Set to RX mode
*/

    for (int i = 0; i < 3; i++) {
        if (!device_is_ready(leds[i].dev)) {
            printk("LED %d device not ready\n", i);
            return 0;
        }

        gpio_pin_configure(leds[i].dev, leds[i].pin, GPIO_OUTPUT_ACTIVE | leds[i].flags);
        gpio_pin_configure(buttons[i].dev, buttons[i].pin, GPIO_INPUT | buttons[i].flags);
    }

    // Test string to send
    const char *test_str = "Hello from UART2 to UART3!\n";

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
        for (int i = 0; i < 3; i++) {
            if (gpio_pin_get(buttons[i].dev, buttons[i].pin)) {
                printk("Button %d pressed\n", i);
                gpio_pin_set(leds[i].dev, leds[i].pin, 1);
            } else {
                gpio_pin_set(leds[i].dev, leds[i].pin, 0);
            }
        }    
        k_sleep(K_MSEC(400));

    }
    return 0;
}
