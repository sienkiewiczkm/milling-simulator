#include "MillPathFormatReader.hpp"
#include "Common.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace common;
using namespace std;

namespace ms
{

PathMovement::PathMovement() :
    type(PathMovementType::Unknown)
{
}

PathMovement::PathMovement(PathMovementType type, glm::vec3 position) :
    type(type),
    position(position)
{
}

MillPathFormatReader::MillPathFormatReader()
{
}

MillPathFormatReader::~MillPathFormatReader()
{
}

const std::vector<PathMovement> &MillPathFormatReader::getMovements() const
{
    return _movements;
}

void MillPathFormatReader::readFromFile(const std::string &filename)
{
    _movements.clear();

    auto fileContents = loadASCIITextFile(filename);

    istringstream iss(fileContents);
    string line;
    while (getline(iss, line))
    {
        int charactersRead = 0;
        int commandNumber = 0;

        if (line[0] == '%') { continue; }
        if (line[0] == 'N')
        {
            ++charactersRead;
            while (line[charactersRead] >= '0' && line[charactersRead] <= '9')
            {
                ++charactersRead;
                commandNumber = (10 * commandNumber)
                    + static_cast<int>(line[charactersRead] - '0');
            }

            if (line[charactersRead] == 'G') // movement
            {
                ++charactersRead;

                int typeId = 0;
                for (int i = 0; i < 2; ++i)
                {
                    typeId = (10*typeId)
                        + static_cast<int>(line[charactersRead]-'0');
                    ++charactersRead;
                }

                if (typeId > 2) { throw string("Unknown movement type"); }

                float f[3] = { 0.0f, 0.0f, 0.0f };
                for (int i = 0; i < 3; ++i)
                {
                    ++charactersRead;
                    int factor = 1;
                    int beginning = charactersRead;
                    int numberLength = 0;
                    if (line[beginning] == '-')
                    {
                        factor = -1;
                        ++beginning;
                    }

                    while ((line[beginning+numberLength] >= '0'
                            && line[beginning+numberLength] <= '9')
                            || line[beginning+numberLength] == '.')
                        ++numberLength;

                    string coordinate = line.substr(beginning, numberLength);
                    f[i] = factor * stod(coordinate);

                    charactersRead = beginning + numberLength;
                }

                auto movementType = typeId == 0
                    ? PathMovementType::FastMovement
                    : PathMovementType::Milling;

                // todo: verify YZ swap
                glm::vec3 position = glm::vec3(f[0], f[2], f[1]);

                _movements.push_back(PathMovement(movementType, position));
            }
        }
    }
}

MillPathFormatWriter::MillPathFormatWriter()
{
}

MillPathFormatWriter::~MillPathFormatWriter()
{
}

void MillPathFormatWriter::writeToFile(
    const std::string &filename,
    const std::vector<PathMovement>& movements
)
{
    int commandId = 3;
    std::ofstream file(filename, std::ios_base::trunc);
    for (const auto& movement: movements)
    {
        file << "N" << commandId++ << "G01"
            << std::setprecision(3) << std::fixed
            << "X" << movement.position.x
            << "Y" << movement.position.z
            << "Z" << movement.position.y
            << std::endl;
    }
    file.close();
}

}
