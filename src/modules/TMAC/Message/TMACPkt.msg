//**************************************************************************
// * file:        Mac80211Pkt.msg
// *
// * author:      Marc L���bbers
// * copyright:   (c) by 'the Glimmer Twins'
// *              Telecommunication Networks Group
// *              TU-Berlin
// * email:       loebbers@tkn.tu-berlin.de
// **************************************************************************
// * part of:     framework implementation developed by tkn
// *************************************************************************/

cplusplus {{
#include "MacPkt_m.h"
}}


class MacPkt;

//
// Defines all fields of an 802.11 MAC frame
//
packet TMACPkt extends MacPkt
{
    int type;
}
