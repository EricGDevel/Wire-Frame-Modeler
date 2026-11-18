#if !defined(RENDERER_HEADER)
#define RENDERER_HEADER

#include "scene.h"
#include "math.h"

typedef enum {
    RENDERING_METHOD_SOFTWARE   = 0, // HW1-2
    RENDERING_METHOD_OPENGL     = 1, // HW3
    RENDERING_METHOD_RAYTRACING = 2, // HW4
    
    RENDERING_METHOD_COUNT,
} RenderingMethod;

// RenderSettings is pointed by the renderer and not stored on it directly,
// so we can share it between different renderers.
typedef struct RenderSettings {
    int width, height;
    
    bool showDebugLayer;
    Vec4 tint;
    int problemX, problemY;
    
    RenderingMethod renderingMethod;
} RenderSettings;

typedef struct Renderer {
    RenderSettings *settings;
    Vec4 *pixels;
} Renderer;

void RenderScene(Scene *scene, Renderer *renderer, double time);

#endif // RENDERER_HEADER


// Renderer Implementation
// TODO: Change ifdef back
#if 1 //defined(RENDERER_IMPLEMENTATION)
#if !defined(RENDERER_IMPLEMENTED)
#define RENDERER_IMPLEMENTED

#include <raylib.h>

#include "stb_ds.h" // For dynamic arrays

void plotPixel(Renderer* renderer, const int x, const int y){
    if(0 <= x < renderer->settings->width && 0<= y < renderer->settings->height){
    renderer->pixels[y * renderer->settings->width + x] = renderer->settings->tint;}
}

void bresenhamHighSlope(Renderer *renderer, const Vec2i p1, const Vec2i p2){
    int x = p1.x;
    int y = p1.y;
    int dir_x = p2.x >= p1.x ? 1 : -1;
    int dir_y = p2.y >= p1.y ? 1 : -1;
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    dx = dx * dir_x;
    int d = 2 * dx - dy;
    int de = 2 * dx;
    int dne = 2 * (dx - dy);
    plotPixel(renderer, x, y);
    for (int y_curr = p1.y; y_curr != p2.y; y_curr += dir_y) {
        if(d < 0){
            d += de;
        }
        else{
            x += dir_x;//advance x
            d += dne;
        }
        plotPixel(renderer, x, y_curr);
    }
}

void bresenhamLowSlope(Renderer *renderer, const Vec2i p1, const Vec2i p2){
    int y = p1.y;
    int x = p1.x;
    int dir_x = p2.x >= p1.x ? 1 : -1;
    int dir_y = p2.y >= p1.y ? 1 : -1;
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    dy = dy * dir_y;
    int d = 2 * dy - dx;
    int de = 2 * dy;
    int dne = 2 * (dy - dx);
    plotPixel(renderer, x, y);
    for(int x_curr = p1.x; x_curr != p2.x; x_curr += dir_x){
        if(d < 0){
            d += de;
        }
        else{
            y+=dir_y;//advance y
            d += dne;
        }
        plotPixel(renderer, x_curr, y);
    }
}

void bresenham(Renderer *renderer, const Vec2i p1, const Vec2i p2) {
    //bresenham works in 2d coordinates
    // This is just from the ppt, might need to expand cases
    if(ABS(p2.y - p1.y) > ABS(p2.x - p1.x)){
        bresenhamHighSlope(renderer, p1, p2);
    } else{
        bresenhamLowSlope(renderer, p1, p2);
    }
}

