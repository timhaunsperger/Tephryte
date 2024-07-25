#include <vector>
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

    app->run();

    delete app;

    return 0;
}









