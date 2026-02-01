#pragma once
#include <vector>
#include <functional>
#include <array>

class Tween {
public:
    enum class Function {
        Linear,
        EaseOutBack,
        EaseOutExpo,
    };

    struct Tweener {
        Tweener() : value(nullptr) {}
        Tweener(float* value, float goal, float time, Function func)
            : value(value), start(*value), goal(goal), time(time), func(func)
        {}

        Tweener operator=(const Tweener& other) {
            this->value = other.value;
            this->start = other.start;
            this->goal = other.goal;
            this->time = other.time;
            this->elapsed = other.elapsed;
            this->func = other.func;
            return *this;
        }

        float* value;
        float start;
        float goal;
        float time;
        float elapsed = 0.0f;
        Function func;
        bool isOver = false;
    };

    static float linear(float start, float goal, float t);
    static float easeOutBack(float start, float goal, float t);
    static float EaseOutExpo(float start, float goal, float t);

    static void update(float dt);

    static Tweener* tween(float* value, float goal, float time, Function func);

    static std::array<Tweener, 512> tweeners; // i sure hope 512 is enough lol
    static int numTweens;
    static float time;
};
