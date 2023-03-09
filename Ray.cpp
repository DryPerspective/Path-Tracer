#include "Ray.h"


const dp::PhysicsVector<3>& Ray::origin() const {
	return m_origin;
}

const dp::PhysicsVector<3>& Ray::direction() const {
	return m_direction;
}

dp::PhysicsVector<3> Ray::at(double inDistance) const {
	return m_origin + m_direction.scaledBy(inDistance);
}