#ifndef GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_43
#define GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_43

#include <GtfTypes/GtfStdDef.h>

#ifndef DP_CONTEXT_ID
    #define DP_CONTEXT_ID ((uint8_t) 0x2b)
#else
    #error "Including multiple datapool headers may lead to ambiguous ID definitions!"
#endif

#define DP_CONTEXT_FINGERPRINT "b8010f7c83c1fbeaac12b5aa32335d91"

//! @brief Type : list string
#define DP_ID_ExtIn_MediaPlayer_TitlelList ((uint32_t) 0x0000000D)

//! @brief Type : float
#define DP_ID_ExtOut_MediaPlayer_Volume ((uint32_t) 0x0000000E)

#endif //GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_43
