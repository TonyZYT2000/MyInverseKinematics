#include "Joint.h"

// Give initial value
Joint::Joint(float length, glm::vec3 pose, glm::vec3 offset,
		glm::vec2 rotXLimit, glm::vec2 rotYLimit, glm::vec2 rotZLimit) : 
		length(length), pose(pose), offset(offset),
		rotXLimit(rotXLimit), rotYLimit(rotYLimit), rotZLimit(rotZLimit) {
	VAO = 0;
	EBO = 0;
	VBO_normals = 0;
	VBO_positions = 0;
	W = glm::mat4(1);
	L = glm::mat4(1);

	pose.x = glm::clamp(pose.x, rotXLimit.x, rotXLimit.y);
	pose.y = glm::clamp(pose.y, rotYLimit.x, rotYLimit.y);
	pose.z = glm::clamp(pose.z, rotZLimit.x, rotZLimit.y);

	// 4 operations, translation and 3 rotations
	glm::mat4 translate = glm::translate(glm::mat4(1), offset);
	glm::mat4 rotX = glm::rotate(pose.x, glm::vec3(1, 0, 0));
	glm::mat4 rotY = glm::rotate(pose.y, glm::vec3(0, 1, 0));
	glm::mat4 rotZ = glm::rotate(pose.z, glm::vec3(0, 0, 1));
	// calculate local matrix
	L = translate * rotZ * rotY * rotX * L;

	color = glm::vec3(0, 1, 1);
	positions = std::vector<glm::vec3>();
	normals = std::vector<glm::vec3>();
	indices = std::vector<unsigned int>();

      initializeBox();
}

Joint::~Joint() {
	// delete all children
	for (Joint* child : children) {
		delete child;
	}

	// delete buffers
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}


void Joint::addChild(Joint* child) {
	children.push_back(child);
}

void Joint::draw(const glm::mat4& viewProjMtx, const GLuint shader) {
	// actiavte the shader program 
	glUseProgram(shader);

	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&W);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);

	for (Joint* child : children) {
		child->draw(viewProjMtx, shader);
	}
}

void Joint::update(const glm::mat4& parent) {
	// calculate world matrix
	W = parent * L;

	// update all children's world matrix
	for (Joint* child : children) {
		child->update(W);
	}
}

glm::vec3 Joint::getJointLocation() {
	// return world location of the joint
	return glm::vec3(W * glm::vec4(glm::vec3(0), 1));
}

glm::vec3 Joint::getEndLocation() {
	// return world location of the far end of the bounding box
	return glm::vec3(W * glm::vec4(0, length, 0, 1));
}

glm::vec3 Joint::jacobianX(glm::vec3 target) {
	// calculate jacobian of x axis
	glm::vec3 axis = glm::vec3(W * glm::vec4(1, 0, 0, 0));
	glm::vec3 difference = target - getJointLocation();
	return glm::cross(axis, difference);
}

glm::vec3 Joint::jacobianY(glm::vec3 target) {
	// calculate jacobian of y axis
	glm::vec3 axis = glm::vec3(W * glm::vec4(0, 1, 0, 0));
	glm::vec3 difference = target - getJointLocation();
	return glm::cross(axis, difference);
}

glm::vec3 Joint::jacobianZ(glm::vec3 target) {
	// calculate jacobian of z axis
	glm::vec3 axis = glm::vec3(W * glm::vec4(0, 0, 1, 0));
	glm::vec3 difference = target - getJointLocation();
	return glm::cross(axis, difference);
}

void Joint::incrementPose(glm::vec3 deltaPose) {
	// increment pose
	pose += deltaPose;

	// clamp the pose so not exceeding limits1
	pose.x = glm::clamp(pose.x, rotXLimit.x, rotXLimit.y);
	pose.y = glm::clamp(pose.y, rotYLimit.x, rotYLimit.y);
	pose.z = glm::clamp(pose.z, rotZLimit.x, rotZLimit.y);

	// 4 operations, translation and 3 rotations
	glm::mat4 translate = glm::translate(glm::mat4(1), offset);
	glm::mat4 rotX = glm::rotate(pose.x, glm::vec3(1, 0, 0));
	glm::mat4 rotY = glm::rotate(pose.y, glm::vec3(0, 1, 0));
	glm::mat4 rotZ = glm::rotate(pose.z, glm::vec3(0, 0, 1));

	// update local matrix
	L = translate * rotZ * rotY * rotX * glm::mat4(1);
}

void Joint::initializeBox() {
	// Specify vertex positions
	positions = {
		// Front
		glm::vec3(-0.1,0,0.1),
		glm::vec3(0.1,0,0.1),
		glm::vec3(0.1,length,0.1),
		glm::vec3(-0.1,length,0.1),

		// Back
		glm::vec3(0.1,0,-0.1),
		glm::vec3(-0.1,0,-0.1),
		glm::vec3(-0.1,length,-0.1),
		glm::vec3(0.1,length,-0.1),

		// Top
		glm::vec3(-0.1,length,0.1),
		glm::vec3(0.1,length,0.1),
		glm::vec3(0.1,length,-0.1),
		glm::vec3(-0.1,length,-0.1),

		// Bottom
		glm::vec3(-0.1,0,-0.1),
		glm::vec3(0.1,0,-0.1),
		glm::vec3(0.1,0,0.1),
		glm::vec3(-0.1,0,0.1),

		// Left
		glm::vec3(-0.1,0,-0.1),
		glm::vec3(-0.1,0,0.1),
		glm::vec3(-0.1,length,0.1),
		glm::vec3(-0.1,length,-0.1),

		// Right
		glm::vec3(0.1,0,0.1),
		glm::vec3(0.1,0,-0.1),
		glm::vec3(0.1,length,-0.1),
		glm::vec3(0.1,length,0.1)
	};

	// Specify normals
	normals = {
		// Front
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),

		// Back			
		glm::vec3(0,0,-1),
		glm::vec3(0,0,-1),
		glm::vec3(0,0,-1),
		glm::vec3(0,0,-1),

		// Top
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),

		// Bottom
		glm::vec3(0,-1,0),
		glm::vec3(0,-1,0),
		glm::vec3(0,-1,0),
		glm::vec3(0,-1,0),

		// Left
		glm::vec3(-1,0,0),
		glm::vec3(-1,0,0),
		glm::vec3(-1,0,0),
		glm::vec3(-1,0,0),

		// Right
		glm::vec3(1,0,0),
		glm::vec3(1,0,0),
		glm::vec3(1,0,0),
		glm::vec3(1,0,0)
	};

	// Specify indices
	indices = {
		0,1,2,	0,2,3,		// Front
		4,5,6,	4,6,7,		// Back
		8,9,10,	8,10,11,		// Top
		12,13,14,	12,14,15,		// Bottom
		16,17,18,	16,18,19,		// Left
		20,21,22,	20,22,23,		// Right
	};


	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO and send the data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
