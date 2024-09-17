/**
 * @brief High resolution sleep.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/nanosleep.html
 *
 * @note rmtp is ignored, as signals are not implemented.
 */
#ifndef MATTER_UTILS_H_
#define MATTER_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <mbedtls/ecp.h>

typedef struct
{
    uint8_t value[68];
    size_t len;
} FactoryDataString;

typedef struct
{
    uint8_t value[602];
    size_t len;
} CertDataString;

typedef struct
{
    uint8_t value[256];
    size_t len;
} VerifierDataString;

typedef struct  
{
    int passcode; 
    int discriminator;    
    int spake2_it;
    FactoryDataString spake2_salt;
    VerifierDataString spake2_verifier;
} CommissionableData;

typedef struct 
{
    CertDataString dac_cert;
    FactoryDataString dac_key;
    CertDataString pai_cert;
    CertDataString cd;
} DeviceAttestationCredentials;

typedef struct 
{
    int vendor_id;
    FactoryDataString vendor_name;
    int product_id;
    FactoryDataString product_name;
    int hw_ver;
    FactoryDataString hw_ver_string; 
    FactoryDataString mfg_date;
    FactoryDataString serial_num;
    FactoryDataString rd_id_uid;
} DeviceInstanceInfo;

typedef struct
{
    CommissionableData cdata;
    DeviceAttestationCredentials dac;
    DeviceInstanceInfo dii;
} FactoryData;

#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
typedef enum {
    MATTER_DACKEY_KEY_TYPE = 0,
    MATTER_OPKEY_KEY_TYPE,
} matter_key_type;

//Matter Crypto Related
#define MATTER_PUBLIC_KEY_SIZE          65
#define MATTER_DAC_PRIVATE_KEY_LENGTH   32
#define MATTER_SHA256_HASH_LENGTH       32
#define MATTER_P256_FE_LENGTH           32

// Matter Error Code
#define MATTER_NOT_IMPLEMENTED          0x2D
#define MATTER_INVALID_ARGUMENT         0x2F
#define MATTER_ERROR_INTERNAL           0xAC
#define MATTER_ERROR_WELL_UNINITIALIZED 0x1C
#endif /* CONFIG_MATTER_SECURE */

// Functions
int32_t ReadFactory(uint8_t *buffer, uint16_t *pfactorydata_len);
int32_t DecodeFactory(uint8_t *buffer, FactoryData *fdp, uint16_t data_len);

#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
int matter_get_signature(uint8_t *pub_buf, size_t pub_size, const unsigned char * msg, size_t msg_size , unsigned char * signature);
size_t matter_gen_new_csr(uint8_t *out_csr, size_t csr_length);
int matter_get_publickey(uint8_t * pubkey, size_t pubkey_size);
int matter_ecdsa_sign_msg(const unsigned char * msg, size_t msg_size , unsigned char * signature);
int matter_serialize(uint8_t *output_buf, size_t output_size);
int matter_deserialize(uint8_t *buf, size_t size);
#endif /* CONFIG_MATTER_SECURE */

#ifdef __cplusplus
}
#endif

#endif // MATTER_UTILS_H_
