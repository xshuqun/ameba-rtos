#include "cmsis.h"
#include "FreeRTOS.h"
#include "platform_stdlib.h"
#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#else
#include "mbedtls/config.h"
#endif
#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"
#include "mbedtls/pk.h"
#include "mbedtls/version.h"

#if defined(CONFIG_SSL_CLIENT_PRIVATE_IN_TZ) && (CONFIG_SSL_CLIENT_PRIVATE_IN_TZ == 1)
extern const char *client_key_s;
#endif

static void* _calloc(size_t count, size_t size)
{
	void *ptr = pvPortMalloc(count * size);
	if(ptr)	memset(ptr, 0, count * size);
	return ptr;
}

#define _free		vPortFree

static int _random(void *p_rng, unsigned char *output, size_t output_len)
{
	/* To avoid gcc warnings */
	( void ) p_rng;

	static unsigned int seed = 0;
	if(seed == 0) {
		extern u32 RandSeedTZ;
		seed = RandSeedTZ;
		srand(seed);
	}

	int rand_num = 0;
	while(output_len) {
		int r = rand();
		if(output_len > sizeof(int)) {
			memcpy(&output[rand_num], &r, sizeof(int));
			rand_num += sizeof(int);
			output_len -= sizeof(int);
		}
		else {
			memcpy(&output[rand_num], &r, output_len);
			rand_num += output_len;
			output_len = 0;
		}
	}

	return 0;
}

#if defined(__ICCARM__)
void (__cmse_nonsecure_call *ns_device_mutex_lock)(uint32_t) = NULL;
void (__cmse_nonsecure_call *ns_device_mutex_unlock)(uint32_t) = NULL;
#else
void __attribute__((cmse_nonsecure_call)) (*ns_device_mutex_lock)(uint32_t) = NULL;
void __attribute__((cmse_nonsecure_call)) (*ns_device_mutex_unlock)(uint32_t) = NULL;
#endif

IMAGE3_ENTRY_SECTION
void NS_ENTRY secure_set_ns_device_lock(
	void (*device_mutex_lock_func)(uint32_t),
	void (*device_mutex_unlock_func)(uint32_t))
{
#if defined(__ICCARM__)
	ns_device_mutex_lock = cmse_nsfptr_create((void (__cmse_nonsecure_call *)(uint32_t)) device_mutex_lock_func);
	ns_device_mutex_unlock = cmse_nsfptr_create((void (__cmse_nonsecure_call *)(uint32_t)) device_mutex_unlock_func);
#else
	ns_device_mutex_lock = cmse_nsfptr_create((void __attribute__((cmse_nonsecure_call)) (*)(uint32_t)) device_mutex_lock_func);
	ns_device_mutex_unlock = cmse_nsfptr_create((void __attribute__((cmse_nonsecure_call)) (*)(uint32_t)) device_mutex_unlock_func);
#endif
}


IMAGE3_ENTRY_SECTION
void NS_ENTRY secure_mbedtls_ssl_conf_rng(mbedtls_ssl_config *conf, void *p_rng)
{
	mbedtls_ssl_conf_rng(conf, _random, p_rng);
}

IMAGE3_ENTRY_SECTION
int NS_ENTRY secure_mbedtls_platform_set_calloc_free(void)
{
#if defined(MBEDTLS_VERSION_NUMBER) && ( MBEDTLS_VERSION_NUMBER==0x02100300 || MBEDTLS_VERSION_NUMBER==0x021C0000)
	mbedtls_platform_setup(NULL);
#endif
	return 	mbedtls_platform_set_calloc_free(_calloc, _free);
}

#if defined(CONFIG_SSL_CLIENT_PRIVATE_IN_TZ) && (CONFIG_SSL_CLIENT_PRIVATE_IN_TZ == 1)
IMAGE3_ENTRY_SECTION
uint8_t *NS_ENTRY secure_mbedtls_pk_parse_key(void)
{

	mbedtls_pk_context *client_pk = (mbedtls_pk_context *) mbedtls_calloc(1, sizeof(mbedtls_pk_context));

	if(client_pk) {
		mbedtls_pk_init(client_pk);

		if(mbedtls_pk_parse_key(client_pk, (unsigned char const *)client_key_s, strlen(client_key_s) + 1, NULL, 0) != 0) {
			DiagPrintf("\n\r ERROR: mbedtls_pk_parse_key \n\r");
			goto error;
		}
	}
	else {
		DiagPrintf("\n\r ERROR: mbedtls_calloc \n\r");
		goto error;
	}

	return client_pk;
error:
	if(client_pk) {
		mbedtls_pk_free(client_pk);
		mbedtls_free(client_pk);
	}

	return NULL;
}
#endif

