#pragma once
#include <string>
#include <iostream>
#include "GameObject.h"
#include "Vector2D.h"
#include "Transform.h"

using namespace std;
class Character : public GameObject {
    public:
        Character(Properties* props) : GameObject(props) {

        }

        virtual void Draw() = 0;
        virtual void Update(float dt) = 0;
        virtual void Clean() = 0;
        virtual ~Character() {}

    protected:
       string m_name;
};
