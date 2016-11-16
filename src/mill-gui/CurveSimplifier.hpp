#pragma once
#include "glm/glm.hpp"
#include <vector>

namespace fw
{

template <typename TVector, typename TPrecision>
class CurveSimplifier
{
public:
    CurveSimplifier();

    TPrecision getMinimumMergeCosine() const;
    void setMinimumMergeCosine(TPrecision minimum);

    std::vector<TVector> simplify(const std::vector<TVector>& curve) const;

private:
    TPrecision _minimumMergeCosine;
};

template <typename TVector, typename TPrecision>
CurveSimplifier<TVector, TPrecision>::CurveSimplifier():
    _minimumMergeCosine{static_cast<TPrecision>(0.9999)}
{
}

template <typename TVector, typename TPrecision>
TPrecision CurveSimplifier<TVector, TPrecision>::getMinimumMergeCosine() const
{
    return _minimumMergeCosine;
}

template <typename TVector, typename TPrecision>
void CurveSimplifier<TVector, TPrecision>::setMinimumMergeCosine(
    TPrecision minimum
)
{
    _minimumMergeCosine = minimum;
}

template <typename TVector, typename TPrecision>
std::vector<TVector> CurveSimplifier<TVector, TPrecision>::simplify(
    const std::vector<TVector>& curve
) const
{
    if (curve.size() <= 2) { return curve; }

    std::vector<TVector> simplifiedCurve{curve[0]};

    auto lastPoint = curve[0];
    auto insertionCandidate = curve[1];
    auto candidateDirection = glm::normalize(insertionCandidate - lastPoint);

    for (auto i = 2; i < curve.size(); ++i)
    {
        const auto &nextPoint = curve[i];
        if (glm::length(nextPoint - lastPoint) < 10e-6) { continue; }
        auto nextDirection = glm::normalize(nextPoint - lastPoint);
        auto deviation = glm::dot(candidateDirection, nextDirection);

        if (deviation > _minimumMergeCosine)
        {
            insertionCandidate = nextPoint;
            continue;
        }

        simplifiedCurve.push_back(insertionCandidate);
        lastPoint = insertionCandidate;
        insertionCandidate = nextPoint;
        candidateDirection = glm::normalize(insertionCandidate - lastPoint);
    }

    simplifiedCurve.push_back(insertionCandidate);

    return simplifiedCurve;
}

}
