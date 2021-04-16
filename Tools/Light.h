#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "Shader.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Model.h"

class Light
{
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	glm::vec3 brightness;

	Light(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 bright)
	{
		this->ambient = amb;
		this->diffuse = diff;
		this->specular = spec;
		this->brightness = bright;
	}
	Light(glm::vec3 color, glm::vec3 bright)
	{
		this->ambient = color;
		this->diffuse = color;
		this->specular = color;
		this->brightness = bright;
	}
	Light()
	{
		this->ambient = glm::vec3(0.1);
		this->diffuse = glm::vec3(0.7);
		this->specular = glm::vec3(1.0);
		this->brightness = glm::vec3(0.1f, 0.7f, 1.0f);
	}
	Light(float amb_r, float amb_g, float amb_b, float diff_r, float diff_g, float diff_b, float spec_r, float spec_g, float spec_b, float bright_a, float bright_d, float bright_s)
	{
		this->ambient = glm::vec3(amb_r, amb_g, amb_b);
		this->diffuse = glm::vec3(diff_r, diff_g, diff_b);
		this->specular = glm::vec3(spec_r, spec_g, spec_b);
		this->brightness = glm::vec3(bright_a, bright_d, bright_s);
	}
	virtual void AddToShader(Shader shader, string name)
	{
		shader.setVec3(name + ".ambient", ambient * brightness.r);
		shader.setVec3(name + ".diffuse", diffuse * brightness.g);
		shader.setVec3(name + ".specular", specular * brightness.b);
	}
	virtual void AddToShaderI(Shader shader, string name_light, int i)
	{
		shader.setVec3(name_light + "[" + std::to_string(i) + "].ambient", ambient * brightness.r);
		shader.setVec3(name_light + "[" + std::to_string(i) + "].diffuse", diffuse * brightness.g);
		shader.setVec3(name_light + "[" + std::to_string(i) + "].specular", specular * brightness.b);
	}
};

class DirectedLight : virtual public Light
{
public:
	glm::vec3 direction;

	DirectedLight(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 dir, glm::vec3 bright) : Light(amb, diff, spec, bright)
	{
		this->direction = dir;
	}

	DirectedLight() :Light()
	{
		direction = glm::vec3(0.0f, 0.0f, -1.0f);
	}

	DirectedLight(glm::vec3 color, glm::vec3 dir, glm::vec3 bright) : Light(color, bright)
	{
		direction = dir;
	}

	DirectedLight(float amb_r, float amb_g, float amb_b, float diff_r, float diff_g, float diff_b, float spec_r, float spec_g, float spec_b, float dir_x, float dir_y, float dir_z, float bright_a, float bright_d, float bright_s) : Light(amb_r, amb_g, amb_b, diff_r, diff_g, diff_b, spec_r, spec_g, spec_b, bright_a, bright_d, bright_s)
	{
		this->direction = glm::vec3(dir_x, dir_y, dir_z);
	}

	void AddToShader(Shader shader, string name)
	{
		Light::AddToShader(shader, name);
		shader.setVec3(name + ".direction", direction);
	}
	void AddToShaderI(Shader shader, string name_light, int i)
	{
		Light::AddToShaderI(shader, name_light, i);
		shader.setVec3(name_light + "[" + std::to_string(i) + "].direction", direction);
	}

};

class PointLight : virtual public Light
{
public:
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

	PointLight(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 pos, float constant, float linear, float quadratic, glm::vec3 bright) : Light(amb, diff, spec, bright)
	{
		this->position = pos;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
	}

