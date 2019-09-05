#include <coordinate3d.h>

void Coordinate3D::receiveXChange(int value)
{
    m_x=value;
    emit publishXChange(value);
}

void Coordinate3D::receiveYChange(int value)
{
    m_y=value;
    emit publishYChange(value);
}

void Coordinate3D::receiveZChange(int value)
{
    m_z=value;
    emit publishZChange(value);
}
