/**
 * @file user_mcp_tools.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-25
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#if 1
#include <FreeRTOS.h>
#include <string.h>
#include <task.h>
#include "timers.h"
#include "user_mcp_tools.h"
#include "vb6824.h"
#include "ws_demo.h"
#include "media.h"
#include "user_mcp_tools.h"
#include "bflb_gpio.h"
#include "stoge.h"
#include "log.h"
#define GPIO_LED_PIN 20

static bool led_state = false;

static struct bflb_device_s *gpio;
/**
 * @brief 初始化 MCP 设备
 *
 */
void user_cmp_tools_hw_init(void)
{
	// 初始化 LED GPIO
	gpio = bflb_device_get_by_name("gpio");
	bflb_gpio_init(gpio, GPIO_LED_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
	if (mcp_tool_arry[0].name == NULL)
	{
		memset(mcp_tool_arry, 0, sizeof(mcp_server_tool_t) * MCP_SERVER_TOOL_NUMBLE_LEN);
	}
}
/**
 * @brief Set the Volume Request Handler object
 *
 * @param value
 */
static void setVolumeRequestHandler(void *value)
{
	int volume = *(int *)value;

	uint8_t vb_volume_value = volume * VB6824_MAX_VOLUME / 100;
	if (vb_volume_value < VB6824_MIN_VOLUME)
	{
		vb_volume_value = VB6824_MIN_VOLUME;
	}
	printf("[%s()-%d]cloud volume:%d set volume:%d\r\n", __func__, __LINE__, volume, vb_volume_value);
	stoge_volume_set(volume);
	vb6824_set_volume(vb_volume_value);
}
/**
 * @brief Set the Volume Request Handler object
 *
 * @param value
 */
static void checkVolumeRequestHandler(void *value)
{
	// int volume;
	returnValues_t *returnValues = (returnValues_t *)value;
	uint8_t vb_volume_value = 0;
	stoge_volume_get(&vb_volume_value);
	sprintf(returnValues->value, "%d", vb_volume_value);
	printf("[%s()-%d]get volume:%s\r\n", __func__, __LINE__, returnValues->value);
}
/**
 * @brief Set the LED Request Handler object
 *
 * @param value
 */
static void setLEDRequestHandler(void *value)
{
	if (value == NULL)
	{
		return;
	}
	led_state = *(int *)value;
	// 设置 LED 状态
	LOG_I("led_state:%d\r\n", led_state);
	if (led_state)
	{
		bflb_gpio_set(gpio, GPIO_LED_PIN);
	}
	else
	{
		bflb_gpio_reset(gpio, GPIO_LED_PIN);
	}
}

static void checkLEDRequestHandler(void *value)
{
	if (value == NULL)
	{
		return;
	}
	returnValues_t *returnValues = (returnValues_t *)value;
	sprintf(returnValues->value, "%s", led_state ? "true" : "false");
}

/**
 * @brief MCP 服务器示例
 *
 */
int user_cmp_creat_tools_examples(cJSON *toolsList)
{
	if (toolsList == NULL)
	{
		return -1;
	}
	cJSON *json_toolsList = toolsList;
	// 添加一个扬声器工具
	mcp_server_tool_t speaker = {
		.name = "Speaker",
		.description = "扬声器",
		.inputSchema = {
			// 设置属性，让小智AI读取当前音量
			.properties = {
				{"volume", "当前音量值,用于查询音量大小", MCP_SERVER_TOOL_TYPE_NUMBER},
			},
			// 设置方法，让小智AI控制音量
			.methods = {
				{"SetVolume", "设置音量", {{"volume", "音量值,0到100之间的整数", MCP_SERVER_TOOL_TYPE_NUMBER}}},
			},
		},
		.setRequestHandler = setVolumeRequestHandler,
		.checkRequestHandler = checkVolumeRequestHandler,
	};
	// 添加一个点灯工具
	mcp_server_tool_t led = {
		.name = "Light",
		.description = "控制是否打开灯光",
		.setRequestHandler = setLEDRequestHandler,
		.checkRequestHandler = checkLEDRequestHandler,
		.inputSchema = {
			// 设置属性，让小智AI读取当前LED状态
			.properties[0].name = "enabled",
			.properties[0].description = "当前灯光状态",
			.properties[0].type = MCP_SERVER_TOOL_TYPE_BOOLEAN,
			// 设置方法，让小智AI控制LED
			.methods[0].name = "SetEnabled",
			.methods[0].description = "设置是否打开灯光",
			// 添加设置参数
			.methods[0].parameters[0].name = "enabled",
			.methods[0].parameters[0].description = "true 表示打开灯光,false 表示关闭灯光",
			.methods[0].parameters[0].type = MCP_SERVER_TOOL_TYPE_BOOLEAN,
		},
	};

	int ret = mcp_server_add_tool_to_toolList(json_toolsList, &speaker);
	ret = mcp_server_add_tool_to_toolList(json_toolsList, &led);

	return ret;
}
#endif