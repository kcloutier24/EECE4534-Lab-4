#include <asm-generic/bitsperlong.h>
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
#include <linux/moduleparam.h>
#include <linux/init.h>

// Module Name
#define MODULE_NAME "gmirror"

// GPIO sizes
#define SW_GPIO_COUNT 8
#define LED_GPIO_COUNT 8
#define SW_MIN_GPIO 893
#define SW_MAX_GPIO 900
#define LED_MIN_GPIO 881
#define LED_MAX_GPIO 888
#define HRTIMER_PERIOD_NS 1000000
 
// variables used in this module
static int irqnum; // SW GPIO IRQ Number
static struct hrtimer gpio_poll; // Switch polling timer

// module parameters
static unsigned int led_gpio_base = 0;
static unsigned int switch_gpio_base = 0;
module_param(led_gpio_base,int,0660);
module_param(switch_gpio_base,int,0660);

// get current switch states
static unsigned int get_switch_states(void)
{
  unsigned int state = 0, i;

  //read current GPIO state and do bitwise operations on
  //      state variable
  for(i = SW_MAX_GPIO; i >= switch_gpio_base; i--)
  {
    state = (state << 1) | gpio_get_value(i);
    //printk(KERN_INFO "State %d: 0x%X\n", i, state);
  }

  // return the sate of all switches
  return state;
}

/* @brief Set LED states */
static void set_leds(unsigned int state)
{
  //set current LED states
  unsigned int i;
  //printk(KERN_INFO "-----------------------\n");
  for(i = led_gpio_base; i <= LED_MAX_GPIO; i++)
  {
    unsigned int k = i - LED_MIN_GPIO;
    gpio_direction_output(i, ((state & ( 1 << k )) >> k));
    //printk(KERN_INFO "LED: %d: 0x%X\n", i, ((state & ( 1 << k )) >> k));
  }
}

/* @brief Initialize GPIOs */
static void initialize_gpio(void)
{
  unsigned int i;
  //set switches as inputs
  for(i = switch_gpio_base; i <= SW_MAX_GPIO; i++)
  {
    int x = gpio_direction_input(i);
    if(x != 0){printk(KERN_ERR "gmirror: Trouble accessing gpio_chip_%d", i);}
  }

  // set LEDs as outputs, all LOW
  set_leds(0x00);
}

/* @brief Handle Switch polling timer */
static enum hrtimer_restart switch_poll_callback(struct hrtimer* timer)
{
  unsigned int states;
  //printk(KERN_INFO "gmirror: Callback\n");

  //mirror states
  set_leds(get_switch_states());

  // restart the timer!
  // re-schedule timer
  hrtimer_forward_now(&gpio_poll, HRTIMER_PERIOD_NS);
  return HRTIMER_RESTART;
}

// initialize module
static int mymod_init(void)
{
  int err;
  struct gpio_chip* chip;
  struct platform_device* pdev;
  struct device* dev;
  struct gpio_desc* gdesc;
  struct resource *res;

  //Make sure GPIO is within range
  if(switch_gpio_base < SW_MIN_GPIO || switch_gpio_base > SW_MAX_GPIO)
  {
    printk(KERN_ERR "gmirror: Switch GPIO Base out of range: %d\n", switch_gpio_base);
    return -EINVAL;
  }

  if(led_gpio_base < LED_MIN_GPIO || led_gpio_base > LED_MAX_GPIO)
  {
    printk(KERN_ERR "gmirror: LED GPIO Base out of range: %d\n", switch_gpio_base);
    return -EINVAL;
  }

  // find GPIO description by GPIO number passed
  
  gdesc = gpio_to_desc(switch_gpio_base);
  if (!gdesc)
    {
      printk(KERN_ERR "gmirror: Error getting GPIO description for switches!\n");
      return -ENODEV;
    }

  // get chip of switches gpio
  chip = gpiod_to_chip(gdesc);
  if (!chip)
    {
      printk(KERN_ERR "gmirror: Error getting switch GPIO chip\n");
      return -ENODEV;
    }
  
  // retrieve original platform device from gpio chip,
  // now we can extract information from the device tree node
  dev = chip->parent;
  pdev = to_platform_device(dev);
  
  // create, initialize and start polling timer
  // initialize and start timer with hrtimer_init() and hrtimer_start(),
  //       use switch_poll_callback. Use relative mode, and set an interval
  //       of 100 ms with ktime_set()
  hrtimer_init(&gpio_poll, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
  gpio_poll.function = &switch_poll_callback;
  hrtimer_start(&gpio_poll, ktime_set(0, (HRTIMER_PERIOD_NS)), HRTIMER_MODE_REL);

  // initialize GPIOs
  initialize_gpio();

  printk(KERN_INFO "gmirror: gmirror initialized successfully\n");

  // A non 0 return means init_module failed; module can't be loaded.
  return 0;
}

// remove module and perform cleanup
static void mymod_exit(void)
{
  hrtimer_cancel(&gpio_poll);
  printk(KERN_INFO "gmirror: gmirror removed successfully\n");
  // nothing to do right now!
}

// declare initialization function
module_init(mymod_init);
module_exit(mymod_exit);

// declare module description and license
MODULE_DESCRIPTION("EECE4534 GPIO IRQ module");
MODULE_LICENSE("GPL");

// insmod gmirror.ko led_gpio_base=881 switch_gpio_base=893
