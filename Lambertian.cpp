#include "Lambertian.h"



bool Lambertian::isScattered(const Ray& inRay, const HitRecord& inRecord, Vec3D& inColourAtten, Ray& scatteredRay) const {
	//Calculate a scattered direction by generating a unit vector inside the unit sphere at the point of collision and with radius equal to the unit normal to the surface.
	auto scatterDirection{ inRecord.m_normal + Vec3D::randLambertianUnitSphere() };

	//Catch fringe cases where the scatter direction is near zero, e.g when the random Lambertian scatter vector is approx equal to minus the unit normal
	if (scatterDirection.length() <= std::numeric_limits<double>::epsilon()) scatterDirection = inRecord.m_normal;

	//Then set up the scattered ray.
	scatteredRay = Ray(inRecord.m_point, scatterDirection);
	inColourAtten = m_albedoColour;
	return true;
}