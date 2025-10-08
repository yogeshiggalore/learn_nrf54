/* include kernel and gpio device drivers */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* timer value 500 ms*/
#define TIMER_VALUE 500

/* led0 node */
#define LED0_NODE DT_ALIAS(led0)

/* led0 device tree */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* kernel timer configurations */
struct k_timer led_timer;
void led_timer_expired(struct k_timer *timer);
void led_timer_stop(struct k_timer *timer);


int main(void)
{
	int ret=0;

	/* check for led0 device readiness */
	if(!device_is_ready(led0.port))
	{
		return -1;
	}

	/* configure led0 pin as output */
	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		return ret;
	}

	gpio_pin_set_dt(&led0, true);

	/* kernel timer init */
	k_timer_init(&led_timer, led_timer_expired, led_timer_stop);

	/* start timer with TIMER_VALUE and delay TIMER_VALUE */
	k_timer_start(&led_timer, K_MSEC(TIMER_VALUE), K_MSEC(TIMER_VALUE));

    return 0;
}

void led_timer_expired(struct k_timer *timer)
{
	/* toggle led0 pin */
	gpio_pin_toggle_dt(&led0);
}

void led_timer_stop(struct k_timer *timer)
{
	/* do nothing */
}