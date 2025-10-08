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

/* gpio callback */
static struct gpio_callback sw0_cb_data;

void sw0_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{		
	gpio_pin_toggle(led0.port, led0.pin);
}

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

	/* configure sw0 interrupt */
	ret = gpio_pin_interrupt_configure_dt(&sw0, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0)
	{
		return -1;
	}

	/* register sw0 callback */
	gpio_init_callback(&sw0_cb_data, sw0_cb, BIT(sw0.pin));
	gpio_add_callback(sw0.port, &sw0_cb_data);

	while (1)
	{
		k_sleep(K_MSEC(1000));
	}

	return 0;
}