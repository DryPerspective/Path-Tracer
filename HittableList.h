#ifndef HITTABLELIST_H
#define HITTABLELIST_H
#pragma once
/* This class contains a list of hittable objects, stored in a std::vector. Easy to use when we have multiple objects in our program (which we do)
*  Note that even though this is kept as a derived class for type Hittable, it isn't a Hittable in the sense that we would ordinarily think it is.
*  Which is to say that the only calculation on whether a ray hits an object done in this class is an iteration over the list, which then delegates 
*  calculations about hitting a particular object to that object's own functions.
*/


#include <memory> //For shared pointers
#include <vector> //For vectors (the standard library kind).

#include "Hittable.h"


class HittableList : public Hittable
{
	
private:
	std::vector<std::shared_ptr<Hittable>> m_objectList;			//Our list of objects.

public:
	//Constructor
	HittableList() {}
	HittableList(std::shared_ptr<Hittable> inObject) { add(inObject); }

	//Basic list manipulation
	void add(std::shared_ptr<Hittable> inObject);
	void clear(); 
	const Hittable& operator[](std::size_t inIndex);
	std::size_t length();


	//And let's not forget our isHit function. Because this is a list of objects, we need to only return the closest object to the camera that we've hit.
	bool isHit(const Ray& inRay, double t_min, double t_max, HitRecord& inRec) const override;

	//This function doesn't really make sense for a list of objects, but we must include an override.
	virtual double minDistanceApart() const override;

	//Return the center of all points in the system, on "average"
	virtual Physics::PhysicsVector<3> getCenter() const override;

};
#endif
