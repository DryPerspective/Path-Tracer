#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H
#pragma once
/* This class simulates a perfect Lambertian (diffuse) material. Scattering is very simple. We consider a small unit sphere centered exactly one surface normal away from the point of collision,
*  and with radius of that surface normal. We then pick a random vector inside that unit sphere for the direction of scattering, and normalise it to length 1 to more accurately simulate
*  the distribution of scattering directions for real Lambertian materials.
*/


#include "Material.h"


class Lambertian : public Material
{
	

private:
	Vec3D m_albedoColour;
public:
	Lambertian(const Vec3D& inColour) : m_albedoColour(inColour) {}

	virtual bool isScattered(const Ray& inRay, const HitRecord& inRecord, Vec3D& inColourAtten, Ray& scatteredRay) const override;
};
#endif