IMAGE3_ENTRY_SECTION
void NS_ENTRY secure_mbedtls_pk_free(mbedtls_pk_context *pk)
{
	mbedtls_pk_free(pk);
	mbedtls_free(pk);
}

IMAGE3_ENTRY_SECTION
int NS_ENTRY secure_mbedtls_pk_can_do(const mbedtls_pk_context *ctx, mbedtls_pk_type_t type)
{
	return mbedtls_pk_can_do(ctx, type);
}

IMAGE3_ENTRY_SECTION
unsigned char NS_ENTRY secure_mbedtls_ssl_sig_from_pk(mbedtls_pk_context *pk)
{
#if defined(MBEDTLS_RSA_C)
	if( mbedtls_pk_can_do( pk, MBEDTLS_PK_RSA ) )
		return( MBEDTLS_SSL_SIG_RSA );
#endif
#if defined(MBEDTLS_ECDSA_C)
	if( mbedtls_pk_can_do( pk, MBEDTLS_PK_ECDSA ) )
		return( MBEDTLS_SSL_SIG_ECDSA );
#endif
	return( MBEDTLS_SSL_SIG_ANON );
}

struct secure_mbedtls_pk_sign_param {
	mbedtls_pk_context *ctx;
	mbedtls_md_type_t md_alg;
	unsigned char *hash;
	size_t hash_len;
	unsigned char *sig;
	size_t *sig_len;
	int (*f_rng)(void *, unsigned char *, size_t);
	void *p_rng;
};

IMAGE3_ENTRY_SECTION
int NS_ENTRY secure_mbedtls_pk_sign(struct secure_mbedtls_pk_sign_param *param)
{

	return mbedtls_pk_sign(param->ctx, param->md_alg, param->hash, param->hash_len,
			param->sig, param->sig_len, _random, param->p_rng);
}

#if defined(ENABLE_AMAZON_COMMON)
#include "mbedtls/threading_alt.h"
#include "mbedtls/threading.h"

IMAGE3_ENTRY_SECTION
mbedtls_pk_type_t NS_ENTRY secure_mbedtls_pk_get_type(const mbedtls_pk_context *ctx)
{
	return mbedtls_pk_get_type(ctx);
}

void s_mbedtls_mutex_init( mbedtls_threading_mutex_t * mutex )
{

}

void s_mbedtls_mutex_free( mbedtls_threading_mutex_t * mutex )
{

}

int s_mbedtls_mutex_lock( mbedtls_threading_mutex_t * mutex )
{
	__disable_irq();
	return 0;
}

int s_mbedtls_mutex_unlock( mbedtls_threading_mutex_t * mutex )
{
	__enable_irq();
	return 0;
}

void (*mbedtls_mutex_init)( mbedtls_threading_mutex_t * ) = s_mbedtls_mutex_init;
void (*mbedtls_mutex_free)( mbedtls_threading_mutex_t * ) = s_mbedtls_mutex_free;
int (*mbedtls_mutex_lock)( mbedtls_threading_mutex_t * ) = s_mbedtls_mutex_lock;
int (*mbedtls_mutex_unlock)( mbedtls_threading_mutex_t * ) = s_mbedtls_mutex_unlock;
#endif

#if defined(CONFIG_MATTER) && CONFIG_MATTER
#include "matter_utils.h"
#include <mbedtls/x509_csr.h>
#include "mbedtls/aes.h"

__weak const uint8_t kSecureDacPrivateKey[] = {
0x76, 0x49, 0x9f, 0xda, 0xf4, 0x30, 0x10, 0x66, 0x36, 0x97, 0x0a, 0x42, 0x88, 0x83, 0x97, 0x1f,
0x5f, 0xff, 0xc7, 0x0d, 0x08, 0xd3, 0xd2, 0x51, 0x5a, 0x17, 0x14, 0x5a, 0xba, 0x3f, 0x95, 0xa4,
};

