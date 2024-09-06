//
// Created by Timothy on 8/31/2024.
//

#ifndef SCENE_H
#define SCENE_H
#include <unordered_set>

#include "TephryteCore.h"



namespace Tephryte {
    class Scene;
    class Attachment;

    class SceneObject {
    public:
        SceneObject*                        parent = nullptr;
        std::unordered_set<SceneObject*>    children = { };
        std::unordered_set<Attachment*>     attachments = { };
        Transform                           transform = { };

        Transform getTransform();

        SceneObject* addChild();

        void removeChild(SceneObject* child);

        SceneObject(const SceneObject& obj) = delete;
        SceneObject(SceneObject&& obj) = delete;

        friend Scene;

    private:
        SceneObject() = default;
        ~SceneObject();
    };

    class Attachment{
    public:
        SceneObject* parent = nullptr;

        virtual ~Attachment() = default;

    };

    class Scene {
    public:
        SceneObject root = {};

    };





} // Tephryte

#endif //SCENE_H
