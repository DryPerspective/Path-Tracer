#ifndef SPHERE_H
#define SPHERE_H
#pragma once
/* A class used to instantiate spheres as objects.
*  Spheres were chosen as the first object as it is really very easy to tell if a given point is on the surface of a sphere.
*/



#include "Hittable.h"


class Sphere : public Hittable
{
	

private:
	Physics::PhysicsVector<3>		m_center;
	double							m_radius;
	std::shared_ptr<Material>		m_material;

public:
	Sphere(Physics::PhysicsVector<3> inCenter, double inRadius, std::shared_ptr<Material> inMat) :m_center(inCenter), m_radius(inRadius), m_material(inMat) {}


	//This function calculates whether an incoming ray has hit the sphere, and updates the HitRecord with information about the impact.
	// In short, the distance from point P to the center C of a sphere is given by (P-C). Thus for points exactly on the surface of the sphere, (P-C)*(P-C)=r^2, by substituting for the geometric sphere equation.
	// NB: In this case, the * operator I'm commenting is the dot product.
	// Expanding point P as a particular position along a ray (P(t) = A + tB, with A as the origin, B as the direction, and t as the distance along), (A + tB - C)*(A + tB - C) = r^2.
	// We want to solve for t as it is the only unknown (C and r and essential sphere properties and we know A and B for a given ray). Expanding the equation yields:
	// (B*B)t^2 + 2t(b*(A-C)) + (A-C)*(A-C) - r^2 = 0.
	// This is a simple quadratic equation and we can use the trusty quadratic formula to solve it. 
	// If no solution exists, the ray does not intersect the sphere. If exactly one solution exists, the ray grazes the surface of the sphere. If two exist, then the ray passes through the sphere (solutions are "entry" and "exit" points)
	//
	// We can simplify our code to cut down on unnecessary calls if we so desire. We know any vector dotted with itself is equal to the length squared of that vector.
	// Similarly, subsituting b = 2h, then all factors of 2 in the traditional quadratic formula cancel out. From a pure math sense this plays a little fast and loose but it works here.
	bool isHit(const Ray& inRay, double t_min, double t_max, HitRecord& inRec) const override;

	//For spheres, this is very simple. Every point on the surface of the sphere is exactly one radius away.
	virtual double minDistanceApart() const override;

	//The center is member data here so easy to return.
	virtual Physics::PhysicsVector<3> getCenter() const override;
};
#endif

