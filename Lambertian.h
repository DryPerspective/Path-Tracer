#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H
#pragma once
/* This class simulates a perfect Lambertian (diffuse) material. Scattering is very simple. We consider a small unit sphere centered exactly one surface normal away from the point of collision,
*  and with radius of that surface normal. We then pick a random vector inside that unit sphere for the direction of scattering, and normalise it to length 1 to more accurately simulate
*  the distribution of scattering directions for real Lambertian materials.
*/


#include "Material.h"

template <typename T>
class Lambertian : public Material<T>
{
private:
	Vector3D<T> m_albedoColour;
public:
	Lambertian(const Vector3D<T>& inColour) : m_albedoColour(inColour) {}


	virtual bool isScattered(const Ray<T>& inRay, const HitRecord<T>& inRecord, Vector3D<T>& inColourAtten, Ray<T>& scatteredRay) const override {
		//Calculate a scattered direction by generating a unit vector inside the unit sphere at the point of collision and with radius equal to the unit normal to the surface.
		Vector3D<T> scatterDirection{ inRecord.m_normal + Vector3D<T>::randLambertianUnitSphere() };

		//Catch fringe cases where the scatter direction is near zero, e.g when the random Lambertian scatter vector is approx equal to minus the unit normal
		if (scatterDirection.isNearZero()) scatterDirection = inRecord.m_normal;

		//Then set up the scattered ray.
		scatteredRay = Ray<T>(inRecord.m_point, scatterDirection);
		inColourAtten = m_albedoColour;
		return true;
	}
};
#endif

