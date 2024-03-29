#ifndef MATERIAL_H
#define MATERIAL_H
#pragma once
/* An abstract class for a general material. The only key thing is calculating how a particular ray will scatter when colliding with an object of that material.
*/

#include "Hittable.h"
#include "Ray.h"

#include "PhysicsVector.h"



class Material
{
public:
	
	//Returns a bool so that the main can easily determine whether a scatter happens at all (and allows us to terminate cases where things will go wrong)
	//However the HitRecord and scattered ray (and usually the inColour) will be altered within the function as well, and can be passed back to other functions
	//with new information about each collision.
	virtual bool isScattered(const Ray& inRay, const HitRecord& inRecord, dp::PhysicsVector<3>& inColourAtten, Ray& scatteredRay) const = 0;

};
#endif
