#include "ZigZagPathGenerator.hpp"
#include "GeometricIntersections.hpp"
#include <iostream>
#include <algorithm>
#include "Common.hpp"

#include "glm/gtx/string_cast.hpp"

namespace ms
{

ZigZagPathGenerator::ZigZagPathGenerator():
    _safeHeight{50.5}
{
}

ZigZagPathGenerator::~ZigZagPathGenerator()
{
}


void ZigZagPathGenerator::setRejectionMode(ZigZagRejectionMode rejectionMode)
{
    _rejectionMode = rejectionMode;
}

void ZigZagPathGenerator::setBaseHeight(double baseHeight)
{
    _baseHeight = baseHeight;
}

void ZigZagPathGenerator::setCuttingToolRadius(double radius)
{
    _toolRadius = radius;
}

void ZigZagPathGenerator::setWorkingArea(
    glm::dvec3 blockSize,
    glm::dmat4 worldMatrix
)
{
    _blockSize = blockSize;
    _blockWorldMatrix = worldMatrix;
    _blockWorldMatrixInv = glm::inverse(worldMatrix);
}

void ZigZagPathGenerator::setInbetweenDistance(double distance)
{
    _inbetweenDistance = distance;
}

void ZigZagPathGenerator::setObjectContours(
    const std::vector<glm::dvec3>& objectContour
)
{
    _objectContour = objectContour;
}

void ZigZagPathGenerator::bake()
{
    _segments.clear();
    _paths.clear();
    _linkMap.clear();
    createUncutZigZags();
    subdivideZigZags();
    buildLinkMap();
    followZigZag();
}

void ZigZagPathGenerator::createUncutZigZags()
{
    auto offsetFactor = 1.3;
    for (auto z = -_blockSize.z / 2 - offsetFactor * _toolRadius;
        z < _blockSize.z / 2 + offsetFactor * _toolRadius;
        z += _inbetweenDistance)
    {
        _segments.push_back({});
        _segments.back().push_back({
            glm::dvec2{-_blockSize.x / 2 - offsetFactor * _toolRadius, z},
            glm::dvec2{+_blockSize.x / 2 + offsetFactor * _toolRadius, z},
            -1,
            -1,
            1
        });
    }
}

void ZigZagPathGenerator::subdivideZigZags()
{
    for (auto line = 0; line < _segments.size(); ++line)
    {
        const auto& segmentLine = _segments[line];
        std::vector<SegmentTuple> subdividedLine;
        for (const auto &segment: segmentLine)
        {
            auto from = std::get<0>(segment);
            auto to = std::get<1>(segment);

            std::vector<std::tuple<double, int>> intersections;
            for (auto i = 0; i < _objectContour.size() - 1; ++i)
            {

                auto intersection = fw::intersectSegments<glm::dvec2, double>(
                    from,
                    to,
                    {_objectContour[i].x, _objectContour[i].z},
                    {_objectContour[i+1].x, _objectContour[i+1].z}
                );

                if (intersection.kind == fw::GeometricIntersectionKind::None)
                {
                    continue;
                }

                auto intersectionPoint = from + (to-from) * intersection.t0;

                intersections.push_back({intersectionPoint.x, i});
            }

            intersections.push_back({from.x, -1});
            intersections.push_back({to.x, -1});

            std::sort(std::begin(intersections), std::end(intersections));

            for (auto i = 0; i < intersections.size(); i += 2)
            {
                subdividedLine.push_back({
                    glm::dvec2{std::get<0>(intersections[i]), from.y},
                    glm::dvec2{std::get<0>(intersections[i+1]), from.y},
                    std::get<1>(intersections[i]),
                    std::get<1>(intersections[i+1]),
                    1
                });
            }
        }

        _segments[line] = subdividedLine;
    }
}

std::vector<PathMovement> ZigZagPathGenerator::buildPaths()
{
    std::vector<PathMovement> movements;

    for (const auto& path: _paths)
    {
        auto ep = findSafeEntryPoint(path[0]);

        movements.push_back({
            PathMovementType::Milling,
            {ep.x, _safeHeight, ep.z}
        });

        movements.push_back({
            PathMovementType::Milling, ep
        });

        for (const auto& position: path)
        {
            movements.push_back({PathMovementType::Milling, position});
        }

        movements.push_back({
            PathMovementType::Milling,
            {path.back().x, _safeHeight, path.back().z}
        });
    }

    return movements;
}

void ZigZagPathGenerator::buildLinkMap()
{
    std::vector<std::tuple<int, int, int, int>> rawLinks;

    for (auto line = 0; line < _segments.size(); ++line)
    {
        const auto& segmentLine = _segments[line];
        for (auto i = 0; i < segmentLine.size(); ++i)
        {
            const auto& segment = segmentLine[i];
            auto leftIndex = std::get<2>(segment);
            auto rightIndex = std::get<3>(segment);

            if (leftIndex >= 0)
            {
                rawLinks.push_back({leftIndex, line, i, 1});
            }
            if (rightIndex >= 0)
            {
                rawLinks.push_back({rightIndex, line, i, 1});
            }
        }
    }

    std::sort(std::begin(rawLinks), std::end(rawLinks));
    _linkMap = rawLinks;
}

void ZigZagPathGenerator::followZigZag()
{
    for (auto line = 0; line < _segments.size(); ++line)
    {
        auto& segmentLine = _segments[line];
        for (auto i = 0; i < segmentLine.size(); ++i)
        {
            if (std::get<4>(segmentLine[i]) == 1)
            {
                followLocalZigZag(line, i);
            }
        }
    }
}

void ZigZagPathGenerator::followLocalZigZag(int line, int segmentId)
{
    std::vector<glm::dvec3> localPath;

    bool leftToRight = true;
    while (line < _segments.size())
    {
        auto& segment = _segments[line][segmentId];
        if (std::get<4>(segment) == 0) { break; }
        std::get<4>(segment) = 0;

        auto from = std::get<0>(segment);
        auto to = std::get<1>(segment);
        auto fromIntersection = std::get<2>(segment);
        auto toIntersection = std::get<3>(segment);

        if (!leftToRight)
        {
            std::swap(from, to);
            std::swap(fromIntersection, toIntersection);
        }

        localPath.push_back({from.x, _baseHeight, from.y});
        localPath.push_back({to.x, _baseHeight, to.y});

        std::cout << "Going from: " << glm::to_string(from)
            << " to " << glm::to_string(to) << std::endl;

        // find lower/higher than toIntersection
        // IF leftToRight = true -> look in ascending order
        // IF == false -> look in descending order

        if (toIntersection == -1)
        {
            ++line;
            if (line >= _segments.size()) { break; }

            if ((segmentId == 0 && leftToRight) || segmentId != 0)
            {
                segmentId = _segments[line].size() - 1;
                leftToRight = false;
            }
            else
            {
                leftToRight = true;
                segmentId = 0;
            }

            continue;
        }

        auto myIntesectionIdx = std::lower_bound(
            std::begin(_linkMap),
            std::end(_linkMap),
            toIntersection,
            [](std::tuple<int, int, int, int> const& x, int target)
            {
                return std::get<0>(x) < target;
            }
        ) - std::begin(_linkMap);

        std::cout << "My intersection index: " <<
            std::get<0>(_linkMap[myIntesectionIdx])
            << " (should be " << toIntersection << ")" << std::endl;

        bool breakout = false;
        auto direction = leftToRight ? -1 : 1;
        auto linksChecked = 1;

        for (linksChecked = 1; linksChecked < _linkMap.size(); ++linksChecked)
        {
            int interid = (myIntesectionIdx + linksChecked * direction)
                % _linkMap.size();
            auto& link = _linkMap[interid];
            auto& available = std::get<3>(link);
            if (available == 1)
            {
                available = 0;

                auto target = std::get<0>(link);
                auto newLine = std::get<1>(link);
                auto newSegmentId = std::get<2>(link);

                std::cout << "Found available contour. "
                    << "Moving to " << target << " (L=" << line
                    << " S=" << newSegmentId << ")" << std::endl;

                followContour(localPath, toIntersection, target, direction);

                line = newLine;
                segmentId = newSegmentId;
                auto newFromIntersection = std::get<2>(
                    _segments[line][segmentId]
                );

                leftToRight = newFromIntersection == target;

                break;
            }
        }

        if (linksChecked >= _linkMap.size())
        {
            break;
        }
    }

    _paths.push_back(localPath);
}

void ZigZagPathGenerator::followContour(
    std::vector<glm::dvec3> &collection,
    int from,
    int to,
    int increase
)
{
    auto current = from;
    auto signedLimit = static_cast<int>(_objectContour.size());
    while (true)
    {
        std::cout << "Following contour " << from << " -> " << to << " inc="
            << increase << " currently = " << current
            << " total = " << _objectContour.size() << std::endl;
        bool willExit = current == to;
        auto contourPt = _objectContour[current];
        collection.push_back({contourPt.x, _baseHeight, contourPt.z});
        current = (current + increase) % signedLimit;
        if (current < 0) current += signedLimit;
        if (willExit) { break;}
    }
}

glm::dvec3 ZigZagPathGenerator::findSafeEntryPoint(
    const glm::dvec3& pos
) const
{
    auto offset = 1.2 * _toolRadius;
    if (_blockSize.x/2 - std::abs(pos.x) < _blockSize.z/2 - std::abs(pos.z))
    {
        return
        {
            (_blockSize.x/2+offset)*common::sgn(pos.x),
            _baseHeight,
            pos.z
        };
    }
    else
    {
        std::cout << pos.y << " vs " << _baseHeight << " ? " << std::endl;
        return
        {
            pos.x,
            _baseHeight,
            (_blockSize.z/2+offset)*common::sgn(pos.z)
        };
    }
}
}
