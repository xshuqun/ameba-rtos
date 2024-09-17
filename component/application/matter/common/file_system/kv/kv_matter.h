#ifndef _KV_MATTER_H_
#define _KV_MATTER_H_

#include <stdint.h>

#ifdef  __cplusplus
extern  "C" {
#endif  // __cplusplus

#ifndef MAX_KEY_LENGTH
#define MAX_KEY_LENGTH 128
#endif

/*============================================================================*
  *                                Functions
  *============================================================================*/
int rt_kv_deinit(void);
int32_t rt_kv_get_length(const char *key);

/** @} */ /* End of group KV */

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // _KV_MATTER_H_
