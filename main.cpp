#include <3ds.h>
#include <citro2d.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#define SCREEN_WIDTH_TOP  400
#define SCREEN_HEIGHT_TOP 240
#define SCREEN_WIDTH_BOT  320
#define SCREEN_HEIGHT_BOT 240

// Colors
#define CLR_TASKBAR      C2D_Color32(15, 40, 75, 200)
#define CLR_WIN7_BLUE    C2D_Color32(0, 110, 210, 255)
#define CLR_WHITE        C2D_Color32(255, 255, 255, 255)
#define CLR_BLACK        C2D_Color32(0, 0, 0, 255)
#define CLR_WIN_FRAME    C2D_Color32(210, 230, 250, 230)
#define CLR_WIN_TITLE    C2D_Color32(35, 85, 155, 255)
#define CLR_CMD_BG       C2D_Color32(10, 10, 10, 255)

enum AppType { NONE, NOTEPAD, CALC, PAINT, MINESWEEPER, BROWSER, MEDIA_PLAYER, SOLITAIRE, CMD };

struct Window {
    AppType type;
    float x, y, w, h;
    bool active;
    const char* title;
};

// Global State
bool startMenuOpen = false;
std::vector<Window> windows;
char timeStr[15];
int soundTimer = 0;

void updateTime() {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timeStr, sizeof(timeStr), "%I:%M:%S %p", timeinfo);
}

void playClickSound() {
    // Placeholder for actual audio trigger
    // In real 3DS, would use ndsp or csnd
}

void openApp(AppType type, const char* title) {
    playClickSound();
    windows.push_back({type, 50.0f + (windows.size()*8), 40.0f + (windows.size()*8), 220, 140, true, title});
    startMenuOpen = false;
}

void drawWindow(Window& win) {
    if (!win.active) return;
    C2D_DrawRectSolid(win.x, win.y, 0, win.w, win.h, CLR_WIN_FRAME);
    C2D_DrawRectSolid(win.x, win.y, 0, win.w, 22, CLR_WIN_TITLE);
    C2D_DrawRectSolid(win.x + win.w - 22, win.y + 2, 0, 18, 18, C2D_Color32(220, 50, 50, 255));
    
    // Content Area
    if (win.type == CMD) {
        C2D_DrawRectSolid(win.x + 2, win.y + 24, 0, win.w - 4, win.h - 26, CLR_CMD_BG);
        // Simple CMD text prompt placeholder
        C2D_DrawRectSolid(win.x + 10, win.y + 35, 0, 80, 2, C2D_Color32(200, 255, 200, 255));
    } else if (win.type == SOLITAIRE) {
        C2D_DrawRectSolid(win.x + 2, win.y + 24, 0, win.w - 4, win.h - 26, C2D_Color32(0, 100, 0, 255));
        for(int i=0; i<4; i++) C2D_DrawRectSolid(win.x + 15 + i*50, win.y + 40, 0, 35, 50, CLR_WHITE);
    } else {
        C2D_DrawRectSolid(win.x + 2, win.y + 24, 0, win.w - 4, win.h - 26, CLR_WHITE);
    }
}

int main(int argc, char* argv[]) {
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget* top = C2D_CreateMainTarget(GFX_TOP, GFX_LEFT, C2D_TARGET_CLEAR);
    C3D_RenderTarget* bottom = C2D_CreateMainTarget(GFX_BOTTOM, GFX_LEFT, C2D_TARGET_CLEAR);

    touchPosition touch;
    float cursorX = SCREEN_WIDTH_BOT / 2;
    float cursorY = SCREEN_HEIGHT_BOT / 2;

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;

        if (hidKeysHeld() & KEY_TOUCH) {
            hidTouchRead(&touch);
            cursorX = touch.px;
            cursorY = touch.py;
            
            if (kDown & KEY_TOUCH) {
                if (touch.px < 40 && touch.py > SCREEN_HEIGHT_BOT - 40) startMenuOpen = !startMenuOpen;
                
                for (auto &win : windows) {
                    if (win.active && touch.px > win.x + win.w - 22 && touch.px < win.x + win.w && touch.py > win.y && touch.py < win.y + 22) {
                        win.active = false;
                        playClickSound();
                    }
                }

                if (startMenuOpen && touch.px < 150) {
                    if (touch.py > 60 && touch.py < 80) openApp(NOTEPAD, "Notepad");
                    else if (touch.py > 85 && touch.py < 105) openApp(CALC, "Calculator");
                    else if (touch.py > 110 && touch.py < 130) openApp(SOLITAIRE, "Solitaire");
                    else if (touch.py > 135 && touch.py < 155) openApp(CMD, "Command Prompt");
                }
            }
        }

        updateTime();

        C3D_FrameBegin(C3D_FRAME_SYNCHRPARAMS);
        C2D_TargetClear(top, CLR_WIN7_BLUE);
        C2D_SceneBegin(top);
        
        C2D_TargetClear(bottom, CLR_WIN7_BLUE);
        C2D_SceneBegin(bottom);

        C2D_DrawRectSolid(0, SCREEN_HEIGHT_BOT - 35, 0, SCREEN_WIDTH_BOT, 35, CLR_TASKBAR);
        C2D_DrawCircleSolid(20, SCREEN_HEIGHT_BOT - 17, 0, 15, C2D_Color32(0, 160, 230, 255));

        for (auto &win : windows) drawWindow(win);

        if (startMenuOpen) {
            C2D_DrawRectSolid(5, 50, 0, 160, 150, C2D_Color32(240, 245, 255, 250));
            for(int i=0; i<4; i++) C2D_DrawRectSolid(10, 60 + i*25, 0, 140, 20, C2D_Color32(210, 225, 245, 255));
        }

        C2D_DrawTriangle(cursorX, cursorY, CLR_WHITE, cursorX+12, cursorY+6, CLR_WHITE, cursorX+6, cursorY+12, CLR_WHITE, 0);
        C3D_FrameEnd(0);
    }

    C2D_Fini(); C3D_Fini(); gfxExit();
    return 0;
}
