#ifndef HITTABLE_H
#define HITTABLE_H
#pragma once
/* An abstract class for a generic hittable object.
*/


#include <memory>	//For shared pointers.

#include "Ray.h"

//Forward declare our Material class as we need pointers to it in the HitRecord struct, but we can't #include it this early or we'd have circular depencency issues.
template <typename S>
class Material;

//A struct to hold information about a single collision.
//This record will typically be passed by non-const reference into functions which calculate hit data and amended inside that function so that the hit data can be used elsewhere.
template <typename T>
struct HitRecord {
	Vector3D<T> m_point;							//The point of collision
	Vector3D<T>	m_normal;							//The normal to the point of collision. The convention used here is that it will always be pointing in the opposite direction to the incoming ray.
	std::shared_ptr<Material<T>> m_materialPtr;		//A pointer to the material of the object in the collision.
	T			m_interval;							//The interval along the ray at which the collision occurred.
	bool		m_frontFace;						//Whether the ray collided from the outside of the object, or the inside. true -> ray came from outside.

	void setNormalDirection(const Ray<T>& inRay, const Vector3D<T>& outwardNormal) {
		//If the ray is already pointing against the normal, then their dot product will be < 0.
		m_frontFace = inRay.direction().innerProduct(outwardNormal) < 0;
		//Once that's calculated, we adjust our normal to guarantee it is pointing against the ray.
		m_normal = (m_frontFace) ? outwardNormal : -outwardNormal;																						
	}
};


template <typename U>
class Hittable
{
public:
	//The function to determine whether an incoming ray hit an object. Note that this function simply returns true if it hit and object in a valid collision.
	//The hit record passed in is also altered to store information about that particular collision.
	virtual bool isHit(const Ray<U>& inRay, U t_min, U t_max, HitRecord<U>& inRec) const = 0;

	//A function to determine the minimum distance the surface of another object must be away from the center of this object to prevent clipping issues.
	//i.e the point on the surface of this object which is furthest away from its center.
	//This is usually easy enough to calculate, but the calculation varies per object.
	virtual U minDistanceApart() const = 0;

	//All objects must have a center, and it is very helpful to be able to access where it is.
	virtual Vector3D<U> getCenter() const = 0;
};

#endif

