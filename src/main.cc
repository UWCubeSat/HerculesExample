/*! @file main.cc
 *   @brief Main file for blink project
 *   @date 22-Dec-2020
 *
 *   Example project for the Hercules TMS570LC43x, blinks LED2 and LED3 using FreeRTOS tasks.
 */

#include "FreeRTOS.h"
#include "os_task.h"
#include "HL_sys_common.h"
#include "HL_gio.h"

/*
 * A struct holding arguments for the blink_led_task, storing a GIO port base, pin,
 * and desired blink frequency.
 */
typedef struct blink_task_args {
  gioPORT_t *gioPort;
  uint32_t pin;
  int frequency;

  blink_task_args(gioPORT_t *port, uint32_t p, int freq) : gioPort(port), pin(p), frequency(freq) { }
} blink_args;

/*! @fn void blink_task(void *p)
 *   @brief Task to be called by FreeRTOS that blinks an led at a frequency dictated by
 *   the given blink_args struct.
 *
 *   @param[in] p Pointer to a blink_args
 */
void blink_led_task(void *p) {
  blink_args args = *((blink_args *) p);
  bool on = true;
  while (1) {
    gioSetBit(args.gioPort, args.pin, on);
    on = !on;
    vTaskDelay(1000 / (2 * args.frequency));
  }
}

/*! @fn int main()
 *   @brief Main function
 *
 *   This function is called after startup, starting two tasks which
 *   blink LED2 and LED3 at frequencies of 2 and 3 Hz respectively.
 */
int main() {
  gioInit();
  gioSetDirection(gioPORTB, 0xFF);

  blink_args *led2 = (blink_args *)pvPortMalloc(sizeof(blink_args));
  *led2 = blink_args(gioPORTB, 6, 2);
  if (xTaskCreate(blink_led_task, "blink_LED2", 128, (void *)led2, 1, nullptr));

  blink_args *led3 = (blink_args *)pvPortMalloc(sizeof(blink_args));
  *led3 = blink_args(gioPORTB, 7, 3);
  xTaskCreate(blink_led_task, "blink_LED3", 128, (void *)led3, 1, nullptr);

  vTaskStartScheduler();
  while (1) { }
}
