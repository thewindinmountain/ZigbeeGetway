#include "tcp_sta.h"
#include "at_command.h"
#include "esp_log.h"

static void proccess_data_from_zigbee(uint8_t* data, unsigned short len)
{
	ESP_LOGI("zigbee gateway:", "proccess zigbee %d bytes",len);
}

void app_main()
{
	init_cmd_task(proccess_data_from_zigbee);
	init_wifi_sta();
}
