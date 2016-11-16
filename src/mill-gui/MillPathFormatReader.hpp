#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace ms
{

enum class PathMovementType
{
    Unknown,
    FastMovement,
    Milling
};

struct PathMovement
{
    PathMovement();
    PathMovement(PathMovementType type, glm::vec3 position);

    PathMovementType type;
    glm::vec3 position;
};

class MillPathFormatReader
{
public:
    MillPathFormatReader();
    ~MillPathFormatReader();

    void readFromFile(const std::string &filename);
    const std::vector<PathMovement> &getMovements() const;

private:
    std::vector<PathMovement> _movements;
};

class MillPathFormatWriter
{
public:
    MillPathFormatWriter();
    ~MillPathFormatWriter();

    void writeToFile(
        const std::string &filename,
        const std::vector<PathMovement>& movements
    );
};

}
