#ifndef METAL_H
#define METAL_H
#pragma once
/* This class simulates a reflective material. In short we use the smoothReflect method in Vector3D.h to reflect incoming rays such that the angle of reflection equals the angle of incidence.
*  We also simulate surface imperfections (and "fuzzy" reflections) by adding a small random perturberance to the reflection, which can be scaled to make perfectly smooth metals or very "fuzzy" ones.
*/



#include "Material.h"

template <typename T>
class Metal : public Material<T>
{
private:
	Vector3D<T> m_albedoColour;
	T			m_fuzz;					//A factor between 0 and 1 to determine the "fuzziness" of the metal. Namely how smoothly the reflections take place.
										//A value of 0 corresponds to a perfectly reflective metal.
public:
	Metal(const Vector3D<T>& inColour, T inFuzz) :m_albedoColour(inColour), m_fuzz((inFuzz<1)?inFuzz:1) {}			//Fuzziness is measured between 0 and 1.

	virtual bool isScattered(const Ray<T>& inRay, const HitRecord<T>& inRecord, Vector3D<T>& inColourAtten, Ray<T>& scatteredRay) const override {
		//First calculate the direction of the reflected ray using the smooth reflect method inside the Vector3D class.
		Vector3D<T> reflectedDirection{ Vector3D<T>::smoothReflect(inRay.direction().getUnitVector(),inRecord.m_normal) };
		//Then assign it to the ray, and add a small random perturbation according to the fuzziness of the material.
		scatteredRay = Ray<T>(inRecord.m_point, reflectedDirection + Vector3D<T>::randInUnitSphere().scaledBy(m_fuzz));
		inColourAtten = m_albedoColour;
		//It only makes sense for our reflected ray to travel outwards from the material and not inwards through it, or exactly parallel.
		//So we return false for the cases where that happens and discard the reflection.
		return (reflectedDirection.innerProduct(inRecord.m_normal) > 0);
	}
};
#endif

