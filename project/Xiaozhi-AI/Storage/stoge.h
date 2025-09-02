#ifndef __STOGE_H__
#define __STOGE_H__
#include <stdint.h>
#define WIFI_SSID_LEN 32
#define WIFI_PASSWORD_LEN 64
#define VOLC_APPID_LEN 64
#define VOLC_ROOMID_LEN 64
#define VOLC_USERID_LEN 64
#define VOLC_BOTID_LEN 64
#define VOLC_TOKEN_LEN 256
#define VOLC_LICENSE_LEN 1024
#define PRODUCT_KEY_LEN 32
#define VOLC_HOSTNAME_LEN 128
#define VOLC_PORT_LEN 5
#define license_key_len 64

#define SG_WIFI_SSID_KEY "_wifi_config_ssid"
#define SG_WIFI_PWD_KEY "_wifi_config_password"
#define volume_key "radio_volume"
#define product_key "_product_key"
#define SG_SERVER_HOST "server_host"
#define SG_SERVER_PORT "server_port"
#define _license_key "_license_key"
#define _serial_number "_serial_number"

int stoge_init();
int stoge_wifi_get(char *ssid, char *pwd);
int stoge_wifi_set(const char *ssid, const char *pwd);
int stoge_volc_get(char *appid, char *roomid, char *userid, char *tokenid);
int stoge_volc_set(const char *appid, const char *roomid, const char *userid, const char *tokenid);
int stoge_netcfg_get(void);
int stoge_netcfg_set(int flag);
int stoge_license_get(char *licenseKey, char *serialNumber);
int stoge_license_set(char *licenseKey, char *serialNumber);
int stoge_productKey_set(const char *productKey);
int stoge_productKey_get(char *productKey);
int stoge_server_hostname_get(char *hostname);
int stoge_server_hostname_set(char *hostname);
int stoge_server_port_get(char *port);
int stoge_server_port_set(char *port);
int stoge_volume_set(uint8_t volume);
int stoge_volume_get(uint8_t *volume);

#endif