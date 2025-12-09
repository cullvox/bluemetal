#pragma once


namespace bl {

struct Voxel {
    constexpr Voxel(uint32_t type, uint32_t);

    uint16_t type : 10;
    uint16_t something: 4;
    uint16_t value;
};

} /* namespace ts */