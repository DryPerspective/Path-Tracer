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



template <typename T>
class Camera
{
private:

	//Member data. Some default velues are provided.
	Vector3D<T> m_cameraPosition{ 0,0,0 };
	Vector3D<T> m_cameraLookingAt{ 0,0,-1 };	//A point that the camera is looking at, to be the center of the frame.
	Vector3D<T> m_cameraUpOrientation{ 0,1,0 };	//"Upwards" orientation for the camera (may be easier to think of as upwards in the system it is recording).
	Vector3D<T> m_lowerLeftCorner;				//The position of the lower left corner of the viewport.
	Vector3D<T> m_horizontalDirection;			//The "horizontal direction" of the image screen, namely the vector from the center directly horizontal to the right-hand edge.
	Vector3D<T> m_verticalDirection;			//The "vertical direction" of the image screen, from the center directly vertical to the upwards edge.
	//These three vectors are unit vectors used to handle the orientation of the camera. They are unit vectors in the camera's basis.
	Vector3D<T> m_alongViewLine;				//Corresponds to +Z. The camera looks in the -Z direction.
	Vector3D<T> m_positiveHorizontal;			//Corresponds to +X
	Vector3D<T> m_positiveVertical;				//Corresponds to +Y
	//
	T			m_aspectRatio{ 16.0 / 9.0 };
	T			m_focalLength{ 1 };
	T			m_fieldOfView{ 60 };			//Vertical FoV in degrees	
	T			m_viewportHeight;
	T			m_viewportWidth;
	T			m_apertureSize{ 0.1 };
	T			m_focusDistance{ 10 };
	T			m_lensRadius;


	static constexpr T pi = 3.14159265358979;

	//Basic conversion of degrees to radians
	T degreesToRads(T inDegrees) { return inDegrees * pi / 180.0; }

	//A common function for setup to prevent code repetition in constructors.
	void setupCamera() {
		//Simple trigonometry - the height of our viewport is detemined by our angle of vertical FoV and focal length. That forms a right angled triangle and we can solve for h.
		auto theta{ degreesToRads(m_fieldOfView) };
		auto h{ m_focalLength * tan(theta / 2) };
		//From here we can calculate the height and width of our viewing port.
		m_viewportHeight = 2 * h;
		m_viewportWidth = m_aspectRatio * m_viewportHeight;

		//Next we have the issue of orientation. What we need are unit vectors in specific directions, such that we can use them to orient the viewport.
		//These will allow us to define the "horizontal" and "vertical" directions of the viewport for any particular camera angle.
		//First we get a vector of unit length in the exact line that the camera is looking at, but facing "backwards" - away from what the camera is looking at.
		m_alongViewLine = (m_cameraPosition - m_cameraLookingAt).getUnitVector();
		//Then we vector product this with the camera's upwards orientation to get a vector which is exactly perpendicular to both, and therefore in the direction
		//of the "horizontal" of the camera's view. Because we took the negative of the alongViewLine this points in the positive horizontal direction
		m_positiveHorizontal = Vector3D<T>::vectorProduct(m_cameraUpOrientation,m_alongViewLine).getUnitVector();
		//We can then vector product this horizontal vector with the alongViewLine vector to get a vector which is in the exact positive virtual direction of the viewport.
		//NB: This is not necessarily parallel to the m_cameraUpOrientation vector.
		m_positiveVertical = Vector3D<T>::vectorProduct(m_alongViewLine,m_positiveHorizontal).getUnitVector();

		//And with those in hand we can calculate the location of the lower left corner of the port.
		m_horizontalDirection = m_positiveHorizontal.scaledBy(m_viewportWidth).scaledBy(m_focusDistance);	//Double scaling is confusing. First we scale the unit vector to the size of the screen,
		m_verticalDirection = m_positiveVertical.scaledBy(m_viewportHeight).scaledBy(m_focusDistance);		//then we must scale it by the focus distance to simulate depth of field.
		m_lowerLeftCorner = m_cameraPosition - m_horizontalDirection.scaledBy(0.5) - m_verticalDirection.scaledBy(0.5) - m_alongViewLine.scaledBy(m_focalLength).scaledBy(m_focusDistance);

		m_lensRadius = m_apertureSize / 2;
	}


public:
	//Default constructor using default values
	Camera() {
		setupCamera();
	}

	//And this one allows us to customise our camera.
	Camera(Vector3D<T> inPosition, Vector3D<T> inLookingAt, Vector3D<T> inUp, T inRatio,  T inFocalLength, T inVFoV, T inAperture, T inFocusDist) 
		: m_cameraPosition(inPosition), m_cameraLookingAt(inLookingAt), m_cameraUpOrientation(inUp), m_aspectRatio(inRatio),  m_focalLength{ inFocalLength }, m_fieldOfView(inVFoV),
		  m_apertureSize(inAperture), m_focusDistance(inFocusDist)
	{
		setupCamera();
	}
		
	//A function to create a ray from the origin to a particular pixel on our image screen, with inputs as coordinates of that pixel.
	//We simulate depth of field using the thin lens approximation
	//Rather than originating all of our rays at the exact position of the camera, we originate them within a small disk, centered at that point.
	//The size of this disk is determined by our aperture size. A size of 0 will result in perfect focus, whereas a large size will result in very high DoF effect
	Ray<T> getCurrentRay(T inputX, T inputY) const {
		//Generate an offset on the unit disk and scale it by the thin lens radius (determined by aperture size)
		Vector3D<T> randDiskVector{ Vector3D<T>::randInUnitDisk().scaledBy(m_lensRadius) };	
		//Then orient it with the camera.
		Vector3D<T> apertureOffset{ m_positiveHorizontal.scaledBy(randDiskVector.getX()) + m_positiveVertical.scaledBy(randDiskVector.getY()) };

		//And return it in the ray.
		return Ray<T>(	m_cameraPosition + apertureOffset,
						m_lowerLeftCorner + m_horizontalDirection.scaledBy(inputX) + m_verticalDirection.scaledBy(inputY) - m_cameraPosition - apertureOffset);
	}

	//Getters for our encapsulated data. It's debatable that we really need to encapsulate.
	T getHeight() { return m_viewportHeight; }
	T getWidth() { return m_viewportWidth; }
	T getRatio() { return m_aspectRatio; }

	

};
#endif

