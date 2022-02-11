#ifndef RAY_H
#define RAY_H
#pragma once
/* A simple object for each ray to be traced. 
* Each ray contains two key member data - an origin and a direction. The distance along a ray that a given point P is can therefore be calculated as P(t) = A + B(t)
* Where A is the origin, B is the direction, and t is a scalar measure of distance. While a very basic implementation of this equation could just iterate along each ray and look for values of t
* where the collision occurred, it's typically more efficient to substitute the ray equation into the equation for our object and solve for t analytically.
*/


#include "Vector3D.h"


template <typename U>
class Ray
{
private:
	Vector3D<U> m_origin;
	Vector3D<U> m_direction;

public:
	//Basic constructors. Since the only member data are Vector3D and the default constructor for that object will properly initialise the object, we shouldn't need to worry about that here.
	Ray() {}
	Ray(const Vector3D<U>& origin, const Vector3D<U>& direction) :m_origin(origin), m_direction(direction) {}

	//Getters for our encapsulated data.
	const Vector3D<U>& origin() const { return m_origin; }
	const Vector3D<U>& direction() const { return m_direction; }

	//This is the big function. Returning the position along a ray. Follows the equation position = origin + distance * direction
	Vector3D<U> at(U inDistance) const { return m_origin + m_direction.scaledBy(inDistance); }
};

#endif

