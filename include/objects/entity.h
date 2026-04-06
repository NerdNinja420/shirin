#ifndef ENTITY_H_
#define ENTITY_H_

#include "math/vec2.h"

// Base class for all game objects that have a world position.
struct Entity {
    Vec2 position;
    virtual ~Entity() = default;
};

#endif
