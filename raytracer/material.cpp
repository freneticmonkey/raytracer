//
//  material.cpp
//  SDL2Test
//
//  Created by Scott Porter on 23/09/13.
//

#include "material.h"

vec3 addColours(vec3 colourA, float scale, vec3 colourB)
{
    vec3 result;
    result.x = colourA.x + scale * colourB.x;
    result.y = colourA.y + scale * colourB.y;
    result.z = colourA.z + scale * colourB.z;
    
    return result;
}

// Default Constructor
Material::Material() :
	m_colour(vec3(1.0f)),
	m_otherColour(vec3(0.0f)),
    m_specular(0.2f),
    m_lambert(0.8f),
    m_turbulance(0.0f),
	m_marble(0.0f)
{
	m_ambient = 1.0f - m_specular - m_lambert;
}

Material::Material(vec3 colour, 
				   vec3 otherColour, 
				   float specular, 
				   float lambert, 
				   float turbulance, 
				   float marble, 
				   float bump)
{
    m_colour = colour;
	m_otherColour = otherColour;
    m_specular = specular;
    m_lambert = lambert;
    m_ambient = 1.0f - m_specular - m_lambert;

	m_turbulance = turbulance;
	m_marble = marble;
	m_bump = bump;
}

Material* Material::setColour(vec3 colour) 
{ 
	m_colour = colour; 
	return this; 
}

Material* Material::setOtherColour(vec3 otherColour) 
{ 
	m_otherColour = otherColour; 
	return this; 
}

Material* Material::setSpecular(float specular) 
{ 
	m_specular = specular; 
	return this; 
}

Material* Material::setLambert(float lambert) 
{ 
	m_lambert = lambert; 
	return this; 
}

Material* Material::setTurbulance(float turbulance) 
{ 
	m_turbulance = turbulance; 
	return this; 
}

Material* Material::setMarble(float marble) 
{ 
	m_marble = marble; 
	return this; 
}

Material* Material::setBump(float bump) 
{ 
	m_bump = bump; 
	return this; 
}

vec3 Material::colour()
{
	return m_colour;
}

float Material::specular()
{
	return m_specular;
}

float Material::lambert()
{
	return m_lambert;
}

float Material::ambient()
{
	return m_ambient;
}

vec3 Material::colourAt(vec3 point)
{
    return m_colour;
}

vec3 Material::colourAt(Ray ray, vec3 point, vec3 normal, Scene * scene)
{
    vec3 base = colourAt(point);
    vec3 colour(0.0f);
    
	if ( m_bump > 0.0f )
	{
		double mult = 10.0;
		double x = mult * point.x;
		double y = mult * point.y;
		double z = mult * point.z;

		float noiseCoefx = float( perlinnoise::noise(x, y, z) );
		float noiseCoefy = float( perlinnoise::noise(y, z, x) );
		float noiseCoefz = float( perlinnoise::noise(z, x, y) );

		x = (1.0f - m_bump ) * normal.x + m_bump * noiseCoefx;
		y = (1.0f - m_bump ) * normal.y + m_bump * noiseCoefy;
		z = (1.0f - m_bump ) * normal.z + m_bump * noiseCoefz;
		
		normal = normalize( vec3(x, y, z) );
	}

	if (m_specular > 0.0f )
    {
        Ray reflectedRay = Ray(point, reflect(ray.vector(), normal));
        vec3 reflectedColour = scene->rayColour(reflectedRay);
        colour = addColours(colour, m_specular, reflectedColour);
    }
    
    if (m_lambert > 0.0f)
    {
        float lambertAmount = 0.0f;
        
        for ( auto &light : scene->getVisibleLights(point) )
        {
            float contribution = dot( normalize((light - point)), normal );
            if ( contribution > 0.0f)
            {
                lambertAmount = lambertAmount + contribution;
                lambertAmount = min(1.0f, lambertAmount);
                colour = addColours(colour, lambertAmount, base);
            }
			// Shadows?
			/*
			else
			{
				colour = addColours(vec3(0.0f), 0.1f, base);
			}
			*/
        }
    }

	if ( m_ambient > 0.0f )
    {
        colour = addColours(colour, m_ambient, base);
    }

	if ( m_turbulance > 0.0f )
	{
		float noiseCoef = 0.0f;
		float coef = 1.0f;
		vec3 output(0.0f);
		float mult = 10.0f;//0.05f;

		for ( int level = 1; level < 10; level++ )
		{
			float m = level * mult;
			double x = m * point.x;
			double y = m * point.y;
			double z = m * point.z;

			noiseCoef += (1.0f / level) * fabs( float( perlinnoise::noise( x, y, z ) ) );
		}
		output = colour + coef * m_lambert * ( noiseCoef * m_colour + ( 1.0f - noiseCoef ) * m_otherColour );

		colour = addColours(colour, m_turbulance, output);
	}

	if ( m_marble > 0.0f && false)
	{
		vec3 output(0.0f);
		float noiseCoef = 0.0f;
		float coef = 1.0f;
		float mult = 10.0f;//0.05f;
		
		for (int level = 1; level < 10; level ++)
        {
			float m = level * mult;
            noiseCoef +=  (1.0f / level)  
            * fabsf(float(perlinnoise::noise(m * point.x,  
											 m * point.y,  
											 m * point.z)));
        };
        noiseCoef = 0.5f * sinf( (point.x + point.y) * 0.05f + noiseCoef) + 0.5f;
        output = colour +  coef * (m_lambert * 1.0f) * (noiseCoef * m_colour + (1.0f - noiseCoef) * m_otherColour);

		colour = addColours(colour, m_marble, output);
	}
    
    return colour;
}

CheckerboardMaterial::CheckerboardMaterial(vec3 colour,
                                           vec3 otherColour,
                                           float checkSize,
                                           float specular,
                                           float lambert) : Material(colour, otherColour, specular, lambert)
{
    m_checkSize = checkSize;
}

vec3 CheckerboardMaterial::colourAt(vec3 point)
{
    vec3 vec = point - vec3(0.0f);
    vec /= 1.0f / m_checkSize;
    if ( ( int(abs(vec.x) + 0.5f) + int(abs(vec.y) + 0.5f) + int(abs(vec.z) + 0.5f) ) % 2)
    {
        return m_otherColour;
    }
    else 
    {
        return m_colour;
    }
}