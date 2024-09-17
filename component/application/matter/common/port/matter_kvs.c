/************************** 
* Matter KVS Related 
**************************/
#include "platform_stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "string.h"
#include "stdbool.h"
#include "kv.h"
#include "chip_porting.h"

#if CONFIG_ENABLE_DCT_ENCRYPTION
#include "mbedtls/aes.h"
#endif

#define MATTER_KVS_MAX_VARIABLE_SIZE 400 // max size of the variable value, 400 bytes variable

#if CONFIG_ENABLE_DCT_ENCRYPTION
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    mbedtls_aes_context aes;

    // key length 32 bytes for 256 bit encrypting, it can be 16 or 24 bytes for 128 and 192 bits encrypting mode
    unsigned char key[] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

#define DCT_REGION_1 0
#define DCT_REGION_2 1

    int32_t dct_encrypt(unsigned char *input_to_encrypt, int input_len, unsigned char *encrypt_output)
    {
        return 0;
    }

    int32_t dct_decrypt(unsigned char *input_to_decrypt, int input_len, unsigned char *decrypt_output)
    {
        return 0;
    }

    int32_t dct_set_encrypted_variable(dct_handle_t *dct_handle, char *variable_name, char *variable_value, uint16_t variable_value_length, uint8_t region)
    {
        return 0;
    }

    int32_t dct_get_encrypted_variable(dct_handle_t *dct_handle, char *variable_name, char *buffer, uint16_t *buffer_size, uint8_t region)
    {
        return 0;
    }

#else
#error "MBEDTLS_CIPHER_MODE_CTR must be enabled to perform DCT flash encryption" 
#endif // MBEDTLS_CIPHER_MODE_CTR
#endif

void modifyKey(char* tempKey, const char* key, int keyLen)
{
    memcpy(tempKey, key, keyLen);
    tempKey[keyLen] = '\0';    
    for (int i = 0; i < keyLen; i++) // substitute slash with dash, slash will cause error during assignment
    {
        if (tempKey[i] == '/')
        {
            tempKey[i] = '-';
        }
    }
}

s32 initPref(void)
{
    s32 ret = rt_kv_init();
    if (ret < 0) {
        ret = DCT_ERROR;
    } else {
        DiagPrintf("initPref success\r\n");
        ret = DCT_SUCCESS;
    }
    return ret;
}

s32 deinitPref(void)
{
    s32 ret = rt_kv_deinit();
    if (ret < 0) {
        ret = DCT_ERROR;
    } else {
        DiagPrintf("deinitPref success\r\n");
        ret = DCT_SUCCESS;
    }
    return ret;
}

s32 registerPref()
{
    return 0;
}

s32 registerPref2()
{
    return 0;
}

s32 clearPref()
{
    return 0;
}

s32 clearPref2()
{
    return 0;
}

s32 deleteKey(const char *domain, const char *key)
{
    char* tempKey;
    int keyLen = strlen(key);
    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen); 
    s32 ret = rt_kv_delete(tempKey);
    if (ret != 0) { //remove success code is 0
        if (ret == -2) {
            ret = DCT_ERR_NOT_FIND;
        } else {
            ret = DCT_ERROR;
        }
    } else {
        DiagPrintf("deleteKey %s success.\r\n", tempKey);
        ret = DCT_SUCCESS;
    }
    free(tempKey);
    return ret;
}

bool checkExist(const char *domain, const char *key)
{
    char* tempKey;
    int keyLen = strlen(key);
    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen); 
    s32 ret = -1;
    u8 *str = malloc(sizeof(u8) * MATTER_KVS_MAX_VARIABLE_SIZE); // use the bigger buffer size
    ret = rt_kv_get(tempKey, str, sizeof(u32));
    if (ret > 0) {
        DiagPrintf("checkExist key=%s found.\n", tempKey);
        goto exit;
    }
    ret = rt_kv_get(tempKey, str, sizeof(u64));
    if (ret > 0) {
    DiagPrintf("checkExist key=%s found.\n", tempKey);
        goto exit;
    }
    ret = rt_kv_get(tempKey, str, MATTER_KVS_MAX_VARIABLE_SIZE);
    if (ret > 0) {
        DiagPrintf("checkExist key=%s found.\n", tempKey);
        goto exit;
    }
