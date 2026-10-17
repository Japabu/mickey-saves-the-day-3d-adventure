#pragma once
// MfProtoFloorInfo: what a floor node holds (MfProtoNode::FloorInfo()).

struct MfProtoFloorInfo {
    MfProtoFloorInfo() { m00 = 0; }
    int m00;    // nonzero: the floor takes no manipulation targets
    int m04;
};
