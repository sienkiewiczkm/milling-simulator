#pragma once
#include <iostream>

#include "AABB.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

#include <array>
#include <memory>
#include <tuple>
#include <vector>

namespace fw
{

template <typename TVector, typename TElement>
class PointQuadtree
{
public:
    PointQuadtree(AABB<TVector> region);
    ~PointQuadtree();

    int getNumElements() const;
    bool containsPoint(TVector position) const;
    bool intersects(const AABB<TVector> &region) const;
    bool addElement(TVector position, const std::shared_ptr<TElement>& element);
    std::vector<std::shared_ptr<TElement>> findElements(
        const AABB<TVector> &region
    ) const;

protected:
    void createSubtrees();
    bool addElementToSubtree(
        TVector position,
        const std::shared_ptr<TElement>& element
    );

    void appendElementsInRegion(
        std::vector<std::shared_ptr<TElement>> &collection,
        const AABB<TVector> &region
    ) const;

private:
    int _subelements;
    AABB<TVector> _aabb;
    bool _subtreesInitialized;
    std::array<std::shared_ptr<PointQuadtree>, 4> _subtrees;
    using ElementTuple = std::tuple<TVector, std::shared_ptr<TElement>>;
    ElementTuple _nodeElement;
};

template <typename TVector, typename TElement>
PointQuadtree<TVector, TElement>::PointQuadtree(AABB<TVector> region):
    _aabb{region},
    _subelements{0},
    _subtreesInitialized{false}
{
}

template <typename TVector, typename TElement>
PointQuadtree<TVector, TElement>::~PointQuadtree()
{
}

template <typename TVector, typename TElement>
int PointQuadtree<TVector, TElement>::getNumElements() const
{
    return _subelements;
}

template <typename TVector, typename TElement>
bool PointQuadtree<TVector, TElement>::containsPoint(TVector position) const
{
    return _aabb.contains(position);
}

template <typename TVector, typename TElement>
bool PointQuadtree<TVector, TElement>::intersects(
    const AABB<TVector> &region
) const
{
    return _aabb.intersect(region).isValid();
}

template <typename TVector, typename TElement>
bool PointQuadtree<TVector, TElement>::addElement(
    TVector position,
    const std::shared_ptr<TElement>& element
)
{
    if (!containsPoint(position)) { return false; }
    ++_subelements;

    if (_subelements == 1)
    {
        _nodeElement = std::make_tuple(position, element);
        return true;
    }

    createSubtrees();

    if (_subelements == 2)
    {
        addElementToSubtree(
            std::get<0>(_nodeElement),
            std::get<1>(_nodeElement)
        );

        _nodeElement = ElementTuple();
    }

    return addElementToSubtree(position, element);
}

template <typename TVector, typename TElement>
std::vector<std::shared_ptr<TElement>>
        PointQuadtree<TVector, TElement>::findElements(
    const AABB<TVector> &region
) const
{
    std::vector<std::shared_ptr<TElement>> outputVector;
    appendElementsInRegion(outputVector, region);
    return outputVector;
}

template <typename TVector, typename TElement>
void PointQuadtree<TVector, TElement>::createSubtrees()
{
    if (_subtreesInitialized) { return; }

    auto dy = (_aabb.max.y - _aabb.min.y) / 2;
    auto dx = (_aabb.max.x - _aabb.min.x) / 2;

    for (auto i = 0; i < 4; ++i)
    {
        auto y = i / 2;
        auto x = i % 2;

        AABB<TVector> aabb;
        _subtrees[i] = std::make_shared<PointQuadtree<TVector, TElement>>(
            AABB<TVector>{
                _aabb.min + TVector{x * dx, y * dy},
                _aabb.min + TVector{(x+1)*dx, (y+1)*dy}
            }
        );
    }

    _subtreesInitialized = true;
}

template <typename TVector, typename TElement>
bool PointQuadtree<TVector, TElement>::addElementToSubtree(
    TVector position,
    const std::shared_ptr<TElement>& element
)
{
    for (auto i = 0; i < 4; ++i)
    {
        if (_subtrees[i]->addElement(position, element))
        {
            return true;
        }
    }

    return false;
}

template <typename TVector, typename TElement>
void PointQuadtree<TVector, TElement>::appendElementsInRegion(
    std::vector<std::shared_ptr<TElement>> &collection,
    const AABB<TVector> &region
) const
{
    if (!intersects(region)) { return; }

    if (_subelements == 1)
    {
        auto position = std::get<0>(_nodeElement);
        if (region.contains(position))
        {
            collection.push_back(
                std::get<1>(_nodeElement)
            );
        }
        return;
    }

    if (!_subtreesInitialized) { return; }
    for (const auto &subtree: _subtrees)
    {
        subtree->appendElementsInRegion(collection, region);
    }
}

}
