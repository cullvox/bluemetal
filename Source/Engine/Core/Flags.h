#pragma once

namespace bl {

    constexpr int bit(int index) {
        return 1 << index; 
    }

    template<typename E>
    using TFlags = std::bitset<E::Count>;

} // namespace bl 
