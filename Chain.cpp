#include "Chain.h"

Chain::Chain(int count, glm::vec3 offset) {
      model = glm::translate(glm::mat4(1), offset) * glm::mat4(1);
      auto boxMin = glm::vec3(-0.1, 0, -0.1);
      auto boxMax = glm::vec3(0.1, 1, 0.1);
      root = new Joint(1, glm::vec3(0), glm::vec3(0),
            glm::vec2(0), glm::vec2(0), glm::vec2(0));

      joints.push_back(root);
      Joint* prevJoint = root;
      for (int i = 1; i < count; ++i) {
            Joint* joint = new Joint(1, glm::vec3(0), glm::vec3(0, 1, 0),
                  glm::vec2(0), glm::vec2(0), glm::vec2(0));
            prevJoint->addChild(joint);
            joints.push_back(joint);
            prevJoint = joint;
      }
}

Chain::~Chain() {
      delete root;
}

void Chain::draw(const glm::mat4& viewProjMtx, GLuint shader) {
      root->draw(viewProjMtx, shader);
}

void Chain::update() {
      root->update(model);
}

void Chain::moveToward(glm::vec3 target) {
      glm::vec3 difference = target - joints[joints.size() - 1]->getEndLocation();
      if (glm::length(difference) > 0.01) {
            float deltaX = 0.001 * glm::dot(root->jacobianX(target), difference);
            float deltaY = 0.001 * glm::dot(root->jacobianY(target), difference);
            float deltaZ = 0.001 * glm::dot(root->jacobianZ(target), difference);
            root->incrementPose(glm::vec3(deltaX, deltaY, deltaZ));
      }
}