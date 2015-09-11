#ifndef GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_45
#define GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_45

#include <GtfTypes/GtfStdDef.h>

#ifndef DP_CONTEXT_ID
    #define DP_CONTEXT_ID ((uint8_t) 0x2d)
#else
    #error "Including multiple datapool headers may lead to ambiguous ID definitions!"
#endif

#define DP_CONTEXT_FINGERPRINT "646e6d6eff262381832a86605d532346"

//! @brief Type : int32_t
#define DP_ID_ExtOut_Navigation_Map_Height ((uint32_t) 0x0000000D)

//! @brief Type : int32_t
#define DP_ID_ExtOut_Navigation_Map_Width ((uint32_t) 0x0000000E)

//! @brief Type : int32_t
#define DP_ID_ExtOut_Navigation_Map_X ((uint32_t) 0x0000000F)

//! @brief Type : int32_t
#define DP_ID_ExtOut_Navigation_Map_Y ((uint32_t) 0x00000010)

#endif //GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_45
