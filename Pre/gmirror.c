#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/driver.h>
#include <linux/device.h>
#include <linux/hrtimer.h>
#include <linux/of_device.h>
#include <asm/io.h>

// Module Name
#define MODULE_NAME "gmirror"

// GPIO sizes
#define SW_GPIO_COUNT 8
#define LED_GPIO_COUNT 8

// variables used in this module
static int irqnum;               // SW GPIO IRQ Number
static struct hrtimer gpio_poll; // Switch polling timer

// module parameters
static unsigned int led_gpio_base = 0;
static unsigned int switch_gpio_base = 0;

// get current switch states
static unsigned int get_switch_states(void)
{
  unsigned int state = 0, i;
  printk(KERN_INFO "SWITCH STATEEEEEEEEEEE %ld", state);

  // TODO: read current GPIO state and do bitwise operations on
  //       state variable

  for (i = 0; i < SW_GPIO_COUNT; ++i)
  {
    state |= gpio_get_value(switch_gpio_base + i) << i;
  }

  // return the sate of all switches
  return state;
}

/* @brief Set LED states */
static void set_leds(unsigned int state)
{
  unsigned int i;
  printk(KERN_INFO "LED SET STATEEEEEEEEEEE %ld", state);
  // TODO: set current LED states

  for (i = 0; i < LED_GPIO_COUNT; ++i)
  {
    gpio_set_value(led_gpio_base + i, (state >> i) & 0x01);
  }
}

/* @brief Initialize GPIOs */
static void initialize_gpio(void)
{
  unsigned int i;
  printk(KERN_INFO "INITIALIZE GPIOOOOOOOOOOOOOOOOOOOOOOO \n");

  // TODO: set switches as inputs

  for (i = 0; i < SW_GPIO_COUNT; ++i)
  {
    gpio_direction_input(switch_gpio_base + i);
  }

  for (i = 0; i < LED_GPIO_COUNT; ++i)
  {
    gpio_direction_output(led_gpio_base + i, 0);
  }

  // set LEDs as outputs, all LOW
  set_leds(0x00);
}

/* @brief Handle Switch polling timer */
static enum hrtimer_restart switch_poll_callback(struct hrtimer *timer)
{
  unsigned int states;

  printk(KERN_INFO "TIMER CALLBACKKKKKKKKKKKKKKKK");

  // read states of switches
  states = get_switch_states();

  // TODO: mirror states
  set_leds(states);

  printk(KERN_INFO "SWITCH STATEEEEEEEEEEE %ld", states);

  // restart the timer!
  // TODO: re-schedule timer
  hrtimer_forward(timer, timer->_softexpires, ktime_set(0, 100000000)); // restart in 100 ms

  return HRTIMER_RESTART;
}

// initialize module
static int mymod_init(void)
{
  int err;
  struct gpio_chip *chip;
  struct platform_device *pdev;
  struct device *dev;
  struct gpio_desc *gdesc;
  struct resource *res;

  // TODO: validate parameter values
  if (led_gpio_base == 0 || switch_gpio_base == 0)
  {
    printk(KERN_ERR "Invalid GPIO base values\n");
    return -EINVAL;
  }
  printk(KERN_INFO "led_gpio_baseeeeeeeeeeeeeeeeeeee: %ld\n", led_gpio_base);
  printk(KERN_INFO "switch_gpio_baseeeeeeeeeeeeeeeeeeeeee: %ld\n", switch_gpio_base);

  // find GPIO description by GPIO number passed
  gdesc = gpio_to_desc(switch_gpio_base);
  if (!gdesc)
  {
    printk("error getting GPIO description for switches!\n");
    return -ENODEV;
  }

  // get chip of switches gpio
  chip = gpiod_to_chip(gdesc);
  if (!chip)
  {
    printk("error getting switch GPIO chip\n");
    return -ENODEV;
  }

  // retrieve original platform device from gpio chip,
  // now we can extract information from the device tree node
  dev = chip->parent;
  pdev = to_platform_device(dev);

  // create, initialize and start polling timer
  // TODO: initialize and start timer with hrtimer_init() and hrtimer_start(),
  //       use switch_poll_callback. Use relative mode, and set an interval
  //       of 100 ms with ktime_set()
  // Initialize the timer struct using the function hrtimer_init
  hrtimer_init(&gpio_poll, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

  gpio_poll.function = &switch_poll_callback;
  // Start the timer with an interval of 100 ms
  hrtimer_start(&gpio_poll, ktime_set(0, 100000000), HRTIMER_MODE_REL); // 100ms

  // initialize GPIOs
  initialize_gpio();

  // A non 0 return means init_module failed; module can't be loaded.
  return 0;
}

// remove module and perform cleanup
static void mymod_exit(void)
{
  // nothing to do right now!
}

// declare initialization function
module_init(mymod_init);
module_exit(mymod_exit);

// module parameter declaration
// TODO: declare module parameters here
// Module parameters
module_param(led_gpio_base, uint, 0);
MODULE_PARM_DESC(led_gpio_base, "LED GPIO base");

module_param(switch_gpio_base, uint, 0);
MODULE_PARM_DESC(switch_gpio_base, "Switch GPIO base");

// declare module description and license
MODULE_DESCRIPTION("EECE4534 GPIO IRQ module");
MODULE_LICENSE("GPL");