void RenderScene(Scene *scene, Renderer *renderer, double time) {
    // Note that 'TextFormat' uses global storage for the strings it returns, so don't save pointers
    // to those directly if you need to store them long-term, copy the result into a persistant buffer.
    DrawText(TextFormat("Drawing scene with %d models", 0), 400, 100, 10, BLACK);//TODO: revert %d=0 to be model amount
    
    RenderSettings *settings = renderer->settings;
    
    // Set background color before drawing model wireframes
    for (int y = 0; y < settings->height; ++y) {
        for (int x = 0; x < settings->width; ++x) {
            Vec4 *pixel = &renderer->pixels[y*settings->width + x];
            pixel->r = (((float)x / (float)(renderer->settings->width))  + renderer->settings->tint.r) / 2.f;
            pixel->g = (((float)y / (float)(renderer->settings->height)) + renderer->settings->tint.g) / 2.f;
            pixel->b = (((sin(time) + 1.0)/2.0) + renderer->settings->tint.b) / 2.f;
            pixel->a = 255;
        }
    }

    //int modelLength = arrlen(scene->models);
    // TODO: Remove loop and use scene->activeModelIndex;
    /*for (int modelIndex = 0; modelIndex < modelLength; modelIndex++) {
        Model *m = &scene->models[modelIndex];
        for (int faceIndex = 0; faceIndex < m->faceCount; faceIndex++) {
            TriangleFace *f = &m->faces[faceIndex];
            int aIndex = f->a.v, bIndex = f->b.v, cIndex = f->c.v;
            if (aIndex == 0 || bIndex == 0 || cIndex == 0) {
                // TODO: Print warning
                continue;
            }
            if (aIndex > m->vertexCount || bIndex > m->vertexCount || cIndex == m->vertexCount) {
                // TODO: Print warning
                continue;
            }

            // Indexes start from 1 according to obj spec
            Vec3 *a = &m->vertices[aIndex - 1], *b = &m->vertices[bIndex - 1], *c = &m->vertices[cIndex - 1];
            
            // Draw wireframe lines
            //bresenham(renderer, a, b);
            //bresenham(renderer, b, c);
            //bresenham(renderer, a, c);
            
        }
    }*/
    bresenham(renderer, (Vec2i){-1, 759}, (Vec2i){1000,250});
}


// static void debugSkeleton() {
//     // You can use raylib to draw shapes to help debug some things, what you draw using raylib's functions
//     // will end up on the debug layer and will be overlayed above the actual scene (if enabled).
//     // Note that you're expected to implement everything yourself, those functions are only here for helping
//     // you debug your own code!
//     for (int modelIndex = 0; modelIndex < arrlen(scene->models); ++modelIndex) {
//         Model model = scene->models[modelIndex];
        
//         for (int i = 0; i < arrlen(model.vertices); i += 3) {
//             // Vector2 v1 = { (model.vertices[i + 0].x + 1.f) * settings->width / 2, (model.vertices[i + 0].y + 1.f) * settings->width / 2 };
//             // Vector2 v2 = { (model.vertices[i + 1].x + 1.f) * settings->width / 2, (model.vertices[i + 1].y + 1.f) * settings->width / 2 };
//             // Vector2 v3 = { (model.vertices[i + 2].x + 1.f) * settings->width / 2, (model.vertices[i + 2].y + 1.f) * settings->width / 2 };
//             // DrawTriangle(v1, v2, v3, WHITE);
//         }
//     }
    
//     // Here's where the real stuff happens: we go over every pixel and initialize it with some random values,
//     // but soon you'll render geometry here instead :)
    
//     // A little advice, I recommend using the naming convensions for projection matrices from Anton Mikhailov,
//     // i.e. fromXtoY (fromModelToCamera, fromCameraToScreen, etc.),
//     // that'll save you a lot of time. Reference: https://youtu.be/QV5OwOCtHQE?t=2750
    
//     // TL;DR: Debug loop
//     for (int y = 0; y < settings->height; ++y) {
//         for (int x = 0; x < settings->width; ++x) {
//             Vec4 *pixel = &renderer->pixels[y*settings->width + x];
            
//             if (x == settings->problemX && y == settings->problemY) {
//                 *pixel = (Vec4){0};
//                 // An example of drawing a circle and some text with raylib over your scene
//                 DrawCircleLines(x, y, 2, RED);
//                 DrawText("^problem", x, y + 2, 8, RED);
//             } else {
//                 pixel->r = (((float)x / (float)(renderer->settings->width))  + renderer->settings->tint.r) / 2.f;
//                 pixel->g = (((float)y / (float)(renderer->settings->height)) + renderer->settings->tint.g) / 2.f;
//                 pixel->b = (((sin(time) + 1.0)/2.0) + renderer->settings->tint.b) / 2.f;
//                 pixel->a = 255;
//             }
//         }
//     }
// }


#endif // RENDERER_IMPLEMENTED
#endif // RENDERER_IMPLEMENTATION
