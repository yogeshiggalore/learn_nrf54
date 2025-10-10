/* include kernel components */
#include <zephyr/kernel.h>

/* include device drivers */
#include <zephyr/drivers/gpio.h>

#define LED_SLEEP_TIME_MS   1000

/* the device tree indentifier for the led0 node */
#define LED0_NODE DT_ALIAS(led0)

/* get devicetree spec for led0 node */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
	int ret;

	/* check if the led port is ready */
	if (!device_is_ready(led.port))
	{
		printk("port %s is not ready\n", led.port->name);
		return -1;
	}
	else
	{
		printk("port %s is ready\n", led.port->name);
	}

	/* configure the led pin */
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		printk("failed to configure led pin\n");
		return -1;
	}
	else
	{
		printk("led pin configured\n");
	}

	/* blink the led */
	while (1)
	{
		/* toggle the led pin */
		gpio_pin_toggle(led.port, led.pin);

		/* print a message */
		printk("led toggled\n");

		/* sleep for some time */
		k_msleep(LED_SLEEP_TIME_MS);
	}

    return 0;
}
