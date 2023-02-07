#include "Sphere.h"



bool Sphere::isHit(const Ray& inRay, double t_min, double t_max, HitRecord& inRec) const {
	Physics::PhysicsVector<3> originToCenter = inRay.origin() - m_center;
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
	Physics::PhysicsVector<3> outwardNormal = (inRec.m_point - m_center).scaledBy(1 / m_radius);		//Generate an outwards-facing normal unit vector.
	inRec.setNormalDirection(inRay, outwardNormal);														//And set the normal stored in the record to follow the convention of opposing the direction of the ray.
	inRec.m_materialPtr = m_material;

	return true;
}

double Sphere::minDistanceApart() const {
	return m_radius;
}

Physics::PhysicsVector<3> Sphere::getCenter() const {
	return m_center;
}