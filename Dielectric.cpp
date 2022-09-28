#include "Dielectric.h"



bool Dielectric::isScattered(const Ray& inRay, const HitRecord& inRecord, Vec3D& inColourAtten, Ray& scatteredRay) const {
	//Attenuation goes to white because the light is never absorbed.
	inColourAtten = Vec3D{ 1,1,1 };
	//Assuming that we are moving between air with refractive index of exactly 1 and this material
	double refractionRatio{ (inRecord.m_frontFace) ? (1 / m_refractiveIndex) : m_refractiveIndex };

	Vec3D outwardsDirection;

	//Calculate total internal reflection. Our refraction is based on Snell's law, however there exists a set of possible values where Snell's law has no solution.
	//Since sin(theta) can never be bigger than 1, what happens if we set up our system such that it does?
	//Solving for sin(theta') we get n/n' sin(theta) = sin(theta') - NB: This is for the case of a ray inside the sphere coming across the outer surface.
	//If n/n' sin(theta) > 1, then we know there is no valid solution for theta', and the light must reflect, not refract.
	//We use the standard trig identity of sin^2 x + cos^2 x = 1, to derive our value of sin(theta).
	//How we calculate cos(theta) is covered in the comments on the refract method in Vector3D.h
	double cosTheta{ fmin(inRecord.m_normal.innerProduct(-inRay.direction()),1.0) };
	double sinTheta{ sqrt(1 - cosTheta * cosTheta) };
	bool refractionForbidden{ (refractionRatio * sinTheta) > 1 };

	//We also need to consider the contribution of the Fresnel Factor, i.e. that the "true" refractive index of a substance is dependent on the angle of incidence to it.
	//Fortunately we're going to use the Schlick approximation to make things easy, specifics down below.
	//Though in reality a ray will be both transmitted and reflected, for the purpose of this simulation we are only simulating one.
	//We pick randomly, but the function here does allow some slight weighting based on reality (more reflective is chosen to reflect more often).
	bool reflectBecauseFresnel{ calcReflection(cosTheta,m_refractiveIndex) > randNumber() };


	if (refractionForbidden || reflectBecauseFresnel) {
		//Calculate the outwards ray direction by perfect reflection
		outwardsDirection = Vec3D::smoothReflect(inRay.direction().getUnitVector(), inRecord.m_normal);
	}
	else {
		//Calculate our outwards ray direction by refraction (details on how in the Vector3D class)
		outwardsDirection = Vec3D::refract(inRay.direction(), inRecord.m_normal, refractionRatio);
	}


	//And update our scattered ray.
	scatteredRay = Ray(inRecord.m_point, outwardsDirection);
	return true;

}

double Dielectric::calcReflection(double inCosTheta, double inRefractiveIndex) {
	double R_0 = (1 - inRefractiveIndex) / (1 + inRefractiveIndex);
	R_0 *= R_0;
	return R_0 + (1 - R_0) * pow(1 - inCosTheta, 5);
}

double Dielectric::randNumber() {
	std::uniform_real_distribution<double> distribution{ 0.0,1.0 };
	static std::mt19937 mersenne;
	return distribution(mersenne);
}