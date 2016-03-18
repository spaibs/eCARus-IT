#ifndef GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_0
#define GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_0

#include <GtfTypes/GtfStdDef.h>

#ifndef DP_CONTEXT_ID
    #define DP_CONTEXT_ID ((uint8_t) 0x00)
#else
    #error "Including multiple datapool headers may lead to ambiguous ID definitions!"
#endif

#define DP_CONTEXT_FINGERPRINT "b3a86a5ff4b6a01a0f8869985142cdfe"

//! @brief Type : int32_t
#define DP_ID_Battery_Percentage ((uint32_t) 0x00000015)

//! @brief Type : string
#define DP_ID_Bottom_Text_specials ((uint32_t) 0x0000001A)

//! @brief Type : color
#define DP_ID_Color_Highlight_Bronze ((uint32_t) 0x0000003B)

//! @brief Type : color
#define DP_ID_Color_Highlight_Gold ((uint32_t) 0x0000003C)

//! @brief Type : color
#define DP_ID_Color_Highlight_Touch ((uint32_t) 0x0000003D)

//! @brief Type : int32_t
#define DP_ID_Color_colorationIndex ((uint32_t) 0x00000016)

//! @brief Type : string
#define DP_ID_Content_BatteryManager_Current ((uint32_t) 0x0000001B)

//! @brief Type : string
#define DP_ID_Content_BatteryManager_HighestV ((uint32_t) 0x0000001C)

//! @brief Type : string
#define DP_ID_Content_BatteryManager_LowestV ((uint32_t) 0x0000001D)

//! @brief Type : string
#define DP_ID_Content_BatteryManager_StackVoltage ((uint32_t) 0x0000001E)

//! @brief Type : string
#define DP_ID_Content_BatteryManager_Temperature ((uint32_t) 0x0000001F)

//! @brief Type : string
#define DP_ID_Content_Gestures_InfoText ((uint32_t) 0x00000020)

//! @brief Type : list string
#define DP_ID_Content_Info_Battery_Item ((uint32_t) 0x0000003E)

//! @brief Type : list string
#define DP_ID_Content_Info_Battery_Value ((uint32_t) 0x0000003F)

//! @brief Type : string
#define DP_ID_Content_Info_General_Name ((uint32_t) 0x00000021)

//! @brief Type : string
#define DP_ID_Content_Info_General_Text ((uint32_t) 0x00000022)

//! @brief Type : list string
#define DP_ID_Content_Info_Technical_Item ((uint32_t) 0x00000040)

//! @brief Type : list string
#define DP_ID_Content_Info_Technical_Value ((uint32_t) 0x00000041)

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

//! @brief Type : list string
#define DP_ID_ExtIn_MediaPlayer_TitlelList ((uint32_t) 0x0000000D)

//! @brief Type : int32_t
#define DP_ID_ExtIn_VehicleMode_CurrentMode ((uint32_t) 0x00000005)

//! @brief Type : float
#define DP_ID_ExtOut_MediaPlayer_Volume ((uint32_t) 0x0000000E)

//! @brief Type : void
#define DP_ID_ExtOut_MediaPlayer_VolumeSet ((uint32_t) 0x00000043)

//! @brief Type : int32_t
#define DP_ID_ExtOut_Navigation_Map_Height ((uint32_t) 0x0000000F)

//! @brief Type : int32_t
#define DP_ID_ExtOut_Navigation_Map_Width ((uint32_t) 0x00000010)

//! @brief Type : int32_t
#define DP_ID_ExtOut_Navigation_Map_X ((uint32_t) 0x00000011)

//! @brief Type : int32_t
#define DP_ID_ExtOut_Navigation_Map_Y ((uint32_t) 0x00000012)

//! @brief Type : list string
#define DP_ID_Gesture_Command ((uint32_t) 0x00000042)

//! @brief Type : void
#define DP_ID_Gesture_Control ((uint32_t) 0x00000044)

//! @brief Type : string
#define DP_ID_Gesture_Down_Button ((uint32_t) 0x00000023)

//! @brief Type : string
#define DP_ID_Gesture_Left_Button ((uint32_t) 0x00000024)

//! @brief Type : string
#define DP_ID_Gesture_Right_Button ((uint32_t) 0x00000025)

//! @brief Type : string
#define DP_ID_Gesture_UP_Button ((uint32_t) 0x00000026)

//! @brief Type : float
#define DP_ID_MediaPlayer_Volume ((uint32_t) 0x00000019)

//! @brief Type : bool
#define DP_ID_Media_isPlayingMusic ((uint32_t) 0x00000013)

//! @brief Type : string
#define DP_ID_MenuItem_Button_Gesture ((uint32_t) 0x00000027)

//! @brief Type : string
#define DP_ID_MenuItem_Button_Info_Battery ((uint32_t) 0x00000028)

//! @brief Type : string
#define DP_ID_MenuItem_Button_Info_General ((uint32_t) 0x00000029)

//! @brief Type : string
#define DP_ID_MenuItem_Button_Info_Sponsors ((uint32_t) 0x0000002A)

//! @brief Type : string
#define DP_ID_MenuItem_Button_Info_Technical ((uint32_t) 0x0000002B)

//! @brief Type : string
#define DP_ID_MenuItem_Button_Information ((uint32_t) 0x0000002C)

//! @brief Type : string
#define DP_ID_MenuItem_Button_Media ((uint32_t) 0x0000002D)

//! @brief Type : string
#define DP_ID_MenuItem_Button_Navigation ((uint32_t) 0x0000002E)

//! @brief Type : string
#define DP_ID_MenuItem_Button_Radio ((uint32_t) 0x0000002F)

//! @brief Type : string
#define DP_ID_MenuItem_Button_Settings ((uint32_t) 0x00000030)

//! @brief Type : int32_t
#define DP_ID_Menu_Info_CurrentItem_Selected ((uint32_t) 0x00000017)

//! @brief Type : int32_t
#define DP_ID_Menu_Info_CurrentSponsor_Selected ((uint32_t) 0x00000018)

//! @brief Type : string
#define DP_ID_ViewTitle_Home ((uint32_t) 0x00000031)

//! @brief Type : string
#define DP_ID_ViewTitle_Info_Battery ((uint32_t) 0x00000032)

//! @brief Type : string
#define DP_ID_ViewTitle_Info_General ((uint32_t) 0x00000033)

//! @brief Type : string
#define DP_ID_ViewTitle_Info_Sponsors1 ((uint32_t) 0x00000034)

//! @brief Type : string
#define DP_ID_ViewTitle_Info_Sponsors2 ((uint32_t) 0x00000035)

//! @brief Type : string
#define DP_ID_ViewTitle_Info_Technical ((uint32_t) 0x00000036)

//! @brief Type : string
#define DP_ID_ViewTitle_Media ((uint32_t) 0x00000037)

//! @brief Type : string
#define DP_ID_ViewTitle_Settings ((uint32_t) 0x00000038)

//! @brief Type : string
#define DP_ID_ViewTitle_Settings_Language ((uint32_t) 0x00000039)

//! @brief Type : string
#define DP_ID_ViewTitle_Settings_Mode ((uint32_t) 0x0000003A)

//! @brief Type : bool
#define DP_ID_touchIndex ((uint32_t) 0x00000014)

#endif //GTF_INCLUDED_DATAPOOL_IDENTIFIERS_OF_CONTEXT_0
