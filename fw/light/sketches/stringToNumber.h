#if !defined(STRINGTONUMBER_H)
#define STRINGTONUMBER_H

#include <stddef.h>
#include <stdint.h>

#if defined (__cplusplus)
extern "C" {
#endif

	typedef enum 
	{
		stnrOk,
		stnrBadArgument,
		stnrUnsupported,
		stnrSyntaxError,
		stnrOverflow,
		stnrUnderflow,
		stnrInternalError
	} StringToNumResult_t;

	StringToNumResult_t tryStringToFloat(const char * const pText, size_t textLen, void * const pResult, size_t resultSize, char decimalSeparator);
	StringToNumResult_t tryStringToInt(const char * const pText, size_t textLen, void * const pResult, size_t resultSize);
	StringToNumResult_t tryStringToUint(const char * const pText, size_t textLen, void * const pResult, size_t resultSize, unsigned	int base);

#if defined(__cplusplus)
} // extern "C"
#endif

#endif // STRINGTONUMBER_H