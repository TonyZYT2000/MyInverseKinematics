#ifndef _CHAIN_H_
#define _CHAIN_H_

#include "core.h"
#include "Joint.h"

////////////////////////////////////////////////////////////////////////////////

class Chain
{
private:
	Joint* root;
	glm::mat4 model;
	std::vector<Joint*> joints;

public:
	Chain(int count, glm::vec3 offset);
	~Chain();

	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	void update();
};

////////////////////////////////////////////////////////////////////////////////

#endif
