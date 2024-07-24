#include "Application.h"
#include "Log.h"
#include "glm/glm.hpp"

struct test {
    int a = 1;
    long b = 2;
    bool c = true;
};

int main() {
    auto *app = new Tephryte::Application;
    test a = {1,19};
    glm::vec2 b = {1, 1 };
    // glm::mat4 c = {b, b, b, b};
    std::vector d  = {b, b};
    std::vector e = {d, d};
    std::vector f = {e, e};

    TPR_ENGINE_ERROR("No App Found", 2, 2.0f, f);
    app->run();

    delete app;

    return 0;
}









