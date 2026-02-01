#include "Tween.h"
#include <math.h>

std::array<Tween::Tweener, 512> Tween::tweeners;
float Tween::time = 0.0f;

float Tween::linear(float start, float goal, float t) {
    return (1.0f - t) * start + t * goal;
}

float Tween::easeOutBack(float start, float goal, float t) {
    // from https://easings.net/#easeOutBack
    float c1 = 1.70158;
    float c3 = c1 + 1.0f;

    return start + (1 + c3 * pow(t - 1, 3) + c1 * pow(t - 1, 2)) * (goal - start);
}

float Tween::EaseOutExpo(float start, float goal, float t) {
    return start + (t == 1 ? 1 : 1 - pow(2, -10 * t)) * (goal - start);
}

void Tween::update(float dt) {
    for (Tweener& tweener : Tween::tweeners) {
        if (tweener.value == nullptr) {
            continue;
        }

        float t = tweener.elapsed / tweener.time;

        switch (tweener.func) {
        case Function::Linear:
            *tweener.value = Tween::linear(tweener.start, tweener.goal, t);
            break;

        case Function::EaseOutBack:
            *tweener.value = Tween::easeOutBack(tweener.start, tweener.goal, t);
            break;

        case Function::EaseOutExpo:
            *tweener.value = Tween::easeOutBack(tweener.start, tweener.goal, t);
            break;
        }

        tweener.elapsed += dt;

        if (t >= 1.0f) {
            tweener.isOver = true;
        }
    }
}

Tween::Tweener* Tween::tween(float* value, float goal, float time, Function func) {
    for (Tweener& tweener : Tween::tweeners) {
        if (tweener.value == nullptr) {
            tweener.value = value;
            tweener.start = *value;
            tweener.goal = goal;
            tweener.time = time;
            tweener.elapsed = 0.0f;
            tweener.func = func;
            tweener.isOver = false;
            return &tweener;
        }
    }

    return nullptr;
}
