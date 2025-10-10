/* include zephyr kernel */
#include <zephyr/kernel.h>

/* include zephyr devicetree */
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>

/* define led0 node */
#define LED0_NODE DT_ALIAS(led0)

/* define sw0 node */
#define SW0_NODE DT_ALIAS(sw0)

/* get dt spec for led0 */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* get dt spec for sw0 */
static const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

uint16_t press_counter=0;

int main(void)
{
	int ret;

	/* check if led0 is ready */
	if (!device_is_ready(led0.port))
	{
		printk("port %s is not ready\n", led0.port->name);
		return -1;
	}
	else
	{
		printk("port %s is ready\n", led0.port->name);
	}

	/* configure led0 as output */
	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	if (ret != 0)
	{
		printk("failed to configure led0 pin\n");
		return -1;
	}
	else
	{
		printk("led0 pin configured\n");
	}

	/* check if sw0 is ready */
	if (!device_is_ready(sw0.port))
	{
		printk("port %s is not ready\n", sw0.port->name);
		return -1;
	}
	else
	{
		printk("port %s is ready\n", sw0.port->name);
	}

	/* configure sw0 as input */
	ret = gpio_pin_configure_dt(&sw0, GPIO_INPUT);
	if (ret != 0)
	{
		printk("failed to configure sw0 pin\n");
		return -1;
	}
	else
	{
		printk("sw0 pin configured\n");
	}

	while (1)
	{
		if(gpio_pin_get_dt(&sw0) == true)
		{
			gpio_pin_set_dt(&led0, 0);
			press_counter++;
			printk("switch pressed %d times\n", press_counter);
		}
		else
		{
			gpio_pin_set_dt(&led0, 1);
		}

		k_sleep(K_MSEC(100));
	}

	return 0;
}