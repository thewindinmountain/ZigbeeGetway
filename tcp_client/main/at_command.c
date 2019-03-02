#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "string.h"
#include "at_command.h"


static const int RX_BUF_SIZE = 1024;
//TODO we should change the com to NUM4 and NUM5
#define TXD_PIN (GPIO_NUM_1)
#define RXD_PIN (GPIO_NUM_3)
#define UART_NUM_OPT UART_NUM_0

recieve_data_callback process_revdata;

static void init() 
{
	const uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	};
	uart_param_config(UART_NUM_OPT, &uart_config);
	uart_set_pin(UART_NUM_OPT,TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	// We won't use a buffer for sending data.
	uart_driver_install(UART_NUM_OPT, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
}

static int sendData(const char* logName, const uint8_t* data,unsigned short len)
{
	const int txBytes = uart_write_bytes(UART_NUM_OPT, (const char*)data, len);
	ESP_LOGI(logName, "Wrote %d bytes", txBytes);
	return txBytes;
}

static void rx_task()
{
	static const char *RX_TASK_TAG = "RX_TASK";
	esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
	uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
	while (1) {
		const int rxBytes = uart_read_bytes(UART_NUM_OPT, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
		
		if (rxBytes > 0) {
			data[rxBytes] = 0;
			ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
			if(process_revdata!=NULL)
				process_revdata(data,rxBytes);	
			ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
		}
	}
	free(data);
}

int send_cmd(uint8_t* data,unsigned short len)
{
	const int txBytes=sendData("send cmd",data,len);
	return txBytes;
}

int init_cmd_task(recieve_data_callback rd_cb)
{
	init();
	process_revdata=rd_cb;
	xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, 6, NULL);
	return 0;
}