	PointLight() :Light()
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		constant = 1.0f;
		linear = 0.045f;
		quadratic = 0.0075f;
	}

	PointLight(glm::vec3 color, glm::vec3 pos, float constant, float linear, float quadratic, glm::vec3 bright) : Light(color, bright)
	{
		this->position = pos;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
	}

	PointLight(float amb_r, float amb_g, float amb_b, float diff_r, float diff_g, float diff_b, float spec_r, float spec_g, float spec_b, float pos_x, float pos_y, float pos_z, float constant, float linear, float quadratic, float bright_a, float bright_d, float bright_s) : Light(amb_r, amb_g, amb_b, diff_r, diff_g, diff_b, spec_r, spec_g, spec_b, bright_a, bright_d, bright_s)
	{
		this->position = glm::vec3(pos_x, pos_y, pos_z);
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
	}

	void AddToShader(Shader shader, string name)
	{
		Light::AddToShader(shader, name);
		shader.setVec3(name + ".position", position);
		shader.setFloat(name + ".constant", constant);
		shader.setFloat(name + ".linear", linear);
		shader.setFloat(name + ".quadratic", quadratic);
	}
	void AddToShaderI(Shader shader, string name_light, int i)
	{
		Light::AddToShaderI(shader, name_light, i);
		shader.setVec3(name_light + "[" + std::to_string(i) + "].position", position);
		shader.setFloat(name_light + "[" + std::to_string(i) + "].constant", constant);
		shader.setFloat(name_light + "[" + std::to_string(i) + "].linear", linear);
		shader.setFloat(name_light + "[" + std::to_string(i) + "].quadratic", quadratic);
	}

	static void AddCount(Shader shader, string name_count, int lightCount)
	{
		shader.setInt(name_count, lightCount);
	}
};


class SpotLight : virtual public Light
{
public:
	glm::vec3 position;
	glm::vec3 direction;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;

	SpotLight(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 pos, glm::vec3 dir, float constant, float linear, float quadratic, glm::vec3 bright, float cutOff, float outerCutOff) : Light(amb, diff, spec, bright)
	{
		this->position = pos;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
		this->direction = dir;
		this->cutOff = cutOff;
		this->outerCutOff = outerCutOff;
	}

	SpotLight() :Light()
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		constant = 1.0f;
		linear = 0.045f;
		quadratic = 0.0075f;
		direction = glm::vec3(0.0f, 0.0f, -1.0f);
		cutOff = glm::cos(glm::radians(12.5f));
		outerCutOff = glm::cos(glm::radians(17.5f));
	}


	SpotLight(glm::vec3 color, glm::vec3 pos, glm::vec3 dir, float constant, float linear, float quadratic, glm::vec3 bright, float cutOff, float outerCutOff) : Light(color, bright)
	{
		this->position = pos;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
		this->direction = dir;
		this->cutOff = cutOff;
		this->outerCutOff = outerCutOff;
	}

	SpotLight(float amb_r, float amb_g, float amb_b, float diff_r, float diff_g, float diff_b, float spec_r, float spec_g, float spec_b, float pos_x, float pos_y, float pos_z, float dir_x, float dir_y, float dir_z, float constant, float linear, float quadratic, float cutOff, float outerCutOff, float bright_a, float bright_d, float bright_s) : Light(amb_r, amb_g, amb_b, diff_r, diff_g, diff_b, spec_r, spec_g, spec_b, bright_a, bright_d, bright_s)
	{
		this->position = glm::vec3(pos_x, pos_y, pos_z);
		this->direction = glm::vec3(dir_x, dir_y, dir_z);
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
		this->cutOff = cutOff;
		this->outerCutOff = outerCutOff;
	}

	void AddToShader(Shader shader, string name)
	{
		Light::AddToShader(shader, name);
		shader.setVec3(name + ".position", position);
		shader.setVec3(name + ".direction", direction);
		shader.setFloat(name + ".constant", constant);
		shader.setFloat(name + ".linear", linear);
		shader.setFloat(name + ".quadratic", quadratic);
		shader.setFloat(name + ".cutOff", cutOff);
		shader.setFloat(name + ".outerCutOff", outerCutOff);

	}
	void AddToShaderI(Shader shader, string name_light, int i)
	{
		Light::AddToShaderI(shader, name_light, i);
		shader.setVec3(name_light + "[" + std::to_string(i) + "].position", position);
		shader.setVec3(name_light + "[" + std::to_string(i) + "].direction", direction);
		shader.setFloat(name_light + "[" + std::to_string(i) + "].constant", constant);
		shader.setFloat(name_light + "[" + std::to_string(i) + "].linear", linear);
		shader.setFloat(name_light + "[" + std::to_string(i) + "].quadratic", quadratic);
		shader.setFloat(name_light + "[" + std::to_string(i) + "].cutOff", cutOff);
		shader.setFloat(name_light + "[" + std::to_string(i) + "].outerCutOff", outerCutOff);
	}

	static void AddCount(Shader shader, string name_count, int lightCount)
	{
		shader.setInt(name_count, lightCount);
	}
};

#endif