unsigned char test_key[] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
unsigned char test_iv[] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
mbedtls_aes_context aes_ctx;

mbedtls_ecp_keypair DacKey;
mbedtls_ecp_keypair OpKey;

int keyInitialized = 0;

/**
 * @brief Clears the Key Pair associated with the specified Matter Key Type.
 * 
 * This function clears the key pair associated with the specified Matter Key Type.
 * 
 * Parameters:
 *  - key_type: The type of Matter Key for which the key pair needs to be cleared. It can be either 
 *              MATTER_DACKEY_KEY_TYPE for Device Attestation Certificate (DAC) key pair or 
 *              MATTER_OPKEY_KEY_TYPE for Operational key pair.
 * 
 * Return:
 *  - None
 * 
 * Upon successful execution, the key pair associated with the specified Matter Key Type is freed, and 
 * the 'keyInitialized' flag is set to 0 to indicate that the key pair is no longer initialized.
 */
IMAGE3_ENTRY_SECTION
int NS_ENTRY matter_secure_clear_keypair(matter_key_type key_type)
{
	if(keyInitialized)
	{
		switch(key_type) {
			case MATTER_DACKEY_KEY_TYPE:
				mbedtls_ecp_keypair_free(&DacKey);
				break;
			case MATTER_OPKEY_KEY_TYPE:
				mbedtls_ecp_keypair_free(&OpKey);
				break;
			default:
				break;
		}
		keyInitialized = 0;
	}
}

/**
 * @brief Calculate the SHA-256 hash for a message.
 * 
 * This function calculates the SHA-256 hash for the input message 'msg' and stores the result in the output buffer 'out_buf'.
 * 
 * Parameters:
 *  - msg: Pointer to the message for which the hash needs to be calculated.
 *  - msg_size: Size of the message.
 *  - out_buf: Pointer to the buffer where the SHA-256 hash will be stored.
 * 
 * Returns:
 *  - 0 on success.
 *  - A negative value if an error occurs.
 */
IMAGE3_ENTRY_SECTION
int NS_ENTRY matter_hash_sha256(const uint8_t * msg, size_t msg_size, uint8_t * out_buf)
{
	// Check if 'msg' or 'out_buf' pointers are nullptr
	if((msg == NULL) || (out_buf == NULL))
	{
		DiagPrintf("ERROR: %s nullptr \n\r", __FUNCTION__);
		return MATTER_INVALID_ARGUMENT;
	}

	// Calculate the SHA-256 hash for the input message
	return mbedtls_sha256_ret(msg, msg_size, out_buf, 0);
}

/**
 * @brief Calculate the SHA-256 hash for a message and sign it using ECDSA.
 * 
 * This function calculates the SHA-256 hash for the input message 'msg' and signs it using ECDSA with the specified key type.
 * The resulting signature is stored in the 'signature' buffer.
 * 
 * Parameters:
 *  - key_type: The type of key to be used for signing (MATTER_DACKEY_KEY_TYPE or MATTER_OPKEY_KEY_TYPE).
 *  - msg: Pointer to the message to be signed.
 *  - msg_size: Size of the message.
 *  - signature: Pointer to the buffer where the signature will be stored.
 * 
 * Returns:
 *  - 0 on success.
 *  - A negative value if an error occurs.
 * 
 * Notes:
 *  - This function first calculates the SHA-256 hash of the input message.
 *  - It then sets up the ECDSA context with the specified key type (DacKey or OpKey).
 *  - The ECDSA signature is computed and stored in the 'signature' buffer.
 *  - Error handling includes printing error messages and returning appropriate error codes.
 */
