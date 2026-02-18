//
// Created by omar on 09.11.25.
//

#ifndef MYCPPPROJECT_PROPERTIES_H
#define MYCPPPROJECT_PROPERTIES_H

#include <tuple>

#include "imgui.h"

namespace  UI {
    static float GRID_SIZE = 400;
    static int CELLS_X = 10;
    static int CELLS_Y = 10;

    static ImVec4 grid_color = ImVec4(0.369 ,0.788, 1, 1.0);
    static ImVec4 wp_color =  ImVec4(24.0f / 255.0f,15.0f / 255.0f,58.0f / 255.0f,1.0f );
};

#endif //MYCPPPROJECT_PROPERTIES_H