#ifndef DIELECTRIC_H
#define DIELECTRIC_H
#pragma once
/* This class handles a dielectric (transparent) material.
*  When a ray collides with an object of this material, the ray is either refracted or reflected. In reality it will be both but for the purposes of the simulation we are only choosing one.
*  Refraction is handled by Snell's Law, with some adjustment for cases of total internal reflection as calculated using the Schlick approximation.
*  The maths used to define each method is commented above the respective function in Vector3D.h
*/


#include <random> //For rng when calculating whether to transmit or reflect

#include "Material.h"

template <typename T>
class Dielectric : public Material<T>
{
private:
	T m_refractiveIndex;		//The refractive index of the material.
public:
	Dielectric(T inIndex) :m_refractiveIndex(inIndex) {}

	virtual bool isScattered(const Ray<T>& inRay, const HitRecord<T>& inRecord, Vector3D<T>& inColourAtten, Ray<T>& scatteredRay) const override {
		//Attenuation goes to white because the light is never absorbed.
		inColourAtten = Vector3D<T>(1.0, 1.0, 1.0);
		//Assuming that we are moving between air with refractive index of exactly 1 and this material
		T refractionRatio{ (inRecord.m_frontFace) ? (1 / m_refractiveIndex) : m_refractiveIndex };

		Vector3D<T> outwardsDirection;
		
		//Calculate total internal reflection. Our refraction is based on Snell's law, however there exists a set of possible values where Snell's law has no solution.
		//Since sin(theta) can never be bigger than 1, what happens if we set up our system such that it does?
		//Solving for sin(theta') we get n/n' sin(theta) = sin(theta') - NB: This is for the case of a ray inside the sphere coming across the outer surface.
		//If n/n' sin(theta) > 1, then we know there is no valid solution for theta', and the light must reflect, not refract.
		//We use the standard trig identity of sin^2 x + cos^2 x = 1, to derive our value of sin(theta).
		//How we calculate cos(theta) is covered in the comments on the refract method in Vector3D.h
		T cosTheta{ fmin(inRecord.m_normal.innerProduct(-inRay.direction()),static_cast<T>(1.0)) };
		T sinTheta{ sqrt(1 - cosTheta * cosTheta) };
		bool refractionForbidden{ (refractionRatio * sinTheta) > 1 };

		//We also need to consider the contribution of the Fresnel Factor, i.e. that the "true" refractive index of a substance is dependent on the angle of incidence to it.
		//Fortunately we're going to use the Schlick approximation to make things easy, specifics down below.
		//Though in reality a ray will be both transmitted and reflected, for the purpose of this simulation we are only simulating one.
		//We pick randomly, but the function here does allow some slight weighting based on reality (more reflective is chosen to reflect more often).
		bool reflectBecauseFresnel{ calcReflection(cosTheta,m_refractiveIndex) > randNumber() };
	

		if (refractionForbidden || reflectBecauseFresnel) {
			//Calculate the outwards ray direction by perfect reflection
			outwardsDirection = Vector3D<T>::smoothReflect(inRay.direction().getUnitVector(),inRecord.m_normal);
		}
		else {
			//Calculate our outwards ray direction by refraction (details on how in the Vector3D class)
			outwardsDirection = Vector3D<T>::refract(inRay.direction(),inRecord.m_normal,refractionRatio);
		}

		
		//And update our scattered ray.
		scatteredRay = Ray<T>(inRecord.m_point, outwardsDirection);
		return true;
		
	}

	//Calculate the reflection coefficient according to the Schlick approximation.
	//Namely, R(theta) = R_0 + (1-R_0)(1-cos(theta))^5, where R_0 = ((n-n')/n+n'))^2
	//As with elsewhere, this method assumes that you're transmitting from or to air with a refractive index of exactly 1.
	static T calcReflection(T inCosTheta, T inRefractiveIndex){
		T R_0 = (1 - inRefractiveIndex) / (1 + inRefractiveIndex);
		R_0 *= R_0;
		return R_0 + (1 - R_0) * pow(1 - inCosTheta, 5);
	}

	//Generate a random number between 0 and 1. Used to determine whether to transmit or reflect.
	static T randNumber() {
		std::uniform_real_distribution<T> distribution{ 0.0,1.0 };
		static std::mt19937 mersenne;
		return distribution(mersenne);
	}


};
#endif