IMAGE3_ENTRY_SECTION
int NS_ENTRY matter_secure_ecdsa_sign_msg(matter_key_type key_type, const unsigned char * msg, size_t msg_size, unsigned char *signature)
{
	int result;
	uint8_t digest[MATTER_SHA256_HASH_LENGTH];

	// Calculate the SHA-256 hash of the message
	memset(&digest[0], 0, sizeof(digest));
	result = matter_hash_sha256(msg, msg_size, &digest[0]);
	if(result != 0)
	{
		DiagPrintf("ERROR: %s hash failed result=%d \n\r", __FUNCTION__, result);
		return MATTER_INVALID_ARGUMENT;
	}

#if defined(MBEDTLS_ECDSA_C)
	mbedtls_mpi r, s;
	mbedtls_mpi_init(&r);
	mbedtls_mpi_init(&s);

	mbedtls_ecdsa_context ecdsa_ctxt;
	mbedtls_ecdsa_init(&ecdsa_ctxt);

	// Set up the ECDSA context with the specified key type
	switch(key_type) {
		case MATTER_DACKEY_KEY_TYPE:
			result = mbedtls_ecdsa_from_keypair(&ecdsa_ctxt, &DacKey);
			break;
		case MATTER_OPKEY_KEY_TYPE:
			result = mbedtls_ecdsa_from_keypair(&ecdsa_ctxt, &OpKey);
			break;
	}

	if(result != 0)
	{
		DiagPrintf("ERROR: %s setup ECDSA failed result=%d \n\r", __FUNCTION__, result);
		return result;
	}

	// Compute the ECDSA signature
	result = mbedtls_ecdsa_sign(&ecdsa_ctxt.grp, &r, &s, &ecdsa_ctxt.d,
								(unsigned char const *)digest, sizeof(digest), _random, NULL);
	if(result != 0)
	{
		DiagPrintf("ERROR: %s ECDSA sign failed result=%d \n\r", __FUNCTION__, result);
		return result;
	}

	if(!(mbedtls_mpi_size(&r) <= MATTER_P256_FE_LENGTH) && (mbedtls_mpi_size(&s) <= MATTER_P256_FE_LENGTH))
	{
		return MATTER_ERROR_INTERNAL;
	}

	// Write the signature into the 'signature' buffer
	result = mbedtls_mpi_write_binary(&r, signature, MATTER_P256_FE_LENGTH);
	if(result != 0)
	{
		DiagPrintf("ERROR: %s write binary failed result=%d \n\r", __FUNCTION__, result);
		return result;
	}

	result = mbedtls_mpi_write_binary(&s, signature + MATTER_P256_FE_LENGTH, MATTER_P256_FE_LENGTH);
	if(result != 0)
	{
		DiagPrintf("ERROR: %s write binary failed result=%d \n\r", __FUNCTION__, result);
		return result;
	}

	mbedtls_ecdsa_free(&ecdsa_ctxt);
	mbedtls_mpi_free(&s);
	mbedtls_mpi_free(&r);
#else
	result = MATTER_NOT_IMPLEMENTED;
#endif
	return result;
}

/**
 * @brief Generate a Certificate Signing Request (CSR) for a CASE Session using the Operational Keypair.
 * 
 * This function generates a CSR for a CASE Session using the Operational Keypair.
 * 
 * Parameters:
 *  - out_csr: Pointer to the buffer where the CSR will be stored.
 *  - csr_length: Length of the CSR buffer.
 * 
 * Returns:
 *  - The length of the generated CSR on success.
 *  - An error code if the CSR generation process fails.
 */
IMAGE3_ENTRY_SECTION
int NS_ENTRY matter_secure_new_csr(uint8_t *out_csr, size_t csr_length)
{
	int result = 0;
	mbedtls_ecp_group_id group = MBEDTLS_ECP_DP_SECP256R1;

	matter_secure_clear_keypair(MATTER_OPKEY_KEY_TYPE);

	// Initialize a new Operational Keypair
	matter_secure_opkey_init_keypair();

#if defined(MBEDTLS_X509_CSR_WRITE_C)
	size_t out_length;

	mbedtls_x509write_csr csr;
	mbedtls_x509write_csr_init(&csr);

	// Set up the CSR with the Operational Keypair
	mbedtls_pk_context pk;
	pk.pk_info = mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY);
	pk.pk_ctx = &OpKey;
	if(pk.pk_info == NULL)
	{
		return MATTER_ERROR_INTERNAL;
	}

	mbedtls_x509write_csr_set_key(&csr, &pk);

	// Set the message digest algorithm to SHA-256
	mbedtls_x509write_csr_set_md_alg(&csr, MBEDTLS_MD_SHA256);

	// Set the subject name of the CSR
	result = mbedtls_x509write_csr_set_subject_name(&csr, "O=CSR");
	if(result != 0)
	{
		DiagPrintf("Error: %s set subject failed, result=%d\n", __FUNCTION__, result);
		return MATTER_ERROR_INTERNAL;
	}

	// Generate the CSR and store it in the output buffer 'out_csr'
	result = mbedtls_x509write_csr_der(&csr, out_csr, csr_length, _random, NULL);
	if(result <= 0)
	{
		DiagPrintf("Error: %s write csr der failed, length=%d \n\r", __FUNCTION__, result);
		return MATTER_ERROR_INTERNAL;
	}

	out_length = (size_t) result;
	result	 = 0;

	// Check for CSR length
	if(out_length > csr_length)
	{
		DiagPrintf("Error: %s length error, length=%d \n\r", __FUNCTION__, out_length);
		return MATTER_ERROR_INTERNAL;
	}

	if (csr_length != out_length)
	{
		// mbedTLS API writes the CSR at the end of the provided buffer.
		// Let's move it to the start of the buffer.
		size_t offset = csr_length - out_length;
		memmove(out_csr, &out_csr[offset], out_length);
	}

	csr_length = out_length;

