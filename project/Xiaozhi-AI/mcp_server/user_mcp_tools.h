/**
 * @file user_mcp_tools.h
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-25
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#if 1
#ifndef USER_MCP_TOOLS_H
#define USER_MCP_TOOLS_H

#include "mcp_server.h"

/**
 * @brief 设备初始化
 *
 */
void user_cmp_tools_hw_init(void);

int user_cmp_creat_tools_examples(cJSON *toolsList);
#endif // USER_MCP_TOOLS_H
#endif