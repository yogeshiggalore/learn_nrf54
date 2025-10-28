/* include headers */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>

/* the device tree indentifier for the led0 node */
#define LED0_NODE DT_ALIAS(led0)

/* get devicetree spec for led0 node */
static const struct gpio_dt_spec dev_led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

const struct device *dev_uart;
struct uart_config cfg_uart;

char msg_uart[] = "Use following commands to control LED \r\nA: Turn on led\r\nB: Turn off led\r\nC: Get current led status\r\n";

void write_to_uart(char array[], uint8_t len);
void read_from_uart(void);

uint8_t led_status = 0;

int main(void)
{
	int ret;

	/* check if the led port is ready */
	if (!device_is_ready(dev_led.port))
	{
		return -1;
	}

	/* configure the led pin */
	ret = gpio_pin_configure_dt(&dev_led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		return -1;
	}

	gpio_pin_set_dt(&dev_led, true);
	led_status = true;

	/* configure the uart */
	cfg_uart.baudrate = 9600;
	cfg_uart.data_bits = UART_CFG_DATA_BITS_8;
    cfg_uart.flow_ctrl = UART_CFG_FLOW_CTRL_NONE;
    cfg_uart.parity    = UART_CFG_PARITY_NONE;
    cfg_uart.stop_bits = UART_CFG_STOP_BITS_1;

	dev_uart = DEVICE_DT_GET(DT_NODELABEL(uart20));
	if(!device_is_ready(dev_uart))
	{
		return -1;
	}

	ret = uart_configure(dev_uart, &cfg_uart);
	if (ret < 0)
	{
		printk("uart configure failed %d\r\n", ret);
		if(ret == -ENOTSUP)
		{
			printk("uart run time cfg not supported\r\n");
		}
		return -1;
	}

	write_to_uart(msg_uart, sizeof(msg_uart));

	gpio_pin_set_dt(&dev_led, false);
    led_status = false;

	while(true)
	{
		read_from_uart();
        k_sleep(K_MSEC(1));
	}

    return 0;
}

void write_to_uart(char array[], uint8_t len)
{
    
    for(uint8_t i=0; i < len;i++)
    {
        uart_poll_out(dev_uart,array[i]);
    }
}

void read_from_uart(void)
{
    char recvch;
    if(uart_poll_in(dev_uart,&recvch) == 0)
    {
        if((recvch == 'A') || (recvch == 'a'))
        {
            gpio_pin_set_dt(&dev_led, false);
            led_status = false;
            write_to_uart("led off\r\n",9);
        }
        else if((recvch == 'B') || (recvch == 'b'))
        {
            gpio_pin_set_dt(&dev_led, true);
            led_status = true;
            write_to_uart("led on\r\n",8);
        }
        else if((recvch == 'C') || (recvch == 'c'))
        {
            if(led_status == true)
            {
                write_to_uart("led off\r\n",9);
            }
            else
            {
                write_to_uart("led on\r\n",8);
            }
        }
        else
        {
            write_to_uart("wrong cmd\r\n",11);
            write_to_uart(msg_uart,strlen(msg_uart));
        }
    }
}