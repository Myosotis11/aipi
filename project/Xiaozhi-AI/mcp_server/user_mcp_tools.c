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
#include "bflb_uart.h"          // 添加 UART 驱动头文件
#include "bflb_gpio.h"
#include "stoge.h"
#include "log.h"

// 移除 GPIO_LED_PIN 和 led_state，不再需要板载 LED 控制
// static bool led_state = false;
// static struct bflb_device_s *gpio;

static struct bflb_device_s *uart0;  // UART 设备句柄

// 辅助函数：通过 UART 发送字符串
static void uart_send_string(const char *str)
{
    while (*str) {
        bflb_uart_putchar(uart0, *str++);
    }
}

/**
 * @brief 初始化 MCP 设备
 *
 */
void user_cmp_tools_hw_init(void)
{
    // 初始化 UART0（或其他空闲串口，根据硬件连接选择）
    uart0 = bflb_device_get_by_name("uart0");   // 通常 UART0 对应 TX0/RX0
    struct bflb_uart_config_s uart_cfg = {
        .baudrate = 115200,                     // 与 STM32 一致
        .data_bits = UART_DATA_BITS_8,
        .stop_bits = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
        .flow_ctrl = UART_FLOW_CTRL_NONE,
        .tx_fifo_threshold = 7,
        .rx_fifo_threshold = 7,
    };
    bflb_uart_init(uart0, &uart_cfg);

    // 不再需要初始化 GPIO
    // if (mcp_tool_arry[0].name == NULL) { ... } // 此段保留，用于重置工具列表，不影响
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
 * @brief Set the LED Request Handler object (修改为串口发送)
 *
 * @param value
 */
static void setLEDRequestHandler(void *value)
{
    if (value == NULL)
    {
        return;
    }
    int led_state = *(int *)value;  // 0 或 1
    LOG_I("led_state:%d\r\n", led_state);
    
    if (led_state) {
        // 发送 LED_ON 指令，带回车换行
        uart_send_string("LED_ON\r\n");
    } else {
        // 发送 LED_OFF 指令，带回车换行
        uart_send_string("LED_OFF\r\n");
    }
}

/**
 * @brief 查询 LED 状态（可选，简单返回本地状态，实际可从 STM32 查询）
 *
 */
static void checkLEDRequestHandler(void *value)
{
    if (value == NULL)
    {
        return;
    }
    returnValues_t *returnValues = (returnValues_t *)value;
    // 由于我们不再维护本地状态，这里固定返回 false，或根据 STM32 反馈来更新
    // 为简单起见，返回 false
    sprintf(returnValues->value, "%s", "false");
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
    // 添加一个点灯工具（已修改为串口控制）
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