exit:
    free(tempKey);
    free(str);
    return (ret > 0) ? true : false;
}

s32 setPref_new(const char *domain, const char *key, u8 *value, size_t byteCount)
{
    char* tempKey;
    int keyLen = strlen(key);
    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen); 
    s32 ret = rt_kv_set(tempKey, value, byteCount);
    if (ret <= 0) { //0 is inclusive because 0 bytes were written
        ret = DCT_ERROR;
    } else {
        DiagPrintf("setPref_new %s success, write %d bytes.\r\n", tempKey, byteCount);
        ret = DCT_SUCCESS;
    }
    free(tempKey);
    return ret;
}

s32 getPref_bool_new(const char *domain, const char *key, u8 *val)
{
    char* tempKey;
    int keyLen = strlen(key);
    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen); 
    s32 ret = rt_kv_get(tempKey, val, sizeof(u8));
    if (ret <= 0) { //0 is inclusive because 0 bytes were read
        if (ret == -1) {
            ret = DCT_ERR_NOT_FIND;
        } else {
            ret = DCT_ERROR;
        }
    } else {
        DiagPrintf("getPref_bool_new %s success, read %d bytes.\r\n", tempKey, sizeof(u8));
        ret = DCT_SUCCESS;
    }
    free(tempKey);
    return ret;
}

s32 getPref_u32_new(const char *domain, const char *key, u32 *val)
{
    char* tempKey;
    int keyLen = strlen(key);
    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);  
    s32 ret = rt_kv_get(tempKey, val, sizeof(u32));
    if (ret <= 0) { //0 is inclusive because 0 bytes were read
        if (ret == -1) {
            ret = DCT_ERR_NOT_FIND;
        } else {
            ret = DCT_ERROR;
        }
    } else {
        DiagPrintf("getPref_u32_new %s success, read %d bytes.\r\n", tempKey, sizeof(u32));
        ret = DCT_SUCCESS;
    }
    free(tempKey);
    return ret;
}

s32 getPref_u64_new(const char *domain, const char *key, u64 *val)
{
    char* tempKey;
    int keyLen = strlen(key);
    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);  
    s32 ret = rt_kv_get(tempKey, val, sizeof(u64));
    if (ret <= 0) { //0 is inclusive because 0 bytes were read
        if (ret == -1) {
            ret = DCT_ERR_NOT_FIND;
        } else {
            ret = DCT_ERROR;
        }
    } else {
        DiagPrintf("getPref_u64_new %s success, read %d bytes.\r\n", tempKey, sizeof(u64));
        ret = DCT_SUCCESS;
    }
    free(tempKey);
    return ret;
}

s32 getPref_str_new(const char *domain, const char *key, char * buf, size_t bufSize, size_t *outLen)
{
    char* tempKey;
    int keyLen = strlen(key);
    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
    memset(buf, 0, bufSize);  
    *outLen = rt_kv_get_length(tempKey);
    s32 ret = rt_kv_get(tempKey, buf, *outLen);
    if (ret <= 0) { //0 is inclusive because 0 bytes were read
        if (ret == -1) {
            ret = DCT_ERR_NOT_FIND;
        } else {
            ret = DCT_ERROR;
        }
    } else {
        DiagPrintf("getPref_str_new %s success, read %d bytes.\r\n", tempKey, bufSize);
        ret = DCT_SUCCESS;
    }
    free(tempKey);
    return ret;
}

s32 getPref_bin_new(const char *domain, const char *key, u8 * buf, size_t bufSize, size_t *outLen)
{
    char* tempKey;
    int keyLen = strlen(key);
    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
    memset(buf, 0, bufSize);
    *outLen = rt_kv_get_length(tempKey);
    s32 ret = rt_kv_get(tempKey, buf, *outLen);
    if (ret <= 0) { //0 is inclusive because 0 bytes were read
        if (ret == -1) {
            ret = DCT_ERR_NOT_FIND;
        } else {
            ret = DCT_ERROR;
        }
    } else {
        DiagPrintf("getPref_bin_new %s success, read %d bytes.\r\n", tempKey, bufSize);
        ret = DCT_SUCCESS;
    }
    free(tempKey);
    return ret;
}

#ifdef __cplusplus
}
#endif
