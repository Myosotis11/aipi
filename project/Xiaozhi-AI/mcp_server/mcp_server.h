/**
 * @file mcp_server_set.h
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-22
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#if 1
#ifndef __MCP_SERVER_H__
#define __MCP_SERVER_H__
#include "cJSON.h"
#include "stdint.h"
#define MCP_SERVER_TOOL_NUMBLE_LEN 10			 // 工具数量
#define MCP_SERVER_TOOL_PROPERTIES_NUM 5		 // 属性数量
#define MCP_SERVER_TOOL_METHODS_NUM 5			 // 方法数量
#define MCP_SERVER_TOOL_METHODS_PARAMETERS_NUM 5 // 方法参数数量
/**
 * @brief MCP 服务器工具类型枚举
 *
 */
typedef enum
{
	MCP_SERVER_TOOL_TYPE_FALSE = 0,
	MCP_SERVER_TOOL_TYPE_TRUE,
	MCP_SERVER_TOOL_TYPE_NULL,
	MCP_SERVER_TOOL_TYPE_NUMBER,
	MCP_SERVER_TOOL_TYPE_STRING,
	MCP_SERVER_TOOL_TYPE_ARRAY,
	MCP_SERVER_TOOL_TYPE_OBJECT,
	MCP_SERVER_TOOL_TYPE_RAW,
	MCP_SERVER_TOOL_TYPE_TEXT,
	MCP_SERVER_TOOL_TYPE_BOOLEAN,
	MCP_SERVER_TOOL_TYPE_MAX,
} mcp_server_tool_type_t;
/**
 * @brief 属性结构体
 *
 */
typedef struct
{
	char *name;					 // 属性名称
	char *description;			 // 属性描述
	mcp_server_tool_type_t type; // 属性类型
} properties_t;

typedef struct
{
	char *name;					 // 参数名称
	char *description;			 // 参数描述
	mcp_server_tool_type_t type; // 参数类型
} parameters_t;
/**
 * @brief 方法结构体
 *
 */
typedef struct
{
	char *name;														 // 方法名称
	char *description;												 // 参数描述
	parameters_t parameters[MCP_SERVER_TOOL_METHODS_PARAMETERS_NUM]; // 方法参数
} methods_t;
/**
 * @brief 输入参数结构体
 *
 */
typedef struct
{
	properties_t properties[MCP_SERVER_TOOL_PROPERTIES_NUM]; // 属性
	methods_t methods[MCP_SERVER_TOOL_METHODS_NUM];			 // 方法
} inputSchema_t;
/**
 * @brief MCP 服务器工具结构体
 *
 */
typedef struct
{
	int error_code; // 错误码
	char value[8];	// 内容
	char *type;		//  类型
} returnValues_t;

typedef struct
{
	char *name;							 // 工具名称
	char *description;					 // 工具描述
	void (*setRequestHandler)(void *);	 // 工具回调函数
	void (*checkRequestHandler)(void *); // 工具回调函数
	inputSchema_t inputSchema;			 // 输入参数
} mcp_server_tool_t;

extern mcp_server_tool_t mcp_tool_arry[MCP_SERVER_TOOL_NUMBLE_LEN];

int mcp_server_add_tool_to_toolList(void *toolsList, mcp_server_tool_t *tool);
returnValues_t mcp_server_responsive_tool_request(char *tool_name, cJSON *params);
returnValues_t mcp_server_responsive_tool_check_request(char *tool_name);
#endif // !__MCP_SERVER_SET_H__
#endif