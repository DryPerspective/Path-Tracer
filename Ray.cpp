#include "Ray.h"


const Vec3D& Ray::origin() const {
	return m_origin;
}

const Vec3D& Ray::direction() const {
	return m_direction;
}

Vec3D Ray::at(double inDistance) const {
	return m_origin + m_direction.scaledBy(inDistance);
}