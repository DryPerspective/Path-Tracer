#include "Ray.h"


const Physics::PhysicsVector<3>& Ray::origin() const {
	return m_origin;
}

const Physics::PhysicsVector<3>& Ray::direction() const {
	return m_direction;
}

Physics::PhysicsVector<3> Ray::at(double inDistance) const {
	return m_origin + m_direction.scaledBy(inDistance);
}