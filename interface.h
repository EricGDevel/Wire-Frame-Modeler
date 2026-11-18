#if !defined(INTERFACE_HEADER)
#define INTERFACE_HEADER

#include "renderer.h"
#include "scene.h"
#include "primitives.h"

void CreateInterface(RenderSettings *settings, Scene *scene);

#endif // INTERFACE_HEADER


// UI Implementation
#if defined(INTERFACE_IMPLEMENTATION)
#if !defined(INTERFACE_IMPLEMENTED)
#define INTERFACE_IMPLEMENTED

#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include <gui_window_file_dialog.h> // For file dialogs

// enums
typedef enum FileDialogOperaion {
    FILE_DIALOG_LOAD_MODEL = 1,
} FileDialogOperaion;

//
// Provided helper functions
//
static Rectangle IntBox(int *value, bool *editMode, bool *dragging, char *textValue, int textValueCap, float *left,
                        int uiTop) {
    Rectangle rect = (Rectangle){*left, uiTop, 40, 16};
    *left += rect.width + 2;

    bool mouseInRect = CheckCollisionPointRec(GetMousePosition(), rect);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        *editMode = !GuiIsLocked() && mouseInRect;
        *dragging = *editMode;
    }
    if (IsKeyReleased(KEY_ENTER)) {
        *editMode = false;
    }

    if (*dragging) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();

            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                delta.y *= 10;
            }
            *value -= (int)(delta.y / 2.f);

            snprintf(textValue, textValueCap, "%i", *value);
        } else {
            *dragging = false;
            if (!mouseInRect) {
                *editMode = false;
            }
        }
    }

    if (!(*editMode)) {
        snprintf(textValue, textValueCap, "%i", *value);
    } else {
        if (IsKeyPressed(KEY_BACKSPACE) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
            memset(textValue, '\0', textValueCap);
        }
    }

    GuiValueBoxInt(rect, NULL, textValue, textValueCap, value, *editMode);

    return rect;
}

static Rectangle FloatBox(float *value, bool *editMode, bool *dragging, char *textValue, int textValueCap, float *left,
                          float uiTop) {
    Rectangle rect = (Rectangle){*left, uiTop, 40, 16};
    *left += rect.width + 2;

    bool mouseInRect = CheckCollisionPointRec(GetMousePosition(), rect);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        *editMode = !GuiIsLocked() && mouseInRect;
        *dragging = *editMode;
    }
    if (IsKeyReleased(KEY_ENTER)) {
        *editMode = false;
    }

    if (*dragging) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();

            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                delta.y *= 10;
            }
            *value -= delta.y * 0.01f;

            snprintf(textValue, textValueCap, "%2.2f", *value);
        } else {
            *dragging = false;
            if (!mouseInRect) {
                *editMode = false;
            }
        }
    }

    if (!(*editMode)) {
        snprintf(textValue, textValueCap, "%2.2f", *value);
    } else {
        if (IsKeyPressed(KEY_BACKSPACE) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
            memset(textValue, '\0', textValueCap);
        }
    }
    GuiValueBoxFloat(rect, NULL, textValue, textValueCap, value, *editMode);

    return rect;
}

// Macros for adding float/int edit boxes that can be changed by dragging up/down with the mouse,
// or manually with the keyboard. The 'left' parameter is automatically incremented with the box width.
// The reason those are macros and not functions, is that the statet of the box needs to stay persistant,
// and the macros are copied into place by the preprocessors so each static variable here has it's own
// place in memory and not collide with other boxes data.
#define BOX_FLOAT(x, left, top, tip)                                                                                   \
    do {                                                                                                               \
        float *value = x;                                                                                              \
        int boxId = boxCounter++;                                                                                      \
        static bool dragging = false;                                                                                  \
        bool editMode = (boxId == activeBoxId);                                                                        \
        if (activateBox && editMode) {                                                                                 \
            activateBox = false;                                                                                       \
        }                                                                                                              \
        bool guiLocked = editMode && GuiIsLocked();                                                                    \
        if (guiLocked)                                                                                                 \
            GuiUnlock();                                                                                               \
                                                                                                                       \
        static char textValue[RAYGUI_VALUEBOX_MAX_CHARS + 1] = {0};                                                    \
        Rectangle rect = FloatBox(value, &editMode, &dragging, textValue, sizeof(textValue), left, top);               \
        if (editMode) {                                                                                                \
            activeBoxId = boxId;                                                                                       \
            GuiLock();                                                                                                 \
        } else if (boxId == activeBoxId) {                                                                             \
            activeBoxId = -1;                                                                                          \
        }                                                                                                              \
        if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {                                      \
            tooltip = tip;                                                                                             \
        }                                                                                                              \
        if (guiLocked)                                                                                                 \
            GuiLock();                                                                                                 \
    } while (0)

#define BOX_FLOAT_CLAMP0(x, left, top, tip)                                                                            \
    do {                                                                                                               \
        BOX_FLOAT(x, left, top, tip);                                                                                  \
        *x = MAX(*x, 0.f);                                                                                             \
    } while (0)

#define BOX_FLOAT_CLAMP01(x, left, top, tip)                                                                           \
    do {                                                                                                               \
        BOX_FLOAT(x, left, top, tip);                                                                                  \
        *x = CLAMP(*x, 0.f, 1.f);                                                                                      \
    } while (0)

#define BOX_INT(x, left, top, tip)                                                                                     \
    do {                                                                                                               \
        int *value = x;                                                                                                \
        int boxId = boxCounter++;                                                                                      \
        static bool dragging = false;                                                                                  \
        bool editMode = (boxId == activeBoxId);                                                                        \
        if (activateBox && editMode) {                                                                                 \
            activateBox = false;                                                                                       \
        }                                                                                                              \
        bool guiLocked = editMode && GuiIsLocked();                                                                    \
        if (guiLocked)                                                                                                 \
            GuiUnlock();                                                                                               \
                                                                                                                       \
        static char textValue[RAYGUI_VALUEBOX_MAX_CHARS + 1] = {0};                                                    \
        Rectangle rect = IntBox(value, &editMode, &dragging, textValue, sizeof(textValue), left, top);                 \
        if (editMode) {                                                                                                \
            activeBoxId = boxId;                                                                                       \
            GuiLock();                                                                                                 \
        } else if (boxId == activeBoxId) {                                                                             \
            activeBoxId = -1;                                                                                          \
        }                                                                                                              \
        if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {                                      \
            tooltip = tip;                                                                                             \
        }                                                                                                              \
        if (guiLocked)                                                                                                 \
            GuiLock();                                                                                                 \
    } while (0)

#define BOX_INT_CLAMP(x, min, max, left, top, tip)                                                                     \
    do {                                                                                                               \
        BOX_INT(x, left, top, tip);                                                                                    \
        *x = CLAMP(*x, min, max);                                                                                      \
    } while (0)

static inline Vec4 V4FromColor(Color c) {
    return (Vec4){(float)(c).r / 255.f, (float)(c).g / 255.f, (float)(c).b / 255.f, (float)(c).a / 255.f};
}

static inline Color ColorFromV4(Vec4 v) {
    return (Color){(int)(v.r * 255.f), (int)(v.g * 255.f), (int)(v.b * 255.f), (int)(v.a * 255.f)};
}

static void Tooltip(const char *label, int x, int y) {
    GuiSetTooltip(label);
    GuiEnableTooltip();
    int width = GuiGetTextWidth(label);
    int height = GuiGetStyle(DEFAULT, TEXT_SIZE);
    int textAlign = GuiGetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL);

    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_CENTER);
    GuiTooltip((Rectangle){x, y, width, height});
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, textAlign);
    GuiDisableTooltip();
}

// Helper: Recompute camera target and up vectors from Position and Euler rotation
static void SyncCameraVectorsFromRotation(Camera *cam) {
    if (!cam) {
        return;
    }
    float dist = V3Length(V3Sub(cam->target, cam->Position));
    if (dist < EPSILON) {
        dist = 3.0f;
    }
    Mat3 basis = GetRotationM3(cam->rotation);
    Vec3 forward = M3MulV3(basis, (Vec3){0.0f, 0.0f, -1.0f});
    Vec3 up = M3MulV3(basis, (Vec3){0.0f, 1.0f, 0.0f});
    cam->target = V3Add(cam->Position, V3MulS(forward, dist));
    cam->up = V3Normalize(up);
}

// Helper: Aim camera target, up, and Euler angles (pitch = rotation.x, yaw = rotation.y) at targetPos
static void FocusCameraOnPoint(Camera *cam, Vec3 targetPos) {
    if (!cam) {
        return;
    }
    Vec3 dir = V3Sub(targetPos, cam->Position);
    float dist = V3Length(dir);
    if (dist < EPSILON) {
        return;
    }
    cam->target = targetPos;
    dir = V3DivS(dir, dist);

    // Standard camera looking down -Z at rotation (0, 0, 0):
    // yaw around Y, pitch around X
    cam->rotation.x = asinf(CLAMP(dir.y, -1.0f, 1.0f));
    cam->rotation.y = atan2f(-dir.x, -dir.z);
    cam->rotation.z = 0.0f;

    Mat3 basis = GetRotationM3(cam->rotation);
    cam->up = V3Normalize(M3MulV3(basis, (Vec3){0.0f, 1.0f, 0.0f}));
}

// Helper: Transform a local movement delta into camera-relative world axes if relativeToCamera is enabled
static Vec3 TransformDeltaByCamera(Vec3 localDelta, const Camera *cam, bool relativeToCamera) {
    if (!relativeToCamera || !cam) {
        return localDelta;
    }
    Mat3 camBasis = GetRotationM3(cam->rotation);
    return M3MulV3(camBasis, localDelta);
}
// Helper functions end

//
// Handling controls and creating the GUI
//
void CreateInterface(RenderSettings *settings, Scene *scene) {
    static bool showInterface = true;
    static bool controlActiveModel = true; // true = control active model, false = control active camera
    static bool relativeToCamera = false; // false = World coordinates, true = Relative to active camera

    // Ensure at least one camera exists
    if (arrlen(scene->cameras) == 0) {
        AddCamera(scene, MakeDefaultCamera());
        scene->activeCameraIndex = 0;
    }
    if (scene->activeCameraIndex < 0 || scene->activeCameraIndex >= arrlen(scene->cameras)) {
        scene->activeCameraIndex = 0;
    }

    // Ensure at least one model exists
    if (arrlen(scene->models) == 0) {
        AddModel(scene, MakePrimativeCube());
    }
    if (scene->activeModelIndex < 0 || scene->activeModelIndex >= arrlen(scene->models)) {
        scene->activeModelIndex = 0;
    }

    // File dialog should lock all other UI elements if it's active, so we need to initialize it first
    static GuiWindowFileDialogState fileDialogState = {};
    static FileDialogOperaion fileDialogOperation = -1;

    static Vec4 *colorPickerValue = NULL;
    static int colorPickerX = 0, colorPickerY = 0;

    static int activeBoxId = -1;
    int boxCounter = 0;
    bool activateBox = false;

    int margin = 12;
    int uiLeft = margin, uiTop = margin;
    int uiWidth = 400, uiHeight = settings->height - 2 * margin;
    int iconButtonSize = 24;
    Rectangle uiRect = (Rectangle){uiLeft, uiTop, uiWidth, uiHeight};

    if (fileDialogState.windowActive || colorPickerValue != NULL) {
        GuiLock();
    }
    if (!GuiIsLocked()) { // General keyboard & mouse controls
        if (IsKeyPressed(KEY_TAB)) {
            activateBox = true;
            if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                activeBoxId -= 1;
            } else {
                activeBoxId += 1;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            activeBoxId = -1;
            activateBox = false;
        }

        if (activeBoxId == -1) {
            // Not currently editing a UI box, handle scene related key bindings
            if (IsKeyPressed(KEY_GRAVE)) {
                showInterface = !showInterface;
            }

            Camera *activeCam = (arrlen(scene->cameras) > 0) ? &scene->cameras[scene->activeCameraIndex] : NULL;
            Model *activeMod = (arrlen(scene->models) > 0 && scene->activeModelIndex >= 0 &&
                                scene->activeModelIndex < arrlen(scene->models))
                ? &scene->models[scene->activeModelIndex]
                : NULL;

            Vec3 *targetPos = NULL;
            Vec3 *targetRot = NULL;
            if (controlActiveModel && activeMod != NULL) {
                targetPos = &activeMod->position;
                targetRot = &activeMod->rotation;
            } else if (!controlActiveModel && activeCam != NULL) {
                targetPos = &activeCam->Position;
                targetRot = &activeCam->rotation;
            }

            // Focus shortcut (F key)
            if (IsKeyPressed(KEY_F) && activeCam != NULL && activeMod != NULL) {
                FocusCameraOnPoint(activeCam, activeMod->position);
            }

            if (targetPos != NULL && targetRot != NULL) {
                Vec3 prevModPos = activeMod ? activeMod->position : (Vec3){0};
                Vec3 prevModRot = activeMod ? activeMod->rotation : (Vec3){0};
                Vec3 prevModScale = activeMod ? activeMod->scale : (Vec3){0};

                float dt = GetFrameTime();
                if (dt <= 0.0f) {
                    dt = 1.0f / 60.0f;
                }
                float moveStep = (scene->step > 0.0f ? scene->step : 1.0f) * dt;
                float rotStep = (scene->step > 0.0f ? scene->step : 1.0f) * dt;

                // Keyboard translation: WASD + Space (up) + Shift (down)
                Vec3 moveDelta = {0.0f, 0.0f, 0.0f};
                if (IsKeyDown(KEY_D)) {
                    moveDelta.x += moveStep;
                }
                if (IsKeyDown(KEY_A)) {
                    moveDelta.x -= moveStep;
                }
                if (IsKeyDown(KEY_SPACE)) {
                    moveDelta.y += moveStep;
                }
                if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                    moveDelta.y -= moveStep;
                }
                if (IsKeyDown(KEY_W)) {
                    moveDelta.z -= moveStep;
                }
                if (IsKeyDown(KEY_S)) {
                    moveDelta.z += moveStep;
                }

                if (V3LengthSquared(moveDelta) > EPSILON) {
                    Vec3 worldDelta = TransformDeltaByCamera(moveDelta, activeCam, relativeToCamera);
                    *targetPos = V3Sub(*targetPos, worldDelta);
                    if (!controlActiveModel && activeCam != NULL) {
                        activeCam->target = V3Sub(activeCam->target, worldDelta);
                    }
                }

                // Q = rotate anticlockwise, E = rotate clockwise
                // Left/Right arrows = yaw left/right
                Vec3 rotDelta = {0.0f, 0.0f, 0.0f};
                if (IsKeyDown(KEY_UP)) {
                    rotDelta.x -= rotStep;
                }
                if (IsKeyDown(KEY_DOWN)) {
                    rotDelta.x += rotStep;
                }
                if (IsKeyDown(KEY_LEFT)) {
                    rotDelta.y += rotStep;
                }
                if (IsKeyDown(KEY_RIGHT)) {
                    rotDelta.y -= rotStep;
                }
                if (IsKeyDown(KEY_Q)) {
                    rotDelta.z += rotStep; // Anticlockwise
                }
                if (IsKeyDown(KEY_E)) {
                    rotDelta.z -= rotStep; // Clockwise
                }

                if (V3LengthSquared(rotDelta) > EPSILON) {
                    Vec3 worldRotDelta = TransformDeltaByCamera(rotDelta, activeCam, relativeToCamera);
                    *targetRot = V3Add(*targetRot, worldRotDelta);
                    if (!controlActiveModel && activeCam != NULL) {
                        SyncCameraVectorsFromRotation(activeCam);
                    }
                }

                // Zoom: '+' (KEY_EQUAL / KEY_KP_ADD) and '-' (KEY_MINUS / KEY_KP_SUBTRACT)
                bool zoomIn = IsKeyDown(KEY_EQUAL) || IsKeyDown(KEY_KP_ADD);
                bool zoomOut = IsKeyDown(KEY_MINUS) || IsKeyDown(KEY_KP_SUBTRACT);
                if (zoomIn || zoomOut) {
                    float zoomDir = zoomIn ? 1.0f : -1.0f;
                    float zoomFactor = 1.0f + zoomDir * moveStep;
                    if (zoomFactor < 0.01f) {
                        zoomFactor = 0.01f;
                    }

                    if (controlActiveModel && activeMod != NULL) {
                        activeMod->scale = V3MulS(activeMod->scale, zoomFactor);
                    } else if (!controlActiveModel && activeCam != NULL) {
                        if (activeCam->pType == PTPerspective) {
                            activeCam->fov = CLAMP(activeCam->fov / zoomFactor, 0.05f, 3.0f);
                        } else {
                            activeCam->left /= zoomFactor;
                            activeCam->right /= zoomFactor;
                            activeCam->bottom /= zoomFactor;
                            activeCam->top /= zoomFactor;
                        }
                    }
                }

                // Mouse viewport controls (when cursor is outside the UI panel)
                bool mouseInUi = showInterface && CheckCollisionPointRec(GetMousePosition(), uiRect);
                bool mouseInToggle = CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, 20, 20});
                if (!mouseInUi && !mouseInToggle) {
                    Vector2 mouseDelta = GetMouseDelta();
                    float stepScale = (scene->step > 0.0f ? scene->step : 1.0f);
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)) {
                        Vec3 dragMove = {mouseDelta.x * 0.01f * stepScale, -mouseDelta.y * 0.01f * stepScale, 0.0f};
                        Vec3 worldDelta = TransformDeltaByCamera(dragMove, activeCam, relativeToCamera);
                        *targetPos = V3Add(*targetPos, worldDelta);
                        if (!controlActiveModel && activeCam != NULL) {
                            activeCam->target = V3Add(activeCam->target, worldDelta);
                        }
                    }
                    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)) {
                        Vec3 dragRot = {-mouseDelta.y * 0.01f, -mouseDelta.x * 0.01f, 0.0f};
                        *targetRot = V3Add(*targetRot, TransformDeltaByCamera(dragRot, activeCam, relativeToCamera));
                        if (!controlActiveModel && activeCam != NULL) {
                            SyncCameraVectorsFromRotation(activeCam);
                        }
                    }
                    float wheel = GetMouseWheelMove();
                    if (wheel != 0.0f) {
                        Vec3 wheelMove = {0.0f, 0.0f, -wheel * 0.25f * stepScale};
                        Vec3 worldDelta = TransformDeltaByCamera(wheelMove, activeCam, relativeToCamera);
                        *targetPos = V3Add(*targetPos, worldDelta);
                        if (!controlActiveModel && activeCam != NULL) {
                            activeCam->target = V3Add(activeCam->target, worldDelta);
                        }
                    }
                }

                if (controlActiveModel && activeMod != NULL &&
                    (!V3Eq(prevModPos, activeMod->position) || !V3Eq(prevModRot, activeMod->rotation) ||
                     !V3Eq(prevModScale, activeMod->scale))) {
                    activeMod->modelTransform = CalcModelTransform(activeMod);
                }
            }
        } else {
            GuiLock();
        }
    }

    // See raygui.h, specifically GuiIconName enum to see what icons are available
    const char *label = GuiIconText(showInterface ? ICON_EYE_ON : ICON_EYE_OFF, NULL);
    GuiToggle((Rectangle){0, 0, 20, 20}, label, &showInterface);

    if (!showInterface) {
        return;
    }

    const char *tooltip = NULL;
    {
        Color background = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));
        background.a = 200;
        DrawRectangleRec(uiRect, background);
        GuiGroupBox(uiRect, "Controls");
        uiLeft += margin;
        uiTop += margin;
    }


    //
    // Start the actual GUI
    //
    {
        float left = uiLeft;
        {
            const char *label = GuiIconText(scene->advanceTime ? ICON_PLAYER_PAUSE : ICON_PLAYER_PLAY, NULL);
            Rectangle rect = (Rectangle){left, uiTop, iconButtonSize, iconButtonSize};
            left += iconButtonSize + 2;

            if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                tooltip = "Advance time";
            }

            if (GuiButton(rect, label)) {
                scene->advanceTime = !scene->advanceTime;
            }
        }
        {
            const char *label = NULL;
            switch (settings->renderingMethod) {
                case RENDERING_METHOD_SOFTWARE: {
                    label = GuiIconText(ICON_PENCIL_BIG, NULL);
                } break;
                case RENDERING_METHOD_OPENGL: {
                    label = GuiIconText(ICON_CPU, NULL);
                } break;
                case RENDERING_METHOD_RAYTRACING: {
                    label = GuiIconText(ICON_LASER, NULL);
                } break;
                case RENDERING_METHOD_COUNT: { /* this case is here only to disable a compiler warning */
                } break;
            }
            Rectangle rect = (Rectangle){left, uiTop, iconButtonSize, iconButtonSize};
            left += iconButtonSize + 2;

            if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                switch (settings->renderingMethod) {
                    case RENDERING_METHOD_SOFTWARE: {
                        tooltip = "Switch to OpenGL rendering";
                    } break;
                    case RENDERING_METHOD_OPENGL: {
                        tooltip = "Switch to raytracing";
                    } break;
                    case RENDERING_METHOD_RAYTRACING: {
                        tooltip = "Switch to software rendering";
                    } break;
                    case RENDERING_METHOD_COUNT: { /* this case is here only to disable a compiler warning */
                    } break;
                }
            }

            if (GuiButton(rect, label)) {
                settings->renderingMethod += 1;
                settings->renderingMethod %= RENDERING_METHOD_COUNT;
            }
        }
        {
            Rectangle rect = (Rectangle){left, uiTop, iconButtonSize, iconButtonSize};
            left += iconButtonSize + 2;

            if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                tooltip = "Background tint";
            }

            if (GuiButton(rect, GuiIconText(ICON_COLOR_BUCKET, NULL))) {
                colorPickerValue = &settings->tint;
                colorPickerX = left + 8;
                colorPickerY = uiTop + 8;
            }
            GuiDrawRectangle(rect, 3, ColorFromV4(settings->tint), BLANK);
        }
        {
            Rectangle rect = (Rectangle){left, uiTop, iconButtonSize, iconButtonSize};
            left += iconButtonSize + 2;

            if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                tooltip = settings->showDebugLayer ? "Hide debug layer" : "Show debug layer";
            }

            GuiToggle(rect, GuiIconText(ICON_LAYERS2, NULL), &settings->showDebugLayer);
        }
        {
            Rectangle rect = (Rectangle){left, uiTop, iconButtonSize, iconButtonSize};
            left += iconButtonSize + 2;

            if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                tooltip = relativeToCamera ? "Movement: Relative to Active Camera (Click for World)"
                                           : "Movement: World Coordinates (Click for Camera-Relative)";
            }

            GuiToggle(rect, GuiIconText(relativeToCamera ? ICON_CAMERA : ICON_BOX_GRID, NULL), &relativeToCamera);
        }

        BOX_FLOAT(&scene->step, &left, uiTop + 4, "Set the step size when moving with WASD");
        {
            Rectangle ppRect = (Rectangle){left, uiTop + 4, 14, 14};
            GuiCheckBox(ppRect, "Show Problem Pixel", &settings->showProblem);
            left += 128;
        }
        if (settings->showProblem) {
            BOX_INT(&settings->problemX, &left, uiTop + 4, "Problem Pixel X");
            BOX_INT(&settings->problemY, &left, uiTop + 4, "Problem Pixel Y");
        }
    }
    uiTop += iconButtonSize + margin;

    // ========================================================================
    // MODEL CONTROLS
    // ========================================================================
    static bool showModelControls = true;
    {
        Rectangle rect = (Rectangle){uiLeft - margin / 2, uiTop + margin / 2, margin, margin};
        GuiCheckBox(rect, NULL, &showModelControls);

        GuiLine((Rectangle){uiLeft + margin, uiTop + margin, uiWidth - margin * 2, 0}, "Model Controls");
        uiTop += 2 * margin;
    }

    if (showModelControls) {
        {
            float left = uiLeft + margin;
            int right = uiRect.x + uiRect.width - margin;
            {
                const char *label = GuiIconText(ICON_FOLDER_FILE_OPEN, "Open OBJ");
                Rectangle rect = (Rectangle){left, uiTop, margin + GuiGetTextWidth(label), 2 * margin};
                left += rect.width + 2;

                if (GuiButton(rect, label)) {
                    const char *modelDir = TextFormat("%s" PATH_SEPERATOR "models", GetApplicationDirectory());
                    fileDialogState = InitGuiWindowFileDialog(modelDir);
                    fileDialogState.windowActive = true;
                    strcpy(fileDialogState.filterExt, "obj");

                    fileDialogOperation = FILE_DIALOG_LOAD_MODEL;
                }
            }
            {
                Rectangle rect = (Rectangle){right - iconButtonSize, uiTop, iconButtonSize, iconButtonSize};
                right -= iconButtonSize + 2;

                if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                    tooltip = "Control active model with keyboard/mouse (WASD/QE, Arrows, Drag)";
                }

                GuiToggle(rect, GuiIconText(ICON_PLAYER, NULL), &controlActiveModel);
            }

            // Primitives
            {
                Rectangle rect = (Rectangle){left, uiTop, 2 * margin, 2 * margin};
                left += rect.width + 2;

                if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                    tooltip = "Add Cube";
                }
                if (GuiButton(rect, GuiIconText(ICON_CUBE, NULL))) {
                    Model primitive = MakePrimativeCube();
                    AddModel(scene, primitive);
                    scene->activeModelIndex = arrlen(scene->models) - 1;
                }
            }
            {
                Rectangle rect = (Rectangle){left, uiTop, 2 * margin, 2 * margin};
                left += rect.width + 2;

                if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                    tooltip = "Add Sphere";
                }
                if (GuiButton(rect, GuiIconText(ICON_SPHERE, NULL))) {
                    Model primitive = MakePrimativeSphere();
                    AddModel(scene, primitive);
                    scene->activeModelIndex = arrlen(scene->models) - 1;
                }
            }
        }
        uiTop += 2 * margin + margin / 2;

        { // Transform controls for the active model
            Model dummyModel = {};
            dummyModel.scale = (Vec3){1.0f, 1.0f, 1.0f};
            Model *activeModel = NULL;
            static bool lock = false;

            if (0 < arrlen(scene->models) && 0 <= scene->activeModelIndex &&
                scene->activeModelIndex < arrlen(scene->models)) {
                activeModel = &scene->models[scene->activeModelIndex];
                lock = false;
            } else {
                activeModel = &dummyModel;
                lock = !GuiIsLocked();
            }
            if (lock) {
                GuiLock();
            }

            float rowHeight = 18;
            float controlTop = uiTop;
            Vec3 prevModPos = activeModel->position;
            Vec3 prevModRot = activeModel->rotation;
            Vec3 prevModScale = activeModel->scale;

            { // Position row
                float left = uiLeft + margin;
                const char *label = "position: ";
                Rectangle rect = (Rectangle){left, uiTop, 80, 16};
                GuiDrawText(label, rect, TEXT_ALIGN_LEFT, GetColor(GuiGetStyle(DEFAULT, TEXT)));
                left += rect.width + 2;

                BOX_FLOAT(&activeModel->position.x, &left, uiTop, "Model Position X");
                BOX_FLOAT(&activeModel->position.y, &left, uiTop, "Model Position Y");
                BOX_FLOAT(&activeModel->position.z, &left, uiTop, "Model Position Z");
                uiTop += rowHeight;
            }
            { // Rotation row
                float left = uiLeft + margin;
                const char *label = "rotation: ";
                Rectangle rect = (Rectangle){left, uiTop, 80, 16};
                GuiDrawText(label, rect, TEXT_ALIGN_LEFT, GetColor(GuiGetStyle(DEFAULT, TEXT)));
                left += rect.width + 2;

                BOX_FLOAT(&activeModel->rotation.x, &left, uiTop, "Model Rotation X (rad)");
                BOX_FLOAT(&activeModel->rotation.y, &left, uiTop, "Model Rotation Y (rad)");
                BOX_FLOAT(&activeModel->rotation.z, &left, uiTop, "Model Rotation Z (rad)");
                uiTop += rowHeight;
            }
            { // Scale row
                float left = uiLeft + margin;
                const char *label = "scale: ";
                Rectangle rect = (Rectangle){left, uiTop, 80, 16};
                GuiDrawText(label, rect, TEXT_ALIGN_LEFT, GetColor(GuiGetStyle(DEFAULT, TEXT)));
                left += rect.width + 2;

                BOX_FLOAT(&activeModel->scale.x, &left, uiTop, "Model Scale X");
                BOX_FLOAT(&activeModel->scale.y, &left, uiTop, "Model Scale Y");
                BOX_FLOAT(&activeModel->scale.z, &left, uiTop, "Model Scale Z");
                uiTop += rowHeight;
            }
            if (activeModel != &dummyModel &&
                (!V3Eq(prevModPos, activeModel->position) || !V3Eq(prevModRot, activeModel->rotation) ||
                 !V3Eq(prevModScale, activeModel->scale))) {
                activeModel->modelTransform = CalcModelTransform(activeModel);
            }
            { // Display overlays row (Bounding Box, Vertex Normals, Face Normals)
                float left = uiLeft + margin;
                const char *label = "overlays: ";
                Rectangle rect = (Rectangle){left, uiTop, 80, 16};
                GuiDrawText(label, rect, TEXT_ALIGN_LEFT, GetColor(GuiGetStyle(DEFAULT, TEXT)));
                left += rect.width + 2;

                Rectangle bboxRect = (Rectangle){left, uiTop + 1, 14, 14};
                GuiCheckBox(bboxRect, "BBox", &activeModel->showBoundingBox);
                left += 60;

                Rectangle vNormRect = (Rectangle){left, uiTop + 1, 14, 14};
                GuiCheckBox(vNormRect, "V-Norm", &activeModel->showVertexNormals);
                left += 72;

                Rectangle fNormRect = (Rectangle){left, uiTop + 1, 14, 14};
                GuiCheckBox(fNormRect, "F-Norm", &activeModel->showFaceNormals);
                uiTop += rowHeight;
            }

            if (activeModel == &dummyModel) {
                Rectangle rect =
                    (Rectangle){uiLeft - margin, controlTop - margin / 2, uiWidth, uiTop - controlTop + margin / 2};
                int textAlignStyle = GuiGetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL);

                GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_MIDDLE);

                Color backcolor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_DISABLED));
                backcolor.a = 230;
                GuiDrawRectangle(rect, 2, GetColor(GuiGetStyle(BORDER, BORDER_COLOR_DISABLED)), backcolor);
                GuiDrawText("No model selected", rect, TEXT_ALIGN_CENTER,
                            GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_DISABLED)));

                GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, textAlignStyle);
            }
            uiTop += margin / 2;

            if (lock) {
                GuiUnlock();
            }
        }

        { // Model list controls
            int rowHeight = 2 * margin;
            int itemCount = 4;
            int modelCount = arrlen(scene->models);

            static Rectangle modelPanelView = {0};
            Rectangle scrollRect =
                (Rectangle){uiLeft - 1, uiTop - 1, uiWidth - 2 * margin + 2, itemCount * (rowHeight + 2)};
            Rectangle contentRect = scrollRect;
            contentRect.x += 1;
            contentRect.y += 1;
            int scrollBarWidth = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH) + 2 * GuiGetStyle(DEFAULT, BORDER_WIDTH);
            contentRect.width -= scrollBarWidth + 2;
            contentRect.height = modelCount * (rowHeight + 2);
            static Vector2 panelScroll = {0, 0};
            GuiScrollPanel(scrollRect, NULL, contentRect, &panelScroll, &modelPanelView);
            int scrollTop = contentRect.y + panelScroll.y;

            if (contentRect.height < scrollRect.height) {
                GuiDrawRectangle((Rectangle){scrollRect.x + scrollRect.width - scrollBarWidth, scrollRect.y + 1,
                                             scrollBarWidth - 1, scrollRect.height - 2},
                                 0, WHITE, GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL)));
            }

            uiTop += scrollRect.height + 4;
            bool lock = !CheckCollisionPointRec(GetMousePosition(), scrollRect) && !GuiIsLocked();
            if (lock) {
                GuiLock();
            }

            BeginScissorMode(modelPanelView.x, modelPanelView.y, modelPanelView.width, modelPanelView.height);

            for (int modelIndex = 0; modelIndex < modelCount; ++modelIndex) {
                Model *model = &scene->models[modelIndex];

                int left = contentRect.x;
                int right = contentRect.x + contentRect.width;

                {
                    Rectangle rect = (Rectangle){left, scrollTop, 2 * margin, 2 * margin};
                    left += rect.width + 2;
                    if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                        tooltip = "Hide/Show Model";
                    }

                    GuiToggle(rect, GuiIconText(model->hidden ? ICON_EYE_OFF : ICON_EYE_ON, NULL), &model->hidden);
                }
                {
                    Rectangle rect = (Rectangle){right - 2 * margin, scrollTop, 2 * margin, 2 * margin};
                    right -= rect.width + 2;

                    if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                        tooltip = "Delete Model";
                    }

                    if (GuiButton(rect, GuiIconText(ICON_BIN, NULL))) {
                        RemoveModel(scene, modelIndex);
                        if (scene->activeModelIndex >= modelIndex) {
                            scene->activeModelIndex -= 1;
                        }
                        modelCount = arrlen(scene->models);
                        break;
                    }
                }
                {
                    Rectangle rect = (Rectangle){right - 2 * margin, scrollTop, 2 * margin, 2 * margin};
                    right -= rect.width + 2;

                    if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                        tooltip = "Focus Active Camera on Model";
                    }

                    if (GuiButton(rect, GuiIconText(ICON_TARGET, NULL))) {
                        if (arrlen(scene->cameras) > 0) {
                            FocusCameraOnPoint(&scene->cameras[scene->activeCameraIndex], model->position);
                        }
                    }
                }
                {
                    Rectangle rect = (Rectangle){right - 2 * margin, scrollTop, 2 * margin, 2 * margin};
                    right -= rect.width + 2;

                    if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                        tooltip = "Toggle Bounding Box";
                    }

                    GuiToggle(rect, GuiIconText(ICON_BOX_CORNERS_SMALL, NULL), &model->showBoundingBox);
                }

                {
                    const char *label = TextFormat(" Model %3d", modelIndex);
                    Rectangle rect = (Rectangle){left, scrollTop, right - left, 2 * margin};

                    bool hovered = false;
                    if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            if (scene->activeModelIndex != modelIndex) {
                                scene->activeModelIndex = modelIndex;
                            } else {
                                scene->activeModelIndex = -1;
                            }
                        }

                        hovered = true;
                    }

                    bool active = modelIndex == scene->activeModelIndex;

                    Color backColor = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
                    Color textColor = GetColor(GuiGetStyle(DEFAULT, TEXT));
                    int border = 0;
                    if (active) {
                        backColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
                        textColor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_PRESSED));
                    } else if (hovered) {
                        backColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED));
                        textColor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_FOCUSED));
                    }
                    if (hovered) {
                        border = 2;
                    }

                    GuiDrawRectangle(rect, border, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED)), backColor);
                    GuiDrawText(label, rect, TEXT_ALIGN_LEFT, textColor);
                }

                scrollTop += rowHeight + 2;
            }
            EndScissorMode();

            if (lock) {
                GuiUnlock();
            }
        }
    }

    // ========================================================================
    // CAMERA CONTROLS
    // ========================================================================
    static bool showCameraControls = true;
    {
        Rectangle rect = (Rectangle){uiLeft - margin / 2, uiTop + margin / 2, margin, margin};
        GuiCheckBox(rect, NULL, &showCameraControls);

        GuiLine((Rectangle){uiLeft + margin, uiTop + margin, uiWidth - margin * 2, 0}, "Camera Controls");
        uiTop += 2 * margin;
    }

    if (showCameraControls) {
        {
            float left = uiLeft + margin;
            int right = uiRect.x + uiRect.width - margin;
            {
                const char *label = GuiIconText(ICON_CAMERA, "Add Camera");
                Rectangle rect = (Rectangle){left, uiTop, margin + GuiGetTextWidth(label), 2 * margin};
                left += rect.width + 2;

                if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                    tooltip = "Add a new camera to the scene";
                }
                if (GuiButton(rect, label)) {
                    AddCamera(scene, MakeDefaultCamera());
                    scene->activeCameraIndex = arrlen(scene->cameras) - 1;
                }
            }
            {
                const char *label = GuiIconText(ICON_TARGET, "Focus Model");
                Rectangle rect = (Rectangle){left, uiTop, margin + GuiGetTextWidth(label), 2 * margin};
                left += rect.width + 2;

                if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                    tooltip = "Focus active camera on active model (Shortcut: F)";
                }
                if (GuiButton(rect, label)) {
                    if (arrlen(scene->cameras) > 0 && arrlen(scene->models) > 0 && scene->activeModelIndex >= 0 &&
                        scene->activeModelIndex < arrlen(scene->models)) {
                        FocusCameraOnPoint(&scene->cameras[scene->activeCameraIndex],
                                           scene->models[scene->activeModelIndex].position);
                    }
                }
            }
            {
                Rectangle rect = (Rectangle){right - iconButtonSize, uiTop, iconButtonSize, iconButtonSize};
                right -= iconButtonSize + 2;

                if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                    tooltip = "Control active camera with keyboard/mouse (WASD/QE, Arrows, Drag)";
                }

                bool controlCamera = !controlActiveModel;
                GuiToggle(rect, GuiIconText(ICON_PLAYER, NULL), &controlCamera);
                controlActiveModel = !controlCamera;
            }
        }
        uiTop += 2 * margin + margin / 2;

        { // Transform & projection controls for the active camera
            Camera dummyCam = MakeDefaultCamera();
            Camera *activeCamera = &dummyCam;
            if (arrlen(scene->cameras) > 0 && scene->activeCameraIndex >= 0 &&
                scene->activeCameraIndex < arrlen(scene->cameras)) {
                activeCamera = &scene->cameras[scene->activeCameraIndex];
            }

            float rowHeight = 18;
            { // Position row
                float left = uiLeft + margin;
                Rectangle rect = (Rectangle){left, uiTop, 80, 16};
                GuiDrawText("position: ", rect, TEXT_ALIGN_LEFT, GetColor(GuiGetStyle(DEFAULT, TEXT)));
                left += rect.width + 2;

                BOX_FLOAT(&activeCamera->Position.x, &left, uiTop, "Camera Position X");
                BOX_FLOAT(&activeCamera->Position.y, &left, uiTop, "Camera Position Y");
                BOX_FLOAT(&activeCamera->Position.z, &left, uiTop, "Camera Position Z");
                uiTop += rowHeight;
            }
            { // Rotation row
                float left = uiLeft + margin;
                Rectangle rect = (Rectangle){left, uiTop, 80, 16};
                GuiDrawText("rotation: ", rect, TEXT_ALIGN_LEFT, GetColor(GuiGetStyle(DEFAULT, TEXT)));
                left += rect.width + 2;

                Vec3 prevRot = activeCamera->rotation;
                BOX_FLOAT(&activeCamera->rotation.x, &left, uiTop, "Camera Pitch / Rotation X (rad)");
                BOX_FLOAT(&activeCamera->rotation.y, &left, uiTop, "Camera Yaw / Rotation Y (rad)");
                BOX_FLOAT(&activeCamera->rotation.z, &left, uiTop, "Camera Roll / Rotation Z (rad)");
                if (!V3Eq(prevRot, activeCamera->rotation)) {
                    SyncCameraVectorsFromRotation(activeCamera);
                }
                uiTop += rowHeight;
            }
            { // Target row
                float left = uiLeft + margin;
                Rectangle rect = (Rectangle){left, uiTop, 80, 16};
                GuiDrawText("target: ", rect, TEXT_ALIGN_LEFT, GetColor(GuiGetStyle(DEFAULT, TEXT)));
                left += rect.width + 2;

                BOX_FLOAT(&activeCamera->target.x, &left, uiTop, "Camera Target X");
                BOX_FLOAT(&activeCamera->target.y, &left, uiTop, "Camera Target Y");
                BOX_FLOAT(&activeCamera->target.z, &left, uiTop, "Camera Target Z");
                uiTop += rowHeight;
            }
            { // Up vector row
                float left = uiLeft + margin;
                Rectangle rect = (Rectangle){left, uiTop, 80, 16};
                GuiDrawText("up: ", rect, TEXT_ALIGN_LEFT, GetColor(GuiGetStyle(DEFAULT, TEXT)));
                left += rect.width + 2;

                BOX_FLOAT(&activeCamera->up.x, &left, uiTop, "Camera Up Vector X");
                BOX_FLOAT(&activeCamera->up.y, &left, uiTop, "Camera Up Vector Y");
                BOX_FLOAT(&activeCamera->up.z, &left, uiTop, "Camera Up Vector Z");
                uiTop += rowHeight;
            }
            { // Projection type & zNear / zFar row
                float left = uiLeft + margin;
                Rectangle rect = (Rectangle){left, uiTop, 80, 16};
                GuiDrawText("proj / z: ", rect, TEXT_ALIGN_LEFT, GetColor(GuiGetStyle(DEFAULT, TEXT)));
                left += rect.width + 2;

                const char *projLabel = (activeCamera->pType == PTPerspective) ? "Perspective" : "Orthographic";
                Rectangle projRect = (Rectangle){left, uiTop, 84, 16};
                left += projRect.width + 4;
                if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), projRect)) {
                    tooltip = "Click to toggle Perspective / Orthographic projection";
                }
                if (GuiButton(projRect, projLabel)) {
                    activeCamera->pType = (activeCamera->pType == PTPerspective) ? PTOrthographic : PTPerspective;
                }

                BOX_FLOAT_CLAMP0(&activeCamera->zNear, &left, uiTop, "Near clipping plane (zNear)");
                BOX_FLOAT_CLAMP0(&activeCamera->zFar, &left, uiTop, "Far clipping plane (zFar)");
                uiTop += rowHeight;
            }
            { // Projection parameters row (FoV for Perspective, Left/Right/Bottom/Top for Orthographic)
                float left = uiLeft + margin;
                if (activeCamera->pType == PTPerspective) {
                    Rectangle rect = (Rectangle){left, uiTop, 80, 16};
                    GuiDrawText("fov (rad): ", rect, TEXT_ALIGN_LEFT, GetColor(GuiGetStyle(DEFAULT, TEXT)));
                    left += rect.width + 2;

                    BOX_FLOAT_CLAMP0(&activeCamera->fov, &left, uiTop, "Field of View in radians");
                } else {
                    Rectangle rect = (Rectangle){left, uiTop, 80, 16};
                    GuiDrawText("L/R/B/T: ", rect, TEXT_ALIGN_LEFT, GetColor(GuiGetStyle(DEFAULT, TEXT)));
                    left += rect.width + 2;

                    BOX_FLOAT(&activeCamera->left, &left, uiTop, "Ortho Left");
                    BOX_FLOAT(&activeCamera->right, &left, uiTop, "Ortho Right");
                    BOX_FLOAT(&activeCamera->bottom, &left, uiTop, "Ortho Bottom");
                    BOX_FLOAT(&activeCamera->top, &left, uiTop, "Ortho Top");
                }
                uiTop += rowHeight + margin / 2;
            }
        }

        { // Camera list controls
            int rowHeight = 2 * margin;
            int itemCount = 3;
            int cameraCount = arrlen(scene->cameras);

            static Rectangle camPanelView = {0};
            Rectangle scrollRect =
                (Rectangle){uiLeft - 1, uiTop - 1, uiWidth - 2 * margin + 2, itemCount * (rowHeight + 2)};
            Rectangle contentRect = scrollRect;
            contentRect.x += 1;
            contentRect.y += 1;
            int scrollBarWidth = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH) + 2 * GuiGetStyle(DEFAULT, BORDER_WIDTH);
            contentRect.width -= scrollBarWidth + 2;
            contentRect.height = cameraCount * (rowHeight + 2);
            static Vector2 camPanelScroll = {0, 0};
            GuiScrollPanel(scrollRect, NULL, contentRect, &camPanelScroll, &camPanelView);
            int scrollTop = contentRect.y + camPanelScroll.y;

            if (contentRect.height < scrollRect.height) {
                GuiDrawRectangle((Rectangle){scrollRect.x + scrollRect.width - scrollBarWidth, scrollRect.y + 1,
                                             scrollBarWidth - 1, scrollRect.height - 2},
                                 0, WHITE, GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL)));
            }

            uiTop += scrollRect.height + 2;
            bool lock = !CheckCollisionPointRec(GetMousePosition(), scrollRect) && !GuiIsLocked();
            if (lock) {
                GuiLock();
            }

            BeginScissorMode(camPanelView.x, camPanelView.y, camPanelView.width, camPanelView.height);

            for (int camIndex = 0; camIndex < cameraCount; ++camIndex) {
                int left = contentRect.x;
                int right = contentRect.x + contentRect.width;

                {
                    Rectangle rect = (Rectangle){right - 2 * margin, scrollTop, 2 * margin, 2 * margin};
                    right -= rect.width + 2;

                    if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                        tooltip = (cameraCount > 1) ? "Delete Camera" : "Cannot delete the only camera";
                    }

                    if (GuiButton(rect, GuiIconText(ICON_BIN, NULL)) && cameraCount > 1) {
                        RemoveCamera(scene, camIndex);
                        if (scene->activeCameraIndex >= arrlen(scene->cameras)) {
                            scene->activeCameraIndex = arrlen(scene->cameras) - 1;
                        }
                        cameraCount = arrlen(scene->cameras);
                        break;
                    }
                }

                {
                    const char *projTag = (scene->cameras[camIndex].pType == PTPerspective) ? "Persp" : "Ortho";
                    const char *label = TextFormat(" Camera %2d (%s)", camIndex, projTag);
                    Rectangle rect = (Rectangle){left, scrollTop, right - left, 2 * margin};

                    bool hovered = false;
                    if (!GuiIsLocked() && CheckCollisionPointRec(GetMousePosition(), rect)) {
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            scene->activeCameraIndex = camIndex;
                        }
                        hovered = true;
                    }

                    bool active = (camIndex == scene->activeCameraIndex);

                    Color backColor = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
                    Color textColor = GetColor(GuiGetStyle(DEFAULT, TEXT));
                    int border = 0;
                    if (active) {
                        backColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
                        textColor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_PRESSED));
                    } else if (hovered) {
                        backColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED));
                        textColor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_FOCUSED));
                    }
                    if (hovered) {
                        border = 2;
                    }

                    GuiDrawRectangle(rect, border, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED)), backColor);
                    GuiDrawText(label, rect, TEXT_ALIGN_LEFT, textColor);
                }

                scrollTop += rowHeight + 2;
            }
            EndScissorMode();

            if (lock) {
                GuiUnlock();
            }
        }
    }

    // Global GUI elements that are drawn after all the other stuff (and maybe on top of it).
    // Those elements should also be always responsive, so we always unlock before drawing it.
    GuiUnlock();
    if (tooltip != NULL) {
        Vector2 mouse = GetMousePosition();
        Tooltip(tooltip, mouse.x, mouse.y);
    }

    if (colorPickerValue != NULL) {
        int padding = GuiGetStyle(COLORPICKER, HUEBAR_PADDING) + GuiGetStyle(COLORPICKER, HUEBAR_WIDTH);
        int size = 100;
        int x = MIN(MAX(0, colorPickerX - size / 2), settings->width - size - padding);
        int y = MIN(MAX(0, colorPickerY - size / 2), settings->height - size - padding);
        Rectangle rect = (Rectangle){x, y, size, size};
        Rectangle bounds = (Rectangle){x, y, size + padding, size + padding};
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), bounds)) {
            colorPickerValue = NULL;
        } else {
            GuiDrawRectangle(bounds, 1, GetColor(GuiGetStyle(BORDER, BORDER_COLOR_NORMAL)),
                             GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL)));
            static float hue = 0.f;
            Vector3 hsv = ConvertRGBtoHSV((Vector3){colorPickerValue->r, colorPickerValue->g, colorPickerValue->b});
            GuiColorPanelHSV(rect, NULL, &hsv);
            if (hsv.y > 0.f && hsv.z > 0.f) {
                hue = hsv.y;
            } else {
                hsv.y = hue;
            }

            Rectangle rectHue = {(float)rect.x + rect.width + GuiGetStyle(COLORPICKER, HUEBAR_PADDING), (float)rect.y,
                                 (float)GuiGetStyle(COLORPICKER, HUEBAR_WIDTH), (float)rect.height};
            Rectangle rectAlpha = {rect.x, rect.y + rect.height + GuiGetStyle(COLORPICKER, HUEBAR_PADDING), rect.width,
                                   GuiGetStyle(COLORPICKER, HUEBAR_WIDTH)};

            GuiColorBarHue(rectHue, NULL, &hsv.x);
            GuiColorBarAlpha(rectAlpha, NULL, &colorPickerValue->a);
            Vector3 rgb = ConvertHSVtoRGB(hsv);
            colorPickerValue->rgb = (Vec3){rgb.x, rgb.y, rgb.z};
        }
    }

    {
        if (IsKeyPressed(KEY_ESCAPE)) {
            fileDialogState.windowActive = false;
        }
        GuiWindowFileDialog(&fileDialogState);

        if (fileDialogState.SelectFilePressed) {
            switch (fileDialogOperation) {
                case FILE_DIALOG_LOAD_MODEL: {
                    const char *fullPath =
                        TextFormat("%s" PATH_SEPERATOR "%s", fileDialogState.dirPathText, fileDialogState.fileNameText);
                    TraceLog(LOG_INFO, "Selected file: %s", fullPath);
                    if (FileExists(fullPath)) {
                        AddModel(scene, LoadModelFromObjFile(fullPath));
                        scene->activeModelIndex = arrlen(scene->models) - 1;
                    }
                } break;
            }
            fileDialogState.SelectFilePressed = false;
        }
    }
}

#endif // INTERFACE_IMPLEMENTED
#endif // INTERFACE_IMPLEMENTATION
