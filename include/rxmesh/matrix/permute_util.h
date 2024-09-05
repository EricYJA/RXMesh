#pragma once

#include <stdint.h>
#include <algorithm>
#include <vector>

namespace rxmesh {
/**
 * @brief given a permutation array, verify that it is a unique permutation,
 * i.e., every index is assigned to one permutation. This is done by sorting the
 * array and then checking if every entry in the array matches its index.
 */
template <typename T>
bool is_unique_permutation(uint32_t size, T* h_permute)
{
    std::vector<T> permute(size);
    std::memcpy(permute.data(), h_permute, size * sizeof(T));

    std::sort(permute.begin(), permute.end());

    for (T i = 0; i < T(permute.size()); ++i) {
        if (i != permute[i]) {
            return false;
        }
    }

    return true;
}
}  // namespace rxmesh