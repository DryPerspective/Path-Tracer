#ifndef VECTOR_SPECFIC_FUNCTIONS
#define VECTOR_SPECFIC_FUNCTIONS
#pragma once




#include "PhysicsVector.h"


/*
* PhysicsVector from the utils library is a wonderful thing, but it doesn't contain scattering functions which this project specifically needs. Nor should it, as that would pollute other projects.
* As such, vector-specific functionality required in this project is listed here as a separate free-function module of sorts. (Not a C++20 module, obviously)
*/



	
//Perhaps redundant, but not polluting the global namespace with these functions is preferable.
namespace dp {

	//First, a "scale-by-vector" function. In pure math terms this is pretty meaningless but it transforms {a,b,c} and {x,y,z} to {ax,by,cz}
	dp::PhysicsVector<3> scaledByVector(const dp::PhysicsVector<3>& toScale, const dp::PhysicsVector<3>& scaleBy);

	//Scattering functions
	//Generate a vector with randomly assigned values between two points.
	dp::PhysicsVector<3> randVector(double inMin, double inMax);

	//Generate a random vector inside the unit sphere.
	dp::PhysicsVector<3> randInUnitSphere();

	//A closer approximation to true Lambertian scattering for diffuse materials.
	//Taking the unit vector biases samples more towards the surface normal (must be on the surface of the unit sphere rather than just inside it)
	//Which, as it turns out, is closer to what Lambertian scattering does in reality.
	dp::PhysicsVector<3> randLambertianUnitSphere();

	//Generate a random vector inside the unit disk. Primarily used to simulate a camera aperture
	dp::PhysicsVector<3> randInUnitDisk();

	//A function to simulate perfect reflection on a smooth surface. We assume angle of incidence equals angle of reflection, and do a little vector mathmatics.
	//First draw your incoming vector V as though it continued straight through the material. Let B be the displacement from the material inner surface to the tip of your vector.
	//It's simple mathematics that the tip of your reflected vector will exist at the tip of your vector inside the material + 2B.
	//The length of B will be given by V*N (dot product), where N is the surface normal. 
	//Therefore the tip of the reflected vector will exist (V*N) distance above the surface, which is in the direction of N.
	//Therefore our reflected vector can be calculated by V - (V*N)N. The minus sign comes from the fact that V and N are in opposite directions.
	dp::PhysicsVector<3> smoothReflect(const dp::PhysicsVector<3>& inRay, const  dp::PhysicsVector<3>& inNormal);

	//How we simulate refraction. Snell's Law: n1 sin(theta1) = n2 sin(theta2), where n (eta) corresponds to the refractive index and the theta is the angle between the ray and the normal.
	//So for air into glass, we get n_air sin(angle of incidence) = n_glass sin(angle of transmission).
	//Since we know both refractive indices and can calculate the angle of incidence for a given ray, we want to solve for sin(theta2) to calculate the trajectory of the transmitted ray.
	//For teminology's sake, unprimed values(n, theta, R) are incident, and primed (n', theta', R') are transmitted. R is the incident/transmitted vector, and N is the vector normal.
	// We want to solve for R', the vector representing the direction of transmission. This is best done by separating R' into components perpendicular and parallel to the normal.
	// After some vector calculus, we can find that R'_perp = n/n' * (R - cos(theta)N)) and that R'_parallel = -sqrt(1-|R'_perp|^2 N).
	//Substituting the identity A*B = |A||B|cost(theta) and confining our A and B to unit vectors(giving A*B = cos(theta)), we can express R'_perp in entirely known quantities
	// R'_perp = n/n' (R + (-R*N)N).
	dp::PhysicsVector<3> refract(const dp::PhysicsVector<3>& inR, const dp::PhysicsVector<3>& inNormal, const double etaOverEtaPrime);

	//This function is intended to read and construct a PhysicsVector object from a std::string
	//This version is very generalised and as such is a little heavy in performance.
	//Specific needs in specific projects can be met by providing a specialisation of this template which is specific to that project.
	//This function returns true if the vector was read in correctly, and false otherwise. In the event of an invalid call, it sets the input vector to {0,0,...0};
	template<std::size_t dim>
	bool readVector(std::string_view inputString, dp::PhysicsVector<dim>& inVector) {
		//First, validate that the string is of the correct format (X,Y,Z)
		std::regex vectorReg{ R"([\{\[\(<]?(\d*\.?\d*\,){0,}\d+\.?\d*[\}\]\)>]?)" };
		//Optionally one of [{(<, then any amount of (0-9, optionally with a . and another [0-9] then ,), then another potentially decimal number and optionally a closing bracket
		if (!std::regex_match(std::string(inputString), vectorReg)) {
			inVector = PhysicsVector<dim>{};
			return false;
		}
		//We delimit around the comma to reach our individual numbers.
		//As we cannot easily insert our dim variable into the regex, the simplesy way to check we have the right number of dimensions is counting the number of commas
		auto numberOfCommas{ std::count(inputString.begin(), inputString.end(),',') };
		if (numberOfCommas != dim - 1) {
			inVector = PhysicsVector<dim>{};
			return false;
		}

		//If the vector is surrounded by brackets, we need to trim them off. We account for all common bracket styles.
		std::string brackets{ "{}[]()<>" };
		if (std::any_of(brackets.begin(), brackets.end(), [inputString](const char& x) {return x == inputString[0]; })) inputString.remove_prefix(1);
		if (std::any_of(brackets.begin(), brackets.end(), [inputString](const char& x) {return x == inputString[inputString.length() - 1]; })) inputString.remove_suffix(1);

		//If we get this far, we have a good degree of confidence that our vector is of the correct format and that external brackets have been trimmed.
		//All that remains is to separate out the numbers, read them, and write them to a PhysicsVector object.
		//This is trivial for 1D vectors, and in this case our entire inputString string should just be the number we want.

		auto parseTerm = [](std::string_view input) -> double {
			double output;
			std::from_chars(input.data(), input.data() + input.length(), output);
			return output;
			};

		if constexpr (dim == 1) {
			inVector.setAt(0, parseTerm(inputString));
		}
		//Otherwise we just read every number up to each comma, and set the vector accordingly
		else {
			for (std::size_t i = 0; i < dim; ++i) {
				auto firstComma{ inputString.find_first_of(',') };
				std::string_view firstTerm{ inputString.substr(0,firstComma) };
				inputString.remove_prefix(firstComma + 1);
				inVector.setAt(i, parseTerm(firstTerm));
			}
		}
		return true;
	}

	template<std::size_t dim>
	PhysicsVector<dim> readVector(std::string_view inString) {
		PhysicsVector<dim> out{};
		readVector(inString, out);
		return out;
	}



}



#endif