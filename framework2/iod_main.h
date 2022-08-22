#define PWM_5_WRAP          (25000) // PWMの分解能：周期 125[Mhz]/25000=5[Khz]->0.2[ms]

#define IOD_BTN_VALUE_INIT  (true)
#define IOD_LED0_VALUE_INIT (true)
#define IOD_LED1_VALUE_INIT (true)
#define IOD_UART_BUFF_SIZE  (32 + 1)
#define IOD_ADC_VALUE_MAX   (0x0FFF)
#define IOD_PWM1_DUTY_MAX   PWM_5_WRAP
#define IOD_PWM2_DUTY_MAX   PWM_5_WRAP

extern void iod_init();
extern void iod_main_1ms();
extern void iod_main_in();
extern void iod_main_out();
extern void iod_read_btn_value(bool *);
extern void iod_write_led0_value(bool);
extern void iod_write_led1_value(bool);
extern bool iod_call_uart_receive(uint8_t *);
extern void iod_call_uart_transmit(uint8_t *);
extern void iod_read_adc_value(uint16_t *);
extern void iod_call_pwm1_set_duty(uint16_t);
extern void iod_call_pwm2_set_duty(uint16_t);
