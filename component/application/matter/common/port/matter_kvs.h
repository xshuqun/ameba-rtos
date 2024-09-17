/**
 * @brief High resolution sleep.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/nanosleep.html
 *
 * @note rmtp is ignored, as signals are not implemented.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <wifi_conf.h>

#ifndef __DCT_ENUM
#define __DCT_ENUM
enum{
  DCT_SUCCESS = 0,			/*!< success */
  DCT_ERROR = -1,				/*!< error */
  DCT_ERR_CRC = -2,			/*!< crc error */
  DCT_ERR_NO_SPACE = -3,		/*!< no space error */
  DCT_ERR_NO_MEMORY = -4,		/*!< alloc memory error */
  DCT_ERR_FLASH_RW = -5,		/*!< flash r/w error */
  DCT_ERR_NOT_FIND = -6,		/*!< not find error */
  DCT_ERR_INVALID = -7,		/*!< invalid operation error */
  DCT_ERR_SIZE_OVER = -8,		/*!< varialbe length over max size error */
  DCT_ERR_MODULE_BUSY = -9,	/*!< module mutex time out */
};
#endif

// for AmebaConfig
s32 initPref(void);
s32 deinitPref(void);
s32 registerPref(void);
s32 registerPref2(void);
s32 clearPref(void);
s32 clearPref2(void);
s32 deleteKey(const char *domain, const char *key);
bool checkExist(const char *domain, const char *key);
s32 setPref_new(const char *domain, const char *key, u8 *value, size_t byteCount);
s32 getPref_bool_new(const char *domain, const char *key, u8 *val);
s32 getPref_u32_new(const char *domain, const char *key, u32 *val);
s32 getPref_u64_new(const char *domain, const char *key, u64 *val);
s32 getPref_str_new(const char *domain, const char *key, char * buf, size_t bufSize, size_t *outLen);
s32 getPref_bin_new(const char *domain, const char *key, u8 * buf, size_t bufSize, size_t *outLen);

#ifdef __cplusplus
}
#endif
