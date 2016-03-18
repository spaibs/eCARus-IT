#ifndef GTF_INCLUDED_EVENTS_IDENTIFIERS_0x10000_H
#define GTF_INCLUDED_EVENTS_IDENTIFIERS_0x10000_H

#include <GtfTypes/GtfStdDef.h>

//! @brief ExtOut_Navigation_Touch_DoubleTap
//!
//! Parameters : 2
//! 1) tapX::int32_t
//! 2) tapY::int32_t
#define EVENT_GRP_ID_ExtOut_Navigation_Touch_DoubleTap    ((uint32_t) 0x00010000)
#define EVENT_MSG_ID_ExtOut_Navigation_Touch_DoubleTap    ((uint32_t) 0x00000003)

//! @brief ExtOut_Navigation_Touch_Move
//!
//! Parameters : 2
//! 1) deltaX::int32_t
//! 2) deltaY::int32_t
#define EVENT_GRP_ID_ExtOut_Navigation_Touch_Move    ((uint32_t) 0x00010000)
#define EVENT_MSG_ID_ExtOut_Navigation_Touch_Move    ((uint32_t) 0x00000000)

//! @brief ExtOut_Navigation_Touch_Rotate
//!
//! Parameters : 1
//! 1) rotationAngle::float
#define EVENT_GRP_ID_ExtOut_Navigation_Touch_Rotate    ((uint32_t) 0x00010000)
#define EVENT_MSG_ID_ExtOut_Navigation_Touch_Rotate    ((uint32_t) 0x00000001)

//! @brief ExtOut_Navigation_Touch_Zoom
//!
//! Parameters : 1
//! 1) zoomFactor::float
#define EVENT_GRP_ID_ExtOut_Navigation_Touch_Zoom    ((uint32_t) 0x00010000)
#define EVENT_MSG_ID_ExtOut_Navigation_Touch_Zoom    ((uint32_t) 0x00000002)

#endif //GTF_INCLUDED_EVENTS_IDENTIFIERS_0x10000_H

