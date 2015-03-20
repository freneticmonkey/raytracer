//
//  material.h
//  SDL2Test
//
//  Created by Scott Porter on 23/09/13.
//

#ifndef SDL2Test_material_h
#define SDL2Test_material_h

#include "glm/glm.hpp"
#include "ray.h"
#include "scene.h"

#include "perlin.h"

using namespace glm;

class Scene;

vec3 addColours(vec3 colourA, float scale, vec3 colourB);

class Material
{
public:
	Material();
    Material(vec3 colour,
			 vec3 otherColour = vec3(0.0f),
			 float specular = 0.2f, 
			 float lambert = 0.8f,
			 float turbulance = 0.0f,
			 float marble = 0.0f,
			 float bump = 0.0f);
    ~Material() {};
    
	Material* setColour(vec3 colour);
	Material* setOtherColour(vec3 otherColour);
	Material* setSpecular(float specular);
	Material* setLambert(float lambert);
	Material* setTurbulance(float turbulance);
	Material* setMarble(float marble);
	Material* setBump(float bump);

   	vec3  colour();
    float specular();
    float lambert();
    float ambient();

	virtual vec3 colourAt(vec3 point);
    vec3 colourAt(Ray ray, vec3 point, vec3 normal, Scene * scene );
    
protected:
    vec3  m_colour;
	vec3 m_otherColour;

    float m_specular;
    float m_lambert;
    float m_ambient;

	float m_turbulance;
	float m_marble;
	float m_bump;
};

class CheckerboardMaterial : public Material
{
public:
    CheckerboardMaterial(vec3 colour = vec3(1.0f),
                         vec3 otherColour = vec3(0.0f),
                         float checkSize = 1.0f,
                         float specular = 0.2f,
                         float lambert = 0.7f);
    ~CheckerboardMaterial() {} ;
    
    vec3 colourAt(vec3 point);
    
private:
    float m_checkSize;
};

#endif
