#ifndef _JOINT_H_
#define _JOINT_H_

#include <iostream>
#include "core.h"

class Joint
{
private:
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;

	glm::mat4 W;
	glm::mat4 L;
	glm::vec3 color;

	// Bounding Box Information
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	// joint data
	float length;
	glm::vec3 pose;
	glm::vec3 offset;
	glm::vec2 rotXLimit;
	glm::vec2 rotYLimit;
	glm::vec2 rotZLimit;

	// child joints
	std::vector<Joint*> children;

	void initializeBox();

public:
	Joint(float length, glm::vec3 pose, glm::vec3 offset,
		glm::vec2 rotXLimit, glm::vec2 rotYLimit, glm::vec2 rotZLimit);
	~Joint();

	void addChild(Joint* child);
	void draw(const glm::mat4& viewProjMtx, const GLuint shader);
	void update(const glm::mat4& parent);
	glm::vec3 getJointLocation();
	glm::vec3 getEndLocation();
	glm::vec3 jacobianX(glm::vec3 target);
	glm::vec3 jacobianY(glm::vec3 target);
	glm::vec3 jacobianZ(glm::vec3 target);
	void incrementPose(glm::vec3 deltaPose);
	void printPose();
};

#endif