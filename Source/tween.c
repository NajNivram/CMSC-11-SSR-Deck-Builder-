#include "tween.h" // cowabummer!
#include <math.h>
#include <stdbool.h>

#undef EndDrawing

#define MAX_TWEENS 64

typedef struct Tween
{
    float *value;
    float start;
    float end;
    float duration;
    EaseCurve curve;
    EaseDirection direction;
    float elapsed;
    bool active;
} Tween;

static Tween tweens[MAX_TWEENS];

float ease_linear(float t) { return t; }
float ease_quad(float t) { return t * t; }
float ease_cubic(float t) { return t * t * t; }
float ease_quart(float t) { return t * t * t * t; }
float ease_quint(float t) { return t * t * t * t * t; }
float ease_expo(float t)
{
    if (t < 0.0f)
        t = 0.0f;
    if (t > 1.0f)
        t = 1.0f;
    return (t == 0.0f) ? 0.0f : powf(2.0f, 10.0f * (t - 1.0f));
}
float ease_circ(float t)
{
    if (t < 0.0f)
        t = 0.0f;
    if (t > 1.0f)
        t = 1.0f;
    return 1.0f - sqrtf(1.0f - t * t);
}

static float ease(float t, EaseCurve f, EaseDirection direction)
{
    switch (direction)
    {
    case EASE_IN:
        return f(t);
    case EASE_OUT:
        return 1.0f - f(1.0f - t);
    case EASE_IN_OUT:
        return (t < 0.5f) ? f(t * 2.0f) * 0.5f : 1.0f - f((1.0f - t) * 2.0f) * 0.5f;
    default:
        return t; // fallback to linear
    }
}

int tween(float *value, float end, float duration, EaseCurve curve, EaseDirection direction)
{
    for (int i = 0; i < MAX_TWEENS; i++)
    {
        if (tweens[i].active)
            continue;

        tweens[i] = (Tween){
            .value = value,
            .start = *value,
            .end = end,
            .duration = duration,
            .curve = curve,
            .direction = direction,
            .elapsed = 0.0f,
            .active = true};
        return i; // return tween index
    }
    return -1; // ITS FULL
}

void tween_update(float delta)
{
    for (int i = 0; i < MAX_TWEENS; i++)
    {
        Tween *tween = &tweens[i];
        if (!tween->active)
            continue;

        if (tween->duration == 0.0f)
        {
            tween_complete(i);
            continue;
        }

        tween->elapsed += delta;
        float t = tween->elapsed / tween->duration;

        if (t >= 1.0f)
        {
            tween_complete(i);
            continue;
        }

        float eased_t = ease(t, tween->curve, tween->direction);
        *tween->value = tween->start + (tween->end - tween->start) * eased_t;
    }
}

void tween_complete(int tween_id)
{
    if (tween_id < 0 || tween_id >= MAX_TWEENS)
        return;

    Tween *tween = &tweens[tween_id];
    if (!tween->active)
        return;

    *tween->value = tween->end;
    tween->active = false;
}

void tween_cancel(int tween_id)
{
    if (tween_id < 0 || tween_id >= MAX_TWEENS)
        return;
    tweens[tween_id].active = false;
}

void tween_clear()
{
    for (int i = 0; i < MAX_TWEENS; i++)
    {
        tweens[i].active = false;
    }
}
