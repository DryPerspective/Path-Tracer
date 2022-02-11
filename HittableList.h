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

template <typename T>
class HittableList : public Hittable<T>
{
private:
	std::vector<std::shared_ptr<Hittable<T>>> m_objectList;			//Our list of objects.

public:
	//Constructor
	HittableList() {}
	HittableList(std::shared_ptr<Hittable<T>> inObject) { add(inObject); }

	//Basic list manipulation
	void add(std::shared_ptr<Hittable<T>> inObject) { m_objectList.push_back(inObject); }
	void clear() { m_objectList.clear(); }
	const Hittable<T>& operator[](T inIndex) { return *(m_objectList[inIndex]); }
	int length() { return m_objectList.size(); }

	


	//And let's not forget our isHit function. Because this is a list of objects, we need to only return the closest object to the camera that we've hit.
	bool isHit(const Ray<T>& inRay, T t_min, T t_max, HitRecord<T>& inRec) const override {
		HitRecord<T> tempRecord;
		bool isHitAnything{ false };
		auto closestSoFar{ t_max };

		//Iterate over our list.
		for (const auto& hittable : m_objectList) {
			if (hittable->isHit(inRay, t_min, closestSoFar, tempRecord)) {	//If something is hit (and remember this function will updated tempRecord with details if so)
				isHitAnything = true;										//We hit something!
				closestSoFar = tempRecord.m_interval;
				inRec = tempRecord;											//Only update our hit record if we actually hit something.
			}
		}

		return isHitAnything;
	}

	//This function doesn't really make sense for a list of objects, but we must include an override.
	virtual T minDistanceApart() const override {
		return 0;
	}

	//Return the center of all points in the system, on "average"
	virtual Vector3D<T> getCenter() const override {
		Vector3D<T> center(0, 0, 0);
		for (const auto& hittable : m_objectList) {
			center += hittable->getCenter();
		}
		T scale{ static_cast<T>(1.0) / static_cast<T>(m_objectList.size()) };
		return center.scaledBy(scale);
	}
};
#endif
