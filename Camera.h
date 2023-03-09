#ifndef CAMERA_H
#define CAMERA_H
#pragma once
/* This class handles a camera object.
* It is kept as a template class so as to not confine the other templates to one type.
* The camera exists at any position within our system's coordinates, and looks in a particular direction.
* The generated image is what the camera would see through a rectangular viewport, exactly one m_focalLength away from the camera, centered at the point the camera is focusing on.
* Dimensions of the viewport, as well as optical effects like depth of field, are variable, and depend on the values the camera is constructed with.
*/


#include "Ray.h"
#include "Material.h"



class Camera final
{

private:
	using Vec3D = dp::PhysicsVector<3>;

	//Member data. Some default velues are provided.
	Vec3D m_cameraPosition{ 0,0,0 };
	Vec3D m_cameraLookingAt{ 0,0,-1 };		//A point that the camera is looking at, to be the center of the frame.
	Vec3D m_cameraUpOrientation{ 0,1,0 };	//"Upwards" orientation for the camera (may be easier to think of as upwards in the system it is recording).
	Vec3D m_lowerLeftCorner;				//The position of the lower left corner of the viewport.
	Vec3D m_horizontalDirection;			//The "horizontal direction" of the image screen, namely the vector from the center directly horizontal to the right-hand edge.
	Vec3D m_verticalDirection;				//The "vertical direction" of the image screen, from the center directly vertical to the upwards edge.
	//These three vectors are unit vectors used to handle the orientation of the camera. They are unit vectors in the camera's basis.
	Vec3D m_alongViewLine;					//Corresponds to +Z. The camera looks in the -Z direction.
	Vec3D m_positiveHorizontal;				//Corresponds to +X
	Vec3D m_positiveVertical;				//Corresponds to +Y
	//
	double			m_aspectRatio{ 16.0 / 9.0 };
	double			m_focalLength{ 1 };
	double			m_fieldOfView{ 60 };			//Vertical FoV in degrees	
	double			m_viewportHeight;
	double			m_viewportWidth;
	double			m_apertureSize{ 0.1 };
	double			m_focusDistance{ 10 };
	double			m_lensRadius;


	static constexpr double pi = 3.14159265358979;

	//Basic conversion of degrees to radians
	double degreesToRads(double inDegrees) { return inDegrees * pi / 180.0; }

	//A common function for setup to prevent code repetition in constructors.
	void setupCamera();


public:
	//Default constructor using default values
	Camera();

	//And this one allows us to customise our camera.
	Camera(Vec3D inPosition, Vec3D inLookingAt, Vec3D inUp, double inRatio,  double inFocalLength, double inVFoV, double inAperture, double inFocusDist) 
		: m_cameraPosition{ inPosition }, m_cameraLookingAt{ inLookingAt }, m_cameraUpOrientation{ inUp }, m_aspectRatio{ inRatio }, m_focalLength{ inFocalLength }, m_fieldOfView{ inVFoV },
		m_apertureSize{ inAperture }, m_focusDistance{ inFocusDist }
	{
		setupCamera();
	}
		
	//A function to create a ray from the origin to a particular pixel on our image screen, with inputs as coordinates of that pixel.
	//We simulate depth of field using the thin lens approximation
	//Rather than originating all of our rays at the exact position of the camera, we originate them within a small disk, centered at that point.
	//The size of this disk is determined by our aperture size. A size of 0 will result in perfect focus, whereas a large size will result in very high DoF effect
	Ray getCurrentRay(double inputX, double inputY) const;

	//Getters for our encapsulated data. It's debatable that we really need to encapsulate.
	double getHeight();
	double getWidth();
	double getRatio();

	

};
#endif

