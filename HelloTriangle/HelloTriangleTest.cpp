#include "HelloTriangle.h"
#include "GLesUtils/X11Util.h"
#include <memory>
int main(){
    auto util = std::make_unique<X11Util>();
    std::shared_ptr<GLES_context> hello_triangle = std::make_shared<HelloTriangle>();
    util->InitContext(hello_triangle, 500, 400, "Hello Triangle");
    util->MainLoop();

}