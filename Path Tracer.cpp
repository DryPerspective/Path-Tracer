/*The main file for processing the simulation and rendering the result. The program flow is as follows:
* First, the camera and configuration constants (e.g. output image size, aspect ratio) are instantiated. Then the five fixed spheres are instantiated.
* Then, the program generates a collection of random spheres using a mersenne twister. It places these spheres exactly 1 radius above the ground and ensures that each sphere is at least
* one radius away from the surface of any other sphere (preventing clipping issues). 
* Once these spheres are generated, the program iterates over every pixel in the output image. It sends out a certain number (determined by a configuration constant in step 1) of rays randomly distributed inside that pixel,
* and if that ray hits an object (calculated inside the object class' functions), the simulation notes the collision data, then sends out a reflected ray after the collision. These reflected rays 
* have a capped maximum depth (again determined by a configuration constant set in step 1), after which we assume all the energy has been lost and we return a pure black.
* The colours of each ray per pixel are summed, and after every ray per pixel has been sent, they are divided by the number of rays per pixel to get the average colour for that pixel.
* The colour per pixel is then written into the output file.
*/



#include <iostream>
#include <fstream>             
#include <cstdio>           //This and the above includes used to create and write to an output file (and the console)
#include <random>           //For the mersenne twister used to generate random spheres
#include <ctime>            //To use the system time as the seed for the Mersenne.


#include "Vector3D.h"
#include "Ray.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Camera.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dielectric.h"

//A couple of type aliases to reduce confusion around exactly what we're talking about, and because most objects here are made from templates.
// I know templating everything isn't strictly necessary, but I felt it provided good portability (I've seen some people swear by using floats instead of doubles, for instance),
// as well as being a good test of my skills to make sure I used them correctly.
//Note that I also only instantiate templated objects using these aliases, meaning you need only change these lines to change the datatype used everywhere else.
//It also means there should be no type mismatches and errors which result from that. I do not recommend changing these aliases to different types from each other.
//
//
using numberType = double;                 //The basic numerical type used to pass around floating point numbers.
using point3D = Vector3D<numberType>;
using direction3D = Vector3D<numberType>;
using colour = Vector3D<numberType>;     //This may seem counterintuitive since RGB values take integer values, but we calculate over a continuous range between 0-1 and normalise to integers later.
using ray3D = Ray<numberType>;
using sphere3D = Sphere<numberType>;


//Constants. Not included ordinarily and used for much needed math.
const numberType infinity = std::numeric_limits<numberType>::infinity();


/*
*
*   Simple Utility functions.
*
*/
//Generates a random real number between the input arguments. This distribution is only used when randomly generating and placing spheres, and when generating antialiasing rays inside a single pixel.
numberType randNumberBetween(numberType inMin, numberType inMax) {
    static std::mt19937 mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
    std::uniform_real_distribution<numberType> distribution{ inMin,inMax };
    return distribution(mersenne);
}
//Return a value confined between two numbers
numberType inBetween(numberType inNum, numberType inMin, numberType inMax) {
    if (inNum < inMin)return inMin;
    if (inMax < inNum)return inMax;
    return inNum;
}





/*
*
*   Rendering functions
*
*/

//A function which takes a colour containing values normalised between 0 and 1, renormalises it to the RGB color range, and writes to the output stream.
//This is where division by number of samples per pixel takes place.
void writeColour(std::ostream& outStream, colour outColour, int samplesPerPixel) {
    auto r{ outColour.getX() };
    auto g{ outColour.getY() };
    auto b{ outColour.getZ() };
    auto scale = 1.0 / static_cast<numberType>(samplesPerPixel);

    //Scale them down. Sqrt is there to act as gamma correction factor.
    r = sqrt(scale*r);
    g = sqrt(scale*g);
    b = sqrt(scale*b);

    outStream << static_cast<int>(256 * inBetween(r,0,0.999)) << ' ' << static_cast<int>(256 * inBetween(g,0,0.999)) << ' ' << static_cast<int>(256 * inBetween(b,0,0.999)) << '\n';
}

//This is the main function to calculate the color of a particular pixel, as per the ray sent "through" it.
//In short, if the ray is projected to hit an object, we determine what the scatter off that object will look like (varies by material, determined in the material file).
//We also calculate the colour attenuation from having hit X objects previously. This is also handled in the material file.
//We keep on scattering rays until a ray never hits an object again, or until we reach the maximum number of deflections allowed.
//Each scatter is scaled by the colour attenuation so the first scatter has the most effect etc
//Then after going through as many objects as we can, we calculate the total colour seen by that ray and return it.
colour calcColour(const ray3D& inRay, const Hittable<numberType>& inObject, int inDepth) {
    HitRecord<numberType> tempRecord;

    //If we previously hit an object and have reached maximum depth
    if (inDepth <= 0)return colour(0, 0, 0);

    //If we hit an object and have not reached maximum depth:
    //NB: use of 0.001 as the minimum bound to solve "shadow acne" issues from floating point approximation issues around t=0.
    if (inObject.isHit(inRay, 0.001, infinity, tempRecord)) {
        ray3D scatteredRay;
        colour attenuationColour;
        //If we can scatter cleanly, we do.
        if (tempRecord.m_materialPtr->isScattered(inRay, tempRecord, attenuationColour, scatteredRay)) {
            return calcColour(scatteredRay, inObject, inDepth - 1).scaledByVector(attenuationColour);
        }
        //Otherwise we return pure black.
        return colour(0, 0, 0);
    }


    //Background work below here, only triggered if the ray doesn't touch anything. Currently a linear scale from blue to white.
    direction3D unitDirection = inRay.direction().getUnitVector();
    auto backgroundT = 0.5 * (unitDirection.getY() + 1);                                                            //Get a linear scalar along the y axis.
    return  colour(1.0, 1.0, 1.0).scaledBy(1.0 - backgroundT) + colour(0.5, 0.7, 1.0).scaledBy(backgroundT);      //And return a colour following a blue/white scale.
}

int main()
{    
    //Image settings, measured in pixels.
    const numberType outImageAspectRatio{ 16.0/9.0 };
    const int outImageWidth{ 400 };
    const int outImageHeight{ static_cast<int>(outImageWidth / outImageAspectRatio) };

    //Camera settings.
    //The camera can be called with specific settings, namely (and in order): 
    // Camera position, Point the center of the camera is looking at, Camera "upwards" orientation, Camera viewport aspect ratio, camera focal length, and camera vertical FoV.
    // There is also a default constructor Camera() which creates a camera with default values.
    point3D cameraPosition(8,  2,  3);                                                               //Position of the camera, defaults to (0,0,0)
    point3D cameraLookingAt(0, 0, 0);                                                               //Point the camera is looking at and focused on. Defaults to (0,0,-1)
    direction3D cameraUpOrientation(0, 1, 0);                                                       //"Upwards" orientation for the camera. Defaults to (0,1,0)
    numberType cameraFocalLength{ 1 };                                                              //Camera focal length, i.e. the distance between the camera and the viewport. Defaults to 1
    numberType cameraVerticalFoV{ 60 };                                                             //Camera vertical field of view angle, measured in degrees. Defaults to 60.
    numberType cameraApertureSize{ 0.1 };                                                          //Simulated aperture size for depth of field. Defaults to 0.1
    numberType cameraFocusDistance{ (cameraLookingAt-cameraPosition).length() };                    //Simulated focus distance for depth of field. Defaults to 10.
    Camera<numberType> simCamera(cameraPosition,cameraLookingAt,cameraUpOrientation,outImageAspectRatio,cameraFocalLength,cameraVerticalFoV,cameraApertureSize,cameraFocusDistance);
    

    //Antialiasing value of number of slightly randomised rays to send per pixel.
    const int raysPerPixel{ 100 };

    //Material maximum depth, i.e. number of times to generate a random reflected ray until returning pure black.
    const int materialMaximumDepth{ 50 };   

    //World settings
    //i.e where all our objects live.
    //First we create the materials needed for our five fixed spheres.
    auto materialGround{ std::make_shared<Lambertian<numberType>>(colour(0.5,0.5,0.5)) };            //A Pale diffuse material to act as the ground.
    auto materialRedDiffuse{ std::make_shared<Lambertian<numberType>>(colour(0.9,0.1,0.1)) };        //A reddish diffuse material.
    auto materialGreyMetal{ std::make_shared<Metal<numberType>>(colour(0.8,0.8,0.8), 0) };         //A smooth bright metal
    auto materialGreyFuzzy{ std::make_shared<Metal<numberType>>(colour(0.8,0.8,0.8), 0.8) };       //A very fuzzy metal
    auto materialDielectric{ std::make_shared<Dielectric<numberType>>(1.5) };                      //A simple dielectric.

    //We create our list of objects, and instantiate the four larger spheres we use as a clear demo for the materials.
    //We also create one very large sphere to act as the ground.
    HittableList<numberType> worldObjects;
    worldObjects.add(std::make_shared<sphere3D>(point3D(2,      1,  4),    1,     materialRedDiffuse));      //A small sphere to act as our test case. NB: original position of (0,1,0)
    worldObjects.add(std::make_shared<sphere3D>(point3D(0, -1000,  -1),     1000,       materialGround));    //A big sphere to act as the ground
    worldObjects.add(std::make_shared<sphere3D>(point3D(0,     1,  2),     1,     materialDielectric)); 
    worldObjects.add(std::make_shared<sphere3D>(point3D(0,      1,  -2),       1,     materialGreyFuzzy));
    worldObjects.add(std::make_shared<sphere3D>(point3D(2,      1,  -6), 1, materialGreyMetal));

    
    //Next we want to generate a large amount of random spheres to populate our scene. Random sphere choice makes for a better test of the system than a premade case.
    int numberOfSpheres{ 250 };      //Number of spheres to generate.
    for (int i = 0; i < numberOfSpheres; ++i) {        

        //We want materials randomised. Actual distribution of materials is covered further down.
        auto randomNumberForMaterial{ randNumberBetween(0,1) };
        //We want smallish spheres
        auto randomNumberForRadius{ randNumberBetween(0.1,0.4) };        

        //Next we want to generate the center of our random spheres, but we don't want our spheres to clip into each other.
        //This is done very eaily in spheres but if and when we use other shapes the framework is there and requires minimal adjusting.
        point3D sphereCenter;
        //Because our function is set to keep on trying vectors forever until we find one which will fit our sphere, there is the possibility of an infinite loop.
        //This can happen if we try to confine too many spheres in too small a space. Just to be safe, we'll use a loop counter to break out.
        int loopCounter{ 0 };
        while (true) {
            //Check our loop counter
            ++loopCounter;
            if (loopCounter > 50)break;
            //And if we're clear, assign a random position to our sphere
            sphereCenter = point3D(randNumberBetween(-10, 10), randomNumberForRadius, randNumberBetween(-10, 10));    //Generate a sphere.
            bool isClipped{ false };
            for (int i = 0; i < worldObjects.length(); ++i) {
                //If the new sphere will clip inside any object.
                if ((sphereCenter - worldObjects[i].getCenter()).length() < (randomNumberForRadius + worldObjects[i].minDistanceApart())) {
                    isClipped = true;
                    break;              //Break here because we need only clip with one object to fail.
                }
            }
            //Because of the nested loop we have to do it this way. If we have any clipping issues, we reject the random point and try again.
            if (isClipped)continue;
            //If we get this far then we know there are no clipping issues.
            break;
        }
        
        //If the loop counter broke us out of the above loop, then we know two things:
        //First, there probably isn't space left in the system to place any new spheres; and second, we have failed to properly initialise the new sphere's position.
        //In both cases we want to end our sphere generation early.
        if (loopCounter > 50)break;
        else loopCounter = 0;


        std::shared_ptr<Material<numberType>> sphereMaterial;

        //We want mostly diffuse
        if (randomNumberForMaterial < 0.6) {
            colour sphereColour{ colour::randVector(0,1) };
            sphereMaterial = std::make_shared<Lambertian<numberType>>(sphereColour);
        }
        //With about 30% metallic.
        else if (randomNumberForMaterial < 0.9) {
            colour sphereColour{ colour::randVector(0.6,1) };
            auto randomFuzziness{ randNumberBetween(0,1) / 2 };
            sphereMaterial = std::make_shared<Metal<numberType>>(sphereColour, randomFuzziness);
            
        }
        //And 10% dielectric
        else {
            sphereMaterial = std::make_shared<Dielectric<numberType>>(1.5);
        }

        //And after creating our position, radius, and material, we add the sphere to our scene.
        worldObjects.add(std::make_shared<sphere3D>(sphereCenter, randomNumberForRadius, sphereMaterial));
    }



    //Rendering settings
    //Create a new file for the output image and have a stream ready to insert to it.
    std::ofstream outImageStream("./outputImage.ppm", std::ios::out | std::ios::binary);

    //Setup line to initialise the image.
    outImageStream << "P3\n" << outImageWidth << ' ' << outImageHeight << "\n255\n";

    //Note that I use j as the outer variable here. While not the usual convention this is by design. It's easier to think as position[i][j] as analogous to position(x,y).
    //This preserves that while allowing for a proper scanline countdown.
    
    for (int j = outImageHeight-1; j >=0; --j) {
        std::cout << "Scanlines Remaining: " << j << '\n';
        for (int i = 0; i < outImageWidth; ++i) {
            //For each pixel, we sum the values of all the colours read by each ray, and then divide them through by the number of rays per pixel in the writeColour function
            colour pixelColour(0, 0, 0);
            //For each pixel, generate rays distributed randomly inside that pixel (antialiasing step)
            for (int s = 0; s < raysPerPixel; ++s) {               
                //Generate X/Y coordinates normalised inside a particular pixel                
                auto normalisedX = static_cast<numberType>(i + randNumberBetween(0, 1)) / (static_cast<numberType>(outImageWidth) - 1);
                auto normalisedY = static_cast<numberType>(j + randNumberBetween(0, 1)) / (static_cast<numberType>(outImageHeight) - 1);
                //Then add them to a ray
                ray3D currentRay = simCamera.getCurrentRay(normalisedX, normalisedY);
                //And sum them into the colour
                pixelColour += calcColour(currentRay, worldObjects, materialMaximumDepth);
            }

            //Then once we have our composite colour from all rays, use this function to scale it accordingly and write it to the file.
            writeColour(outImageStream,pixelColour,raysPerPixel);

            
        }
    }
}

