#define PWM_5_WRAP          (25000) // PWMの分解能：周期 125[Mhz]/25000=5[Khz]->0.2[ms]
#define GPIO_GP0_UART       (0)
#define GPIO_GP1_UART       (1)
#define GPIO_GP2_PORT       (2)
#define GPIO_GP7_PORT       (7)
#define GPIO_GP10_PWM       (10)
#define GPIO_GP11_PWM       (11)
#define GPIO_GP25_PORT      PICO_DEFAULT_LED_PIN
#define GPIO_GP26_ADC       (26)

#define IOD_BTN_VALUE_INIT  (true)
#define IOD_LED0_VALUE_INIT (true)
#define IOD_LED1_VALUE_INIT (true)
#define IOD_UART_BUFF_SIZE  (32 + 1)
#define IOD_ADC_VALUE_MAX   (0x0FFF)
#define IOD_PWM1_DUTY_MAX   PWM_5_WRAP
#define IOD_PWM2_DUTY_MAX   PWM_5_WRAP

// iod_main
extern void iod_init();
extern void iod_main_1ms();
extern void iod_main_in();
extern void iod_main_out();
// iod_port
extern void iod_port_init();
extern void iod_port_main_1ms();
extern void iod_port_main_in();
extern void iod_port_main_out();
extern void iod_read_btn_value(bool *);
extern void iod_write_led0_value(bool);
extern void iod_write_led1_value(bool);
// iod_uart
extern void iod_uart_init();
extern void iod_uart_main_1ms();
extern void iod_uart_main_in();
extern void iod_uart_main_out();
extern bool iod_call_uart_receive(uint8_t *);
extern void iod_call_uart_transmit(uint8_t *);
// iod_adc
extern void iod_adc_init();
extern void iod_adc_main_1ms();
extern void iod_adc_main_in();
extern void iod_adc_main_out();
extern void iod_read_adc_value(uint16_t *);
// iod_pwm
extern void iod_pwm_init();
extern void iod_pwm_main_1ms();
extern void iod_pwm_main_in();
extern void iod_pwm_main_out();
extern void iod_call_pwm1_set_duty(uint16_t);
extern void iod_call_pwm2_set_duty(uint16_t);
