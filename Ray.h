#ifndef RAY_H
#define RAY_H
#pragma once
/* A simple object for each ray to be traced. 
* Each ray contains two key member data - an origin and a direction. The distance along a ray that a given point P is can therefore be calculated as P(t) = A + B(t)
* Where A is the origin, B is the direction, and t is a scalar measure of distance. While a very basic implementation of this equation could just iterate along each ray and look for values of t
* where the collision occurred, it's typically more efficient to substitute the ray equation into the equation for our object and solve for t analytically.
*/


#include "Vec3D.h"


class Ray
{

private:
	Vec3D m_origin;
	Vec3D m_direction;

public:
	//Basic constructors. Since the only member data are Vec3D and the default constructor for that object will properly initialise the object, we shouldn't need to worry about that here.
	Ray() {}
	Ray(const Vec3D& origin, const Vec3D& direction) :m_origin(origin), m_direction(direction) {}

	//Getters for our encapsulated data.
	const Vec3D& origin() const; 
	const Vec3D& direction() const;

	//This is the big function. Returning the position along a ray. Follows the equation position = origin + distance * direction
	Vec3D at(double inDistance) const; 
};

#endif

