#ifndef GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_1
#define GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_1

#include <GtfTypes/GtfStdDef.h>

#ifndef DP_CONTEXT_ID
    #define DP_CONTEXT_ID ((uint8_t) 0x01)
#else
    #error "Including multiple datapool headers may lead to ambiguous ID definitions!"
#endif

#define DP_CONTEXT_FINGERPRINT "2d83b7f94ef23121be6c6cb1ea1db9c5"

//! @brief Type : float
#define DP_ID_ExtIn_Rating_Average ((uint32_t) 0x00000000)

#endif //GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_1
