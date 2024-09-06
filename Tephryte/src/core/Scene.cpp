//
// Created by Timothy on 8/31/2024.
//

#include "Scene.h"

namespace Tephryte {

    Transform SceneObject::getTransform() {
        return parent == nullptr ? transform : parent->getTransform();
    }

    SceneObject* SceneObject::addChild() {
        auto* child = new SceneObject();

        child->parent = this;
        children.insert(child);

        return child;
    }

    void SceneObject::removeChild(SceneObject* child) {
        children.erase(child);
        delete child;
    }

    SceneObject::~SceneObject() {
        for (auto child: children) {
            delete child;
        }
        for (auto attachment: attachments) {
            delete attachment;
        }
    }



} // Tephryte