#include "Tephryte.h"
#include "glm.hpp"

int main() {
    auto *app = new Tephryte::Application;
    glm::vec2 vec = {0, 0};
    TPR_ENGINE_ERROR("No App Found", 6, 5475.0654f, vec)
//    app->run();

    delete app;

    return 0;
}









