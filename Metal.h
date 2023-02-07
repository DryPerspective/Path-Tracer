#ifndef METAL_H
#define METAL_H
#pragma once
/* This class simulates a reflective material. In short we use the smoothReflect method in Vector3D.h to reflect incoming rays such that the angle of reflection equals the angle of incidence.
*  We also simulate surface imperfections (and "fuzzy" reflections) by adding a small random perturberance to the reflection, which can be scaled to make perfectly smooth metals or very "fuzzy" ones.
*/



#include "Material.h"


class Metal : public Material
{
	

private:
	Physics::PhysicsVector<3>		m_albedoColour;
	double							m_fuzz;					//A factor between 0 and 1 to determine the "fuzziness" of the metal. Namely how smoothly the reflections take place.
															//A value of 0 corresponds to a perfectly reflective metal.
public:
	Metal(const Physics::PhysicsVector<3>& inColour, double inFuzz) :m_albedoColour(inColour), m_fuzz((inFuzz < 1) ? inFuzz : 1) {}			//Fuzziness is measured between 0 and 1.

	virtual bool isScattered(const Ray& inRay, const HitRecord& inRecord, Physics::PhysicsVector<3>& inColourAtten, Ray& scatteredRay) const override;
};
#endif

