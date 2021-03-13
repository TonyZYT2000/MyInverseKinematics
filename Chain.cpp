#include "Chain.h"

Chain::Chain(int count, glm::vec3 offset) {
      // model matrix
      model = glm::translate(glm::mat4(1), offset) * glm::mat4(1);

      // bounding box value
      auto boxMin = glm::vec3(-0.1, 0, -0.1);
      auto boxMax = glm::vec3(0.1, 1, 0.1);

      // two different joint limit
      auto fixed = glm::vec2(0, 0);
      auto noLimit = glm::vec2(-100000, 100000);
      
      // root joint, with limit on x axis and z axis
      root = new Joint(1, glm::vec3(0), glm::vec3(0),
            glm::vec2(fixed), glm::vec2(fixed), glm::vec2(noLimit));
      joints.push_back(root);

      // record previous joint to build the chain
      Joint* prevJoint = root;
      // create child joint one by one, connected together into a chain
      for (int i = 1; i < count; ++i) {
            // intermediate joint with no limit
            Joint* joint = new Joint(1, glm::vec3(0), glm::vec3(0, 1, 0),
                  glm::vec2(noLimit), glm::vec2(noLimit), glm::vec2(noLimit));
            prevJoint->addChild(joint);
            joints.push_back(joint);
            prevJoint = joint;
      }
}

Chain::~Chain() {
      // delete from root, joint delete handle child deletion
      delete root;
}

void Chain::draw(const glm::mat4& viewProjMtx, GLuint shader) {
      root->draw(viewProjMtx, shader);
}

void Chain::update() {
      root->update(model);
}

// Do inverse kinematics to move the chain toward the target
void Chain::moveToward(glm::vec3 target) {
      // difference between the target and the end of the chain
      glm::vec3 difference = target - joints[joints.size() - 1]->getEndLocation();
      // if not close enough
      if (glm::length(difference) > 0.01) {
            // calculate jacobian for each joint and calculate the angle they should move
            for (auto joint : joints) {
                  float deltaX = 0.001 * glm::dot(joint->jacobianX(target), difference);
                  float deltaY = 0.001 * glm::dot(joint->jacobianY(target), difference);
                  float deltaZ = 0.001 * glm::dot(joint->jacobianZ(target), difference);
                  // increment pose to move toward the target
                  joint->incrementPose(glm::vec3(deltaX, deltaY, deltaZ));
            }
      }
}