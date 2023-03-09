
#include "HittableList.h"


void HittableList::add(std::shared_ptr<Hittable> inObject) {
	m_objectList.push_back(inObject);
}

void HittableList::clear() { 
	m_objectList.clear(); 
}

const Hittable& HittableList::operator[](std::size_t inIndex) {
	return *(m_objectList[inIndex]);
}

std::size_t HittableList::length() {
	return m_objectList.size(); 
}

bool HittableList::isHit(const Ray& inRay, double t_min, double t_max, HitRecord& inRec) const {
	HitRecord tempRecord;
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

double HittableList::minDistanceApart() const {
	return 0;															//Trivial solution as this function doesn't really apply to this object.								
}

dp::PhysicsVector<3> HittableList::getCenter() const {
	dp::PhysicsVector<3> center{ 0,0,0 };
	for (const auto& hittable : m_objectList) {
		center += hittable->getCenter();
	}
	double scale{ (1.0) / static_cast<double>(m_objectList.size()) };
	return center.scaledBy(scale);
}