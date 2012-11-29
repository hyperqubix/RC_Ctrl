
#ifndef RC_CORE_H_
#define RC_CORE_H_

#include <stdint.h>



// This struct identifies the "public" functions
typedef struct RC_CORE_t
{
	void (*Init)(void);
	void (*Update)(void);
	void (*MainLoop)(void);
}RC_CORE_t;

void RC_CORE_Constructor(RC_CORE_t *RC_Core_Obj);

#endif // RC_CORE_H_
