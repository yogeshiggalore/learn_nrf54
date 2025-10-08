/* include zephyr kernel */
#include <zephyr/kernel.h>

/* include zephyr devicetree */
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>

/* define led0 node */
#define LED0_NODE DT_ALIAS(led0)

/* define sw0 node */
#define SW0_NODE DT_ALIAS(sw0)

/* get dt spec for led0 */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* get dt spec for sw0 */
static const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

int main(void)
{
	int ret;

	/* check if led0 is ready */
	if (!device_is_ready(led0.port))
	{
		return -1;
	}

	/* configure led0 as output */
	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	if (ret != 0)
	{
		return -1;
	}

	/* check if sw0 is ready */
	if (!device_is_ready(sw0.port))
	{
		return -1;
	}

	/* configure sw0 as input */
	ret = gpio_pin_configure_dt(&sw0, GPIO_INPUT);
	if (ret != 0)
	{
		return -1;
	}

	while (1)
	{
		gpio_pin_get_dt(&sw0) == 0 ? gpio_pin_set_dt(&led0, 1) : gpio_pin_set_dt(&led0, 0);
		k_sleep(K_MSEC(10));
	}

	return 0;
}