
#include <new> // std::nothrow

#include "lightSky.h"

#include "controlState.h"
#include "fbState.h"
#include "uiState.h"

bool initLs();
void terminateLs();

namespace {
    lsSystem* pSystem = nullptr;
    lsDisplay* pDisplay = nullptr;
}

/*
 * Main
 */
int main(int argc, char** argv) {
    std::cout << "Initializing LightSky.\n";
    
    /*
     * Game Initialization
     */
    std::cout << "Parameter count: " << argc;
    for (int argCount = 0; argCount < argc; ++argCount) {
        std::cout << '\t' << argCount << ":\t" << argv[argCount] << '\n';
    }
    std::cout << std::endl;
    
    if (!initLs()) {
        terminateLs();
        return false;
    }
    
    // push some states and run the game
    if (!pSystem->pushGameState(new controlState{})
    ||  !pSystem->pushGameState(new fbState{})
    ||  !pSystem->pushGameState(new uiState{})
    ) {
        terminateLs();
        return -1;
    }
    
    while (pSystem->isRunning()) {
        pSystem->run();
    }
    
    terminateLs();
    
    return 0;
}

/*
 * Initialize Light Sky
 */
bool initLs() {
    if (!lsInit()) {
        std::cerr
            << "Unable to initialize LightSky."
            << std::endl;
        return false;
    }
    
    pDisplay = new(std::nothrow) lsDisplay{};
    if (!pDisplay || !pDisplay->init(math::vec2i{800, 600})) {
        std::cerr << "Unable to create a display object for LightSky." << std::endl;
        return false;
    }
    
    pSystem = new(std::nothrow) lsSystem();
    if (!pSystem || !pSystem->init(*pDisplay, true)) {
        std::cerr << "Failed to initialize the LightSky system manager.\n" << std::endl;
        return false;
    }
    
    std::cout << "LightSky Successfully initialized.\n" << std::endl;;
    return true;
}

/*
 * Terminate lightSky
 */
void terminateLs() {
    delete pSystem;
    pSystem = nullptr;
    
    delete pDisplay;
    pDisplay = nullptr;
    
    lsTerminate();
    
    std::cout << "LightSky successfully terminated.\n" << std::endl;
}