#ifndef DIELECTRIC_H
#define DIELECTRIC_H
#pragma once
/* This class handles a dielectric (transparent) material.
*  When a ray collides with an object of this material, the ray is either refracted or reflected. In reality it will be both but for the purposes of the simulation we are only choosing one.
*  Refraction is handled by Snell's Law, with some adjustment for cases of total internal reflection as calculated using the Schlick approximation.
*  The maths used to define each method is commented above the respective function in Vector3D.h
*/


#include <random> //For rng when calculating whether to transmit or reflect

#include "PhysicsVector.h"
#include "Material.h"


class Dielectric : public Material
{

private:
	double m_refractiveIndex;		//The refractive index of the material.
public:
	Dielectric(double inIndex) :m_refractiveIndex{ inIndex } {}

	virtual bool isScattered(const Ray& inRay, const HitRecord& inRecord, dp::PhysicsVector<3>& inColourAtten, Ray& scatteredRay) const override;

	//Calculate the reflection coefficient according to the Schlick approximation.
	//Namely, R(theta) = R_0 + (1-R_0)(1-cos(theta))^5, where R_0 = ((n-n')/n+n'))^2
	//As with elsewhere, this method assumes that you're transmitting from or to air with a refractive index of exactly 1.
	static double calcReflection(double inCosTheta, double inRefractiveIndex);

	//Generate a random number between 0 and 1. Used to determine whether to transmit or reflect.
	static double randNumber();


};
#endif

