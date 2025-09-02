/**
 * @file mcp_server_set.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-22
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#if 1
#include "mcp_server.h"
#include "log.h"
mcp_server_tool_t mcp_tool_arry[MCP_SERVER_TOOL_NUMBLE_LEN] = {0};
char *mcp_sever_type_str[MCP_SERVER_TOOL_TYPE_MAX] = {"false", "true", "null", "number", "string", "array", "object", "raw", "text", "boolean"};
static returnValues_t ret = {0};

// 枚举值到0~10索引的映射表
uint8_t enum_to_index[] = {
	[cJSON_False] = MCP_SERVER_TOOL_TYPE_FALSE,
	[cJSON_True] = MCP_SERVER_TOOL_TYPE_TRUE,
	[cJSON_NULL] = MCP_SERVER_TOOL_TYPE_NULL,
	[cJSON_Number] = MCP_SERVER_TOOL_TYPE_NUMBER,
	[cJSON_String] = MCP_SERVER_TOOL_TYPE_STRING,
	[cJSON_Array] = MCP_SERVER_TOOL_TYPE_ARRAY,
	[cJSON_Object] = MCP_SERVER_TOOL_TYPE_OBJECT,
	[cJSON_Raw] = MCP_SERVER_TOOL_TYPE_RAW,
};
/**
 * @brief 添加工具到工具列表
 *
 * @param toolsList 工具列表
 * @param tool 工具对象
 * @return int int 0 表示成功，-32602 表示失败
 */
int mcp_server_add_tool_to_toolList(void *toolsList, mcp_server_tool_t *tool)
{

	if (tool == NULL || toolsList == NULL)
	{

		return -32604;
	}
	cJSON *json_toolsList = (cJSON *)toolsList;
	mcp_server_tool_t *tmp_tool = tool; // 临时工具对象
	if (mcp_tool_arry[0].name == NULL)
	{
		memcpy(&mcp_tool_arry[0], tmp_tool, sizeof(mcp_server_tool_t));
	}
	else
	{
		for (int i = 0; i < MCP_SERVER_TOOL_NUMBLE_LEN; i++)
		{
			if (mcp_tool_arry[i].name == NULL)
			{
				memcpy(&mcp_tool_arry[i], tmp_tool, sizeof(mcp_server_tool_t));
				break;
			}
		}
	}

	// 创建并添加工具对象到toolsList
	cJSON *json_tool = cJSON_CreateObject();
	cJSON_AddItemToArray(json_toolsList, json_tool);
	cJSON_AddStringToObject(json_tool, "name", tmp_tool->name);
	cJSON_AddStringToObject(json_tool, "description", tmp_tool->description);
	// inputSchema 对象
	cJSON *inputSchema = cJSON_CreateObject();
	cJSON_AddItemToObject(json_tool, "inputSchema", inputSchema);
	// 添加properties参数
	if (sizeof(tmp_tool->inputSchema.properties) / sizeof(properties_t) > 0)
	{
		cJSON *properties = cJSON_CreateObject();
		cJSON_AddItemToObject(inputSchema, "properties", properties);
		for (size_t i = 0; i < MCP_SERVER_TOOL_PROPERTIES_NUM; i++)
		{
			if (tmp_tool->inputSchema.properties[i].name != NULL) // 判断是否为空
			{
				cJSON *prop = cJSON_CreateObject();
				cJSON_AddStringToObject(prop, "description", tmp_tool->inputSchema.properties[i].description);
				cJSON_AddStringToObject(prop, "type", mcp_sever_type_str[tmp_tool->inputSchema.properties[i].type]);
				cJSON_AddItemToObject(properties, tmp_tool->inputSchema.properties[i].name, prop);
			}
		}
	}

	if (sizeof(tmp_tool->inputSchema.methods) / sizeof(methods_t) > 0)
	{
		cJSON *methods = cJSON_CreateObject();
		for (size_t i = 0; i < MCP_SERVER_TOOL_METHODS_NUM; i++)
		{
			if (tmp_tool->inputSchema.methods[i].name != NULL) // 判断是否为空
			{
				cJSON *method = cJSON_CreateObject();
				cJSON_AddItemToObject(inputSchema, "methods", methods);
				cJSON_AddItemToObject(methods, tmp_tool->inputSchema.methods[i].name, method);
				cJSON_AddStringToObject(method, "description", tmp_tool->inputSchema.methods[i].description);
				// 添加parameters参数
				if (sizeof(tmp_tool->inputSchema.methods[i].parameters) / sizeof(parameters_t) > 0)
				{
					cJSON *parameters = cJSON_CreateObject();
					cJSON_AddItemToObject(method, "parameters", parameters);
					for (size_t i = 0; i < MCP_SERVER_TOOL_METHODS_PARAMETERS_NUM; i++)
					{
						if (tmp_tool->inputSchema.methods[i].parameters[i].name != NULL) // 判断是否为空
						{
							cJSON *param = cJSON_CreateObject();
							cJSON_AddItemToObject(parameters, tmp_tool->inputSchema.methods[i].parameters[i].name, param);
							cJSON_AddStringToObject(param, "description", tmp_tool->inputSchema.methods[i].parameters[i].description);
							cJSON_AddStringToObject(param, "type", mcp_sever_type_str[tmp_tool->inputSchema.properties[i].type]);
						}
					}
				}
			}
		}
	}

	return 0;
}
/**
 * @brief Construct a new mcp server responsive tool request object
 * 		// 根据工具名称，找到对应的工具，并执行对应的请求
 *
 * @param tool_name
 * @param arguments
 * @return returnValues_t
 */
