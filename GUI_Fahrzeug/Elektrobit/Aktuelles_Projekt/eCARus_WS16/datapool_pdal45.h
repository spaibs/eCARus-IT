#ifndef GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_45
#define GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_45

#include <GtfTypes/GtfStdDef.h>

#ifndef DP_CONTEXT_ID
    #define DP_CONTEXT_ID ((uint8_t) 0x2d)
#else
    #error "Including multiple datapool headers may lead to ambiguous ID definitions!"
#endif

#define DP_CONTEXT_FINGERPRINT "05dbaf4c01b2c59cc27f74df2201f7dc"

//! @brief Type : int32_t
#define DP_ID_ExtOut_Navigation_Map_Height ((uint32_t) 0x00000010)

//! @brief Type : int32_t
#define DP_ID_ExtOut_Navigation_Map_Width ((uint32_t) 0x00000011)

//! @brief Type : int32_t
#define DP_ID_ExtOut_Navigation_Map_X ((uint32_t) 0x00000012)

//! @brief Type : int32_t
#define DP_ID_ExtOut_Navigation_Map_Y ((uint32_t) 0x00000013)

#endif //GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_45