exit:
	mbedtls_x509write_csr_free(&csr);

	return csr_length;
#else
	DiagPrintf("Error: %s MBEDTLS_X509_CSR_WRITE_C is not enabled. CSR cannot be created \n\r", __FUNCTION__);
	return MATTER_NOT_IMPLEMENTED;
#endif
}

/**
 * @brief Generate a new Operational Keypair for a CASE Session.
 * 
 * This function generates a new Operational Keypair for use in a CASE (Commissioning and Authentication of IoT Devices) Session.
 * 
 * Returns:
 *  - 0 on success.
 *  - An error code indicating failure during the key generation process.
 * 
 * Notes:
 *  - The function initializes a new Operational Keypair structure ('OpKey') and clears any existing keypair of the same type.
 *  - It generates a new keypair using the elliptic curve specified by 'group' (SECP256R1).
 *  - If an error occurs during the key generation process, the function prints an error message and returns the error code.
 */
IMAGE3_ENTRY_SECTION
int NS_ENTRY matter_secure_opkey_init_keypair()
{
	int result = 0;
	mbedtls_ecp_group_id group = MBEDTLS_ECP_DP_SECP256R1;

	matter_secure_clear_keypair(MATTER_OPKEY_KEY_TYPE);

	// Initialize the Operational Keypair
	mbedtls_ecp_keypair_init(&OpKey);

	// Generate a new keypair using the specified elliptic curve group
	result = mbedtls_ecp_gen_key(group, &OpKey, _random, NULL);
	if(result != 0)
	{
		DiagPrintf("Error: %s gen key failed, result=%d \n\r", __FUNCTION__, result);
		goto exit;
	}

	keyInitialized = 1;
	return result;

exit:
	matter_secure_clear_keypair(MATTER_OPKEY_KEY_TYPE);
	return result;
}

/**
 * @brief Encrypts the Operational Private Key for Storing into DCT.
 * 
 * This function encrypts the Operational Private Key using AES-CTR encryption to prepare it for storing into DCT. It utilizes a predefined AES key and initialization vector (IV) to perform
 * the encryption process securely.
 * 
 * Parameters:
 *  - buf: Pointer to the buffer containing the original Operational Private Key.
 *  - size: Size of the buffer containing the original Operational Private Key.
 * 
 * Returns:
 *  - 0 on success.
 *  - A negative value if an error occurs during encryption.
 * 
 * Upon successful encryption, the encrypted Operational Private Key is copied back to the input buffer 'buf'.
 */
IMAGE3_ENTRY_SECTION
int NS_ENTRY matter_secure_encrypt_key(uint8_t *buf, size_t size)
{
	int result = 0;
	size_t nc_off = 0;
	unsigned char nonce_counter[16] = {0};
	unsigned char stream_block[16] = {0};

	// Initialize AES context
	mbedtls_aes_init(&aes_ctx);
	
	// Set encryption key
	mbedtls_aes_setkey_enc(&aes_ctx, test_key, 256);

	// Allocate memory for decrypted_privkey
	unsigned char *encrypted_privkey = (unsigned char*) pvPortMalloc(size);
	if (encrypted_privkey == NULL)
	{
		result = -1;
		goto exit;
	}

	// Initialize variables and copy IV
	memset(encrypted_privkey, 0, size);
	memcpy(nonce_counter, test_iv, sizeof(nonce_counter));

	// Decrypt the encrypted Operational private key using AES-CTR
	result = mbedtls_aes_crypt_ctr(&aes_ctx, size, &nc_off, nonce_counter, stream_block, buf, encrypted_privkey);
	if (result !=0)
	{
		DiagPrintf("ERROR: %s privkey decrypt failed! result=%d \n\r", __FUNCTION__, result);
		goto exit; 
	}

	// Copy decrypted key back to input buffer
	memset(buf, 0, size);
	memcpy(buf, encrypted_privkey, size);

exit:
	if (encrypted_privkey)
	{
		vPortFree(encrypted_privkey);
	}

	mbedtls_aes_free(&aes_ctx);
	return result;
}

