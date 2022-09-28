#include "Metal.h"



bool Metal::isScattered(const Ray& inRay, const HitRecord& inRecord, Vec3D& inColourAtten, Ray& scatteredRay) const {
	//First calculate the direction of the reflected ray using the smooth reflect method inside the Vector3D class.
	auto reflectedDirection{ Vec3D::smoothReflect(inRay.direction().getUnitVector(),inRecord.m_normal) };
	//Then assign it to the ray, and add a small random perturbation according to the fuzziness of the material.
	scatteredRay = Ray(inRecord.m_point, reflectedDirection + Vec3D::randInUnitSphere().scaledBy(m_fuzz));
	inColourAtten = m_albedoColour;
	//It only makes sense for our reflected ray to travel outwards from the material and not inwards through it, or exactly parallel.
	//So we return false for the cases where that happens and discard the reflection.
	return (reflectedDirection.innerProduct(inRecord.m_normal) > 0);
}