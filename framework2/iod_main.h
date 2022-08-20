#define IOD_BTN_VALUE_INIT (true)
#define IOD_LED0_VALUE_INIT (true)
#define IOD_LED1_VALUE_INIT (false)
#define IOD_UART_BUFF_SIZE (32 + 1)

extern void iod_init();
extern void iod_main_1ms();
extern void iod_main_in();
extern void iod_main_out();
extern void iod_read_btn_value(bool *);
extern void iod_write_led0_value(bool);
extern void iod_write_led1_value(bool);
extern bool iod_call_uart_receive(uint8_t *);
extern void iod_call_uart_transmit(uint8_t *);
