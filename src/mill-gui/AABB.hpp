#pragma once

#include "glm/glm.hpp"

namespace fw
{

template <typename TVector>
class AABB
{
public:
    AABB();
    AABB(const TVector& min, const TVector& max);
    ~AABB();

    bool isValid() const;
    bool contains(const TVector& vec) const;
    AABB<TVector> intersect(const AABB<TVector> &rhs) const;

    TVector min;
    TVector max;
};

template <typename TVector>
AABB<TVector>::AABB():
    min{},
    max{}
{
}

template <typename TVector>
AABB<TVector>::AABB(const TVector& min, const TVector& max):
    min{min},
    max{max}
{
}

template <typename TVector>
AABB<TVector>::~AABB()
{
}

template <typename TVector>
bool AABB<TVector>::isValid() const
{
    return glm::all(glm::lessThanEqual(min, max));
}

template <typename TVector>
bool AABB<TVector>::contains(const TVector& vec) const
{
    return glm::all(glm::lessThanEqual(min, vec))
        && glm::all(glm::lessThanEqual(vec, max));
}

template <typename TVector>
AABB<TVector> AABB<TVector>::intersect(const AABB<TVector> &rhs) const
{
    return AABB<TVector>{
        glm::max(min, rhs.min),
        glm::min(max, rhs.max)
    };
}

}