/**
 * @brief Retrieves the public key of the Operational Keypair.
 * 
 * This function retrieves the public key of the Operational Keypair (`OpKey`) and writes it into 
 * the provided buffer 'pubkey'.
 * 
 * Parameters:
 *  - pubkey: Pointer to the buffer where the public key will be written.
 *  - pubkey_size: Size of the buffer to accommodate the public key.
 * 
 * Returns:
 *  - 0 on success.
 *  - A negative value if an error occurs during the retrieval process.
 */
IMAGE3_ENTRY_SECTION
int NS_ENTRY matter_secure_get_opkey_pub(uint8_t * pubkey, size_t pubkey_size)
{
	int result = 0;
	size_t temp_size = 0;

	// Write the public key of the Operational Keypair into the buffer
	result = mbedtls_ecp_point_write_binary(&OpKey.grp, &OpKey.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, 
											&temp_size, (unsigned char *) pubkey, pubkey_size);
	if(result != 0)
	{
		DiagPrintf("ERROR: %s write public key failed, result=%d \n\r", __FUNCTION__, result);
		return result;
	}

	// Check if the size of the written public key matches the expected size
	if(temp_size != pubkey_size)
	{
		return -1;
	}

	return result;
}

/**
 * @brief Retrieves and encrypts the private key of the Operational Keypair.
 * 
 * This function retrieves the original private key of the Operational Keypair (`OpKey`) and writes it into
 * the provided buffer `privkey`. It then encrypt the private key using a secure encryption function.
 * 
 * Parameters:
 *  - privkey: Pointer to the buffer where the encrypted private key will be written.
 *  - privkey_size: Size of the buffer to accommodate the encrypted private key.
 * 
 * Returns:
 *  - 0 on success.
 *  - A negative value if an error occurs during the retrieval or encryption process.
 */
IMAGE3_ENTRY_SECTION
int NS_ENTRY matter_secure_get_opkey_priv(uint8_t * privkey, size_t privkey_size)
{
	int result = 0;

	// Retrieve the encrypted private key of the Operational Keypair and write it into the buffer 'privkey'
	result = mbedtls_mpi_write_binary(&OpKey.d, privkey, privkey_size);
	if (result != 0)
	{
		DiagPrintf("ERROR: %s get private key failed! result=%d \n\r", __FUNCTION__, result);
		matter_secure_clear_keypair(MATTER_OPKEY_KEY_TYPE);
		goto exit;
	}

	// Decrypt the encrypted private key
	result = matter_secure_encrypt_key(privkey, privkey_size);
	if (result != 0)
	{
		DiagPrintf("ERROR: %s encrypt private key failed! result=%d \n\r", __FUNCTION__, result);
		matter_secure_clear_keypair(MATTER_OPKEY_KEY_TYPE);
		goto exit;
	}

exit:

	return result;
}

/**
 * @brief Prepare Operational Keypair
 * 
 * When the function SignWithStoredOpKey() is called, it retrieves the Operational Keypair from the key storage. 
 * The buffer 'buf' will contain the Operationalpublic key and the encrypted private key of the Operational Keypair.
 * 
 * Whenever the key storage reads and requires the Operational Keypair, it invokes matter_secure_get_opkey():
 *    1. It copies the public key from the buffer 'buf' into the Opkey
 *    2. It decrypts the private key and copies the decrypted private key into the Opkey.
 * 
 * It's important to note that the decrypted private key remains within the secure context and is not exposed to 
 * non-secure environments. The Operational Keypair (Opkey) is exclusively used within a secure context.
 */