returnValues_t mcp_server_responsive_tool_request(char *tool_name, cJSON *arguments)
{

	if (tool_name == NULL || arguments == NULL)
	{
		ret.error_code = -32602;
		return ret;
	}
	// 开始定位工具
	cJSON *param = NULL;
	// 计算当前有多少个工具
	uint8_t tools_numble = 0;
	for (; tools_numble < MCP_SERVER_TOOL_NUMBLE_LEN; tools_numble++)
	{
		if (mcp_tool_arry[tools_numble].name == NULL)
		{
			break;
		}
	}
	if (tools_numble == 0)
	{
		ret.error_code = -32602;
		return ret;
	}

	for (int i = 0; i < tools_numble; i++)
	{
		if (strcmp(mcp_tool_arry[i].name, tool_name) == 0)
		{
			// 判断到底是哪个方法
			for (int j = 0; j < sizeof(mcp_tool_arry[i].inputSchema.methods) / sizeof(methods_t); j++)
			{
				// 遍历参数
				for (size_t k = 0; k < sizeof(mcp_tool_arry[i].inputSchema.methods[j].parameters) / sizeof(parameters_t); k++)
				{

					if (cJSON_GetObjectItem(arguments, mcp_tool_arry[i].inputSchema.methods[j].name) == NULL)
					{
						param = cJSON_GetObjectItem(arguments, mcp_tool_arry[i].inputSchema.methods[j].parameters[k].name);
					}
					else
					{
						cJSON *methods = cJSON_GetObjectItem(arguments, mcp_tool_arry[i].inputSchema.methods[j].name);
						param = cJSON_GetObjectItem(methods, mcp_tool_arry[i].inputSchema.methods[j].parameters[k].name);
						if (param == NULL)
						{
							ret.error_code = -32602;
							return ret;
						}
					}
					mcp_server_tool_type_t type = enum_to_index[param->type];

					// 回调函数
					if (mcp_tool_arry[i].setRequestHandler != NULL)
					{
						switch (type)
						{
						case MCP_SERVER_TOOL_TYPE_FALSE:
							sprintf(ret.value, "%s", "false");
							goto __ret;
						case MCP_SERVER_TOOL_TYPE_TRUE:
							sprintf(ret.value, "%s", "true");
							goto __ret;
						case MCP_SERVER_TOOL_TYPE_BOOLEAN:
							sprintf(ret.value, "%s", "bool");
							goto __ret;
						case MCP_SERVER_TOOL_TYPE_NUMBER:
							sprintf(ret.value, "%d", param->valueint);
						__ret:
							if (mcp_tool_arry[i].setRequestHandler != NULL)
							{
								LOG_I("param->valueint=%d\r\n", param->valueint);
								mcp_tool_arry[i].setRequestHandler(&param->valueint);
								ret.error_code = 0;
								ret.type = mcp_sever_type_str[MCP_SERVER_TOOL_TYPE_TEXT];
							}
							else
							{
								memset(ret.value, 0, sizeof(ret.value));
								ret.error_code = -32604;
							}
							return ret;

						case MCP_SERVER_TOOL_TYPE_OBJECT:
							// 暂不支持
							break;
						case MCP_SERVER_TOOL_TYPE_ARRAY:
							// 暂不支持
							break;
						case MCP_SERVER_TOOL_TYPE_RAW:
						case MCP_SERVER_TOOL_TYPE_NULL:
						case MCP_SERVER_TOOL_TYPE_STRING:
						case MCP_SERVER_TOOL_TYPE_TEXT:
							if (mcp_tool_arry[i].setRequestHandler != NULL)
							{
								mcp_tool_arry[i].setRequestHandler(&param->valuestring);
								ret.error_code = 0;
								ret.type = mcp_sever_type_str[MCP_SERVER_TOOL_TYPE_TEXT];
								sprintf(ret.value, "%s", param->valuestring);
							}
							else
							{
								ret.error_code = -32604;
								memset(ret.value, 0, sizeof(ret.value));
							}
							return ret;
							break;
						default:
							break;
						}
					}
				}
			}
		}
	}
	// 回调
	ret.error_code = -32602;
	return ret;
}

/**
 * @brief Construct a new mcp server responsive tool check request object
 *  		// 根据工具名称，找到对应的工具，并执行对应的请求
 * @param tool_name // 工具名称
 * @param ret_val  // 返回数据的缓存
 * @return returnValues_t // 返回值
 */
returnValues_t mcp_server_responsive_tool_check_request(char *tool_name)
{
	returnValues_t ret = {0};
	if (tool_name == NULL)
	{
		ret.error_code = -32602;
		goto __exit;
	}
	// 开始定位工具
	cJSON *param = NULL;
	// 计算当前有多少个工具
	uint8_t tools_numble = 0;
	for (; tools_numble < MCP_SERVER_TOOL_NUMBLE_LEN; tools_numble++)
	{
		if (mcp_tool_arry[tools_numble].name == NULL)
		{
			break;
		}
	}
	if (tools_numble == 0)
	{
		printf("tools numble is 0\r\n");
		ret.error_code = -32602;
		goto __exit;
	}
	// 遍历工具
	for (int i = 0; i < tools_numble; i++)
	{
		if (strcmp(mcp_tool_arry[i].name, tool_name) == 0)
		{
			// 调用回调函数 获取数据
			if (mcp_tool_arry[i].checkRequestHandler != NULL)
			{
				ret.type = mcp_sever_type_str[MCP_SERVER_TOOL_TYPE_TEXT];
				mcp_tool_arry[i].checkRequestHandler(&ret);
				goto __exit;
			}
			else
			{
				ret.error_code = -32604;
				goto __exit;
			}
		}
	}
__exit:
	return ret;
}
#endif