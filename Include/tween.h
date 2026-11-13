#ifndef TWEEN_H
#define TwEEN_H

#include "raylib.h"

typedef float (*EaseCurve)(float t);

typedef enum
{
    EASE_IN,
    EASE_OUT,
    EASE_IN_OUT
} EaseDirection;

float ease_linear(float t);
float ease_quad(float t);
float ease_cubic(float t);
float ease_quart(float t);
float ease_quint(float t);
float ease_expo(float t);
float ease_circ(float t);

int tween(float *value, float end, float duration, EaseCurve curve, EaseDirection direction);
void tween_update(float delta);
void tween_complete(int tween_id);
void tween_cancel(int tween_id);
void tween_clear();

#define EndDrawing()                  \
    do                                \
    {                                 \
        tween_update(GetFrameTime()); \
        EndDrawing();                 \
    } while (0)

#endif
