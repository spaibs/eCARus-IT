#ifndef GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_42
#define GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_42

#include <GtfTypes/GtfStdDef.h>

#ifndef DP_CONTEXT_ID
    #define DP_CONTEXT_ID ((uint8_t) 0x2a)
#else
    #error "Including multiple datapool headers may lead to ambiguous ID definitions!"
#endif

#define DP_CONTEXT_FINGERPRINT "21905ca2cdf7a395e039672d550fe738"

//! @brief Type : list string
#define DP_ID_ExtIn_Battery_ChargeInfo ((uint32_t) 0x0000000C)

//! @brief Type : list int32_t
#define DP_ID_ExtIn_Battery_ChargeState ((uint32_t) 0x0000000B)

//! @brief Type : string
#define DP_ID_ExtIn_Battery_DurchschnittSpannung ((uint32_t) 0x00000006)

//! @brief Type : string
#define DP_ID_ExtIn_Battery_HoechsteZelle ((uint32_t) 0x00000007)

//! @brief Type : string
#define DP_ID_ExtIn_Battery_NiedrigsteZelle ((uint32_t) 0x00000008)

//! @brief Type : string
#define DP_ID_ExtIn_Battery_StackSpannung ((uint32_t) 0x00000009)

//! @brief Type : string
#define DP_ID_ExtIn_Battery_Temperatur ((uint32_t) 0x0000000A)

//! @brief Type : bool
#define DP_ID_ExtIn_Indicator_Fern_IsOn ((uint32_t) 0x00000000)

//! @brief Type : bool
#define DP_ID_ExtIn_Indicator_Left_IsOn ((uint32_t) 0x00000001)

//! @brief Type : bool
#define DP_ID_ExtIn_Indicator_Light_IsOn ((uint32_t) 0x00000002)

//! @brief Type : bool
#define DP_ID_ExtIn_Indicator_Right_IsOn ((uint32_t) 0x00000003)

//! @brief Type : bool
#define DP_ID_ExtIn_Indicator_Warning_IsOn ((uint32_t) 0x00000004)

//! @brief Type : int32_t
#define DP_ID_ExtIn_VehicleMode_CurrentMode ((uint32_t) 0x00000005)

#endif //GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_42