IMAGE3_ENTRY_SECTION
int NS_ENTRY matter_secure_get_opkey(uint8_t *buf, size_t size)
{
	int result = 0;
	size_t nc_off = 0;
	unsigned char nonce_counter[16] = {0};
	unsigned char stream_block[16] = {0};

	mbedtls_aes_init(&aes_ctx);
	mbedtls_aes_setkey_enc(&aes_ctx, test_key, 256);

	//initialize decrypted_privkey for store decrypted private key
	unsigned char *decrypted_privkey = (unsigned char*) pvPortMalloc(MATTER_P256_FE_LENGTH);
	if (decrypted_privkey == NULL)
	{
		result = -1;
		goto exit;
	}

	unsigned char *pubkey = (unsigned char*) pvPortMalloc(MATTER_PUBLIC_KEY_SIZE);
	if (pubkey == NULL)
	{
		result = -1;
		goto exit;
	}

	//clears decrypted_privkey and pubkey
	memset(decrypted_privkey, 0, MATTER_P256_FE_LENGTH);
	memset(pubkey, 0, MATTER_PUBLIC_KEY_SIZE);

	//copy operational public key from buf into pubkey
	memcpy(pubkey, buf, MATTER_PUBLIC_KEY_SIZE);
	//copy IV into nonce_counter
	memcpy(nonce_counter, test_iv, sizeof(nonce_counter));

	//decrypt operational private key
	result = mbedtls_aes_crypt_ctr(&aes_ctx, MATTER_P256_FE_LENGTH, &nc_off, nonce_counter, stream_block, buf + MATTER_PUBLIC_KEY_SIZE, decrypted_privkey);
	if (result !=0)
	{
		DiagPrintf("ERROR: %s decryption failed! result=%d \n\r", __FUNCTION__, result);
		goto exit; 
	}

	//Initialize Opkey
	matter_secure_clear_keypair(MATTER_OPKEY_KEY_TYPE);
	mbedtls_ecp_keypair_init(&OpKey);
	keyInitialized = 1;
	//set Operational keypair ecp group as MBEDTLS_ECP_DP_SECP256R1
	result = mbedtls_ecp_group_load(&OpKey.grp, MBEDTLS_ECP_DP_SECP256R1);
	if (result != 0)
	{
		DiagPrintf("ERROR: %s load grp failed! result=%d \n\r", result);
		goto exit; 
	}
	// set Opkey public key
	result = mbedtls_ecp_point_read_binary(&OpKey.grp, &OpKey.Q, (const unsigned char *)pubkey, MATTER_PUBLIC_KEY_SIZE);
	if (result != 0)
	{
		DiagPrintf("ERROR: %s set pubkey failed! result=%d \n\r", result);
		goto exit; 
	}
	// set decrypted priv key into Opkey
	result = mbedtls_mpi_read_binary(&OpKey.d, decrypted_privkey, MATTER_P256_FE_LENGTH);
	if (result != 0)
	{
		DiagPrintf("ERROR: %s set privkey failed! result=%d \n\r", result);
		goto exit; 
	}
exit:
	if (decrypted_privkey)
	{
		vPortFree(decrypted_privkey);
	}
	if (pubkey)
	{
		vPortFree(pubkey);
	}

	mbedtls_aes_free(&aes_ctx);
	return result;
}

/**
 * @brief Serializes the Operational Keypair for storage in the Device Configuration Table (DCT).
 * 
 * This function serializes the Operational Keypair by retrieving its public key and encrypting its private key.
 * The serialized data is then written into the provided output buffer 'output_buf'.
 * 
 * Parameters:
 *  - output_buf: Pointer to the buffer where the serialized data will be written.
 *  - output_size: Size of the output buffer.
 * 
 * Returns:
 *  - 0 on success.
 *  - A negative value if an error occurs during the serialization process.
 * 
 * Notes:
 *  - The Operational Keypair (Opkey) is assumed to have been previously generated and initialized.
 *  - This function first retrieves the public and private keys of the Operational Keypair.
 *  - It then writes the public key followed by the encrypted private key into the output buffer.
 *  - If an error occurs during the retrieval or serialization process, the function prints an error message,
 *    clears the Operational Keypair, and returns the error code.
 */
