#include "Camera.h"

#include "VectorFunc.h"



void Camera::setupCamera() {
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
	m_positiveHorizontal = Vec3D::vectorProduct(m_cameraUpOrientation, m_alongViewLine).getUnitVector();
	//We can then vector product this horizontal vector with the alongViewLine vector to get a vector which is in the exact positive virtual direction of the viewport.
	//NB: This is not necessarily parallel to the m_cameraUpOrientation vector.
	m_positiveVertical = Vec3D::vectorProduct(m_alongViewLine, m_positiveHorizontal).getUnitVector();

	//And with those in hand we can calculate the location of the lower left corner of the port.
	m_horizontalDirection = m_positiveHorizontal.scaledBy(m_viewportWidth).scaledBy(m_focusDistance);	//Double scaling is confusing. First we scale the unit vector to the size of the screen,
	m_verticalDirection = m_positiveVertical.scaledBy(m_viewportHeight).scaledBy(m_focusDistance);		//then we must scale it by the focus distance to simulate depth of field.
	m_lowerLeftCorner = m_cameraPosition - m_horizontalDirection.scaledBy(0.5) - m_verticalDirection.scaledBy(0.5) - m_alongViewLine.scaledBy(m_focalLength).scaledBy(m_focusDistance);

	m_lensRadius = m_apertureSize / 2;
}

Camera::Camera() {
	setupCamera();
}

Ray Camera::getCurrentRay(double inputX, double inputY) const {
	//Generate an offset on the unit disk and scale it by the thin lens radius (determined by aperture size)
	Vec3D randDiskVector{ dp::randInUnitDisk().scaledBy(m_lensRadius) };
	//Then orient it with the camera.
	Vec3D apertureOffset{ m_positiveHorizontal.scaledBy(randDiskVector.getX()) + m_positiveVertical.scaledBy(randDiskVector.getY()) };

	//And return it in the ray.
	return Ray(m_cameraPosition + apertureOffset, m_lowerLeftCorner + m_horizontalDirection.scaledBy(inputX) + m_verticalDirection.scaledBy(inputY) - m_cameraPosition - apertureOffset);
}

double Camera::getHeight() { return m_viewportHeight; }
double Camera::getWidth() { return m_viewportWidth; }
double Camera::getRatio() { return m_aspectRatio; }