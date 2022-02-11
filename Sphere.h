#ifndef SPHERE_H
#define SPHERE_H
#pragma once
/* A class used to instantiate spheres as objects.
*  Spheres were chosen as the first object as it is really very easy to tell if a given point is on the surface of a sphere.
*/



#include "Hittable.h"

template <typename T>
class Sphere : public Hittable<T>
{
private:
	Vector3D<T>						m_center;
	T								m_radius;
	std::shared_ptr<Material<T>>	m_material;

public:
	Sphere(Vector3D<T> inCenter, T inRadius, std::shared_ptr<Material<T>> inMat) :m_center(inCenter), m_radius(inRadius), m_material(inMat) {}


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
	bool isHit(const Ray<T>& inRay, T t_min, T t_max, HitRecord<T>& inRec) const override {
		Vector3D<T> originToCenter = inRay.origin() - m_center;
		auto a = inRay.direction().lengthSquared();
		auto h = originToCenter.innerProduct(inRay.direction());					//Where h = b/2 as per our substitution.
		auto c = originToCenter.lengthSquared() - m_radius * m_radius;
		auto discriminant = h * h - a * c;

		//If there is no collision, return false
		if (discriminant < 0)return false;

		//Otherwise return the nearest solution which lies between t_min and t_max
		auto discrimRoot = sqrt(discriminant);
		auto solution = (-h - discrimRoot) / a;			//Take the "default" solution of minus in the plus/minus
		if (solution < t_min || t_max < solution) {		//If it's out of range...
			solution = (-h + discrimRoot) / a;			//take the other solution.
			if (solution < t_min || t_max < solution) {	//If it's still out of range
				return false;							//Treat it as though no collision occurred.
			}
		}

		//If we get this far then we have a collision in the acceptable range. So set up the hit record.
		inRec.m_interval = solution;
		inRec.m_point = (inRay.at(inRec.m_interval));
		Vector3D<T> outwardNormal = (inRec.m_point - m_center) / m_radius;	//Generate an outwards-facing normal unit vector.
		inRec.setNormalDirection(inRay, outwardNormal);						//And set the normal stored in the record to follow the convention of opposing the direction of the ray.
		inRec.m_materialPtr = m_material;

		return true;
	}

	//For spheres, this is very simple. Every point on the surface of the sphere is exactly one radius away.
	virtual T minDistanceApart() const override {
		return m_radius;
	}

	//The center is member data here so easy to return.
	virtual Vector3D<T> getCenter() const override {
		return m_center;
	}
};
#endif