IMAGE3_ENTRY_SECTION
int NS_ENTRY matter_secure_serialize(uint8_t *output_buf, size_t output_size)
{
	int result = 0;
	uint8_t privkey[MATTER_P256_FE_LENGTH];
	uint8_t pubkey[MATTER_PUBLIC_KEY_SIZE];

	// Get the public key of the Operational Keypair
	result = matter_secure_get_opkey_pub(pubkey, sizeof(pubkey));
	if (result != 0)
	{
		DiagPrintf("ERROR: %s get public key failed! result=%d \n\r", __FUNCTION__, result);
		goto exit;
	}

	// Get the private key of the Operational Keypair
	result = matter_secure_get_opkey_priv(privkey, sizeof(privkey));
	if (result != 0)
	{
		DiagPrintf("ERROR: %s get private key failed! result=%d \n\r", __FUNCTION__, result);
		goto exit;
	}

	// Write the public key to the output buffer
	memcpy(output_buf, pubkey, sizeof(pubkey));

	// Write the encrypted private key to the output buffer
	memcpy(output_buf + sizeof(pubkey), privkey, sizeof(privkey));

exit:
	return result;
}

/**
 * @brief Deserialize Device Attestation Certificate (DAC) keypair.
 * 
 * This function deserializes the DAC keypair by loading the necessary parameters into the DacKey structure.
 * 
 * Parameters:
 *  - pub_buf: Pointer to the buffer containing the public key.
 *  - pub_size: Size of the public key buffer.
 * 
 * Returns:
 *  - 0 on success.
 *  - A negative value if an error occurs during the deserialization process.
 */
IMAGE3_ENTRY_SECTION
int NS_ENTRY matter_secure_deserialize(uint8_t *pub_buf, size_t pub_size)
{
	int result = 0;

	// Initialize the DAC keypair structure
	mbedtls_ecp_keypair_init(&DacKey);

	keyInitialized = 1;

	// Set the ECP group for the DAC keypair to MBEDTLS_ECP_DP_SECP256R1
	result = mbedtls_ecp_group_load(&DacKey.grp, MBEDTLS_ECP_DP_SECP256R1);
	if (result != 0)
	{
		DiagPrintf("ERROR: %s load grp failed! result=%d \n\r", __FUNCTION__, result);
		matter_secure_clear_keypair(MATTER_DACKEY_KEY_TYPE);
		goto exit;
	}

	// Set the DAC public key into DacKey
	result = mbedtls_ecp_point_read_binary(&DacKey.grp, &DacKey.Q, (const unsigned char *)pub_buf, pub_size);
	if (result != 0)
	{
		DiagPrintf("ERROR: %s set public key failed! result=%d \n\r", __FUNCTION__, result);
		matter_secure_clear_keypair(MATTER_DACKEY_KEY_TYPE);
		goto exit;
	}

	// Set the DAC private key into DacKey
	result = mbedtls_mpi_read_binary(&DacKey.d, kSecureDacPrivateKey, MATTER_DAC_PRIVATE_KEY_LENGTH);
	if (result != 0)
	{
		DiagPrintf("ERROR: %s set private key failed! result=%d \n\r", __FUNCTION__, result);
		matter_secure_clear_keypair(MATTER_DACKEY_KEY_TYPE);
		goto exit;
	}

exit:

	return result;
}

/**
 * @brief Initialize Device Attestation Certificate (DAC) keypair
 * 
 * This function initializes the DAC keypair.
 * The private key is obtained from main_s.c in the array `kSecureDacPrivateKey`.
 * The public key is obtained from the buffer 'pub_buf'.
 * 
 * Parameters:
 *  - pub_buf: Pointer to the buffer containing the public key.
 *  - pub_size: Size of the public key buffer.
 * 
 * Returns:
 *  - 0 on success.
 *  - A negative value if an error occurs during the initialization process.
 */
IMAGE3_ENTRY_SECTION
int NS_ENTRY matter_secure_dac_init_keypair(uint8_t *pub_buf, size_t pub_size)
{
	int result = 0;

	// Deserialize the DAC keypair
	result = matter_secure_deserialize(pub_buf, pub_size);
	if(result != 0) {
		DiagPrintf("Error: %s deserialize failed result=%d \n\r", __FUNCTION__, result);
		return result;
	}

	return result;
}
#endif /* CONFIG_MATTER */
