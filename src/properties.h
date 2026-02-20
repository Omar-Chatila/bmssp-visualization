//
// Created by omar on 09.11.25.
//

#ifndef MYCPPPROJECT_PROPERTIES_H
#define MYCPPPROJECT_PROPERTIES_H

#include "imgui.h"

#define DIJKSTRA_EVENTS 3
#define BMSSP_EVENTS 4

enum class ProgramState {
    StartSelection,
    Execution
};

enum DijkstraEvent {
    D_PQ,
    D_FIN,
    D_NA
};

enum BMSSPEvent {
    B_FR,
    B_PIV,
    B_FIN,
    B_NA
};

namespace  UI {
    static float GRID_SIZE = 400;
    static int CELLS_X = 10;
    static int CELLS_Y = 10;

    static ImVec4 grid_color = ImVec4(0.369f ,0.788f, 1.0f, 1.0f);
    static ImVec4 wp_color =  ImVec4(24.0f / 255.0f,15.0f / 255.0f,58.0f / 255.0f,1.0f );

    static int start_x = 0, start_y = 0;
    static ImVec4 start_color = {0.0f, 1.0f, 0.0f, 1.0f};

    static ProgramState state = ProgramState::StartSelection;

    namespace dijkstra {
        static bool events[DIJKSTRA_EVENTS];
        static ImVec4 event_cols[DIJKSTRA_EVENTS] = {
            {0.95f, 0.75f, 0.20f, 1.0f},  // In PQ: Gold
            {0.80f, 0.25f, 0.25f, 1.0f},  // Finalized: Red
            {0.35f, 0.40f, 0.45f, 1.0f}   // Undiscovered: Grey/Blue
        };
    }

    namespace bmssp {
        static bool events[BMSSP_EVENTS];
        static ImVec4 event_cols[BMSSP_EVENTS] = {
            {0.95f, 0.75f, 0.20f, 1.0f},  // Frontier: Gold
            {0.60f, 0.40f, 0.85f, 1.0f},  // Pivot: Purple
            {0.80f, 0.25f, 0.25f, 1.0f},  // Finalized: Red
            {0.35f, 0.40f, 0.45f, 1.0f}   // Undiscovered: Grey/Blue
        };
    }
};

#endif //MYCPPPROJECT_PROPERTIES_H