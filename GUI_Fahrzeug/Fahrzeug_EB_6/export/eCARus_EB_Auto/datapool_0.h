#ifndef GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_0
#define GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_0

#include <GtfTypes/GtfStdDef.h>

#ifndef DP_CONTEXT_ID
    #define DP_CONTEXT_ID ((uint8_t) 0x00)
#else
    #error "Including multiple datapool headers may lead to ambiguous ID definitions!"
#endif

#define DP_CONTEXT_FINGERPRINT "F436ED4E4A979676F472E73C5A09003F"

//! @brief Type : Color
#define DP_ID_Color_Highlight_Touch ((uint32_t) 0x00000000)

//! @brief Type : String
#define DP_ID_Menu_Item_Button_Music ((uint32_t) 0x00000001)

//! @brief Type : Bool
#define DP_ID_ExtIn_Indicator_Left_IsOn ((uint32_t) 0x00000002)

//! @brief Type : Bool
#define DP_ID_ExtIn_Indicator_Right_IsOn ((uint32_t) 0x00000003)

#endif //GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_0
