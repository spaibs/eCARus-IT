#ifndef GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_43
#define GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_43

#include <GtfTypes/GtfStdDef.h>

#ifndef DP_CONTEXT_ID
    #define DP_CONTEXT_ID ((uint8_t) 0x2b)
#else
    #error "Including multiple datapool headers may lead to ambiguous ID definitions!"
#endif

#define DP_CONTEXT_FINGERPRINT "2dd81b111dcdfb41cafa6ab3a650f91b"

//! @brief Type : list string
#define DP_ID_ExtIn_MediaPlayer_TitlelList ((uint32_t) 0x0000000E)

//! @brief Type : float
#define DP_ID_ExtOut_MediaPlayer_Volume ((uint32_t) 0x0000000F)

#endif //GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_43
