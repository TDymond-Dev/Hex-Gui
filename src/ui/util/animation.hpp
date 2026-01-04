#ifndef UI_ANIMATE
#define UI_ANIMATE

#include "misc.hpp"

#include <functional>
#include <memory>
#include <cmath>

namespace ui {
    struct Animation {
        void update(float dt) {
            if (!started) {
                m_start = dt;
                started = true;
            }

            onUpdate(dt);
        }

        virtual void onUpdate(float dt) = 0;

        Animation(float duration) : m_duration(duration) {}

        bool started = false;
        float m_duration;
        float m_start = 0.0;
        std::function<void(float)> m_f;

        bool finished(float dt) {
            if (dt - m_start >= m_duration) {
                return true;
            }
            return false;
        }
    };

    struct LinearAnimation : public Animation {
        LinearAnimation(float duration) : Animation(duration) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            m_f(t);
        }

    };

    struct StepAnimation : public Animation {
        float step;
        StepAnimation(float duration, float step) : Animation(duration), step(step) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            t = std::round(t / step) * step; // This will round 't' to the nearest 'step'
            m_f(t);
        }
    };

    struct SmoothStepAnimation : public Animation {
        SmoothStepAnimation(float duration) : Animation(duration) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            t = t * t * (3 - 2 * t); // Smoothstep function
            m_f(t);
        }
    };

    struct EaseInAnimation : public Animation {
        EaseInAnimation(float duration) : Animation(duration) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            t = t * t; // Ease-in function
            m_f(t);
        }
    };

    struct EaseOutAnimation : public Animation {
        EaseOutAnimation(float duration) : Animation(duration) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            t = t * (2 - t); // Ease-out function
            m_f(t);
        }
    };

    struct CurveAnimation : public Animation {
        CurveAnimation(float duration) : Animation(duration) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            t = std::sin(t * (PI * 2)); // Curve function
            m_f(t);
        }
    };

    struct BounceAnimation : public Animation {
        BounceAnimation(float duration) : Animation(duration) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            t = std::abs(std::sin(6 * t * PI) * (1 - t)); // Bounce function
            m_f(t);
        }
    };

    struct ElasticAnimation : public Animation {
        ElasticAnimation(float duration) : Animation(duration) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            t = t == 0 ? 0 : t == 1 ? 1 : std::pow(2, -10 * t) * std::sin((t - 0.075) * (2 * PI) / 0.3) + 1;
            m_f(t);
        }
    };

    struct BackAnimation : public Animation {
        BackAnimation(float duration) : Animation(duration) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            float s = 1.70158;
            t = t * t * ((s + 1) * t - s);
            m_f(t);
        }
    };

    struct QuadraticAnimation : public Animation {
        QuadraticAnimation(float duration) : Animation(duration) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            t = t * t;
            m_f(t);
        }
    };

    struct CubicAnimation : public Animation {
        CubicAnimation(float duration) : Animation(duration) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            t = t * t * t;
            m_f(t);
        }
    };

    struct QuarticAnimation : public Animation {
        QuarticAnimation(float duration) : Animation(duration) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            t = t * t * t * t;
            m_f(t);
        }
    };

    struct QuinticAnimation : public Animation {
        QuinticAnimation(float duration) : Animation(duration) {}

        virtual void onUpdate(float dt) override {
            float t = (dt - m_start) / ((m_start + m_duration) - m_start);
            t = t * t * t * t * t;
            m_f(t);
        }
    };


    struct Animate {
        static void Linear(float duration, std::function<void(float)>&& f) {
            m_animations.push_back(std::make_shared<LinearAnimation>(duration));
            m_animations.back()->m_f = f;
        }

        static void Step(float duration, float step, std::function<void(float)>&& f) {
            m_animations.push_back(std::make_shared<StepAnimation>(duration, step));
            m_animations.back()->m_f = f;
        }

        static void SmoothStep(float duration, std::function<void(float)>&& f) {
            m_animations.push_back(std::make_shared<SmoothStepAnimation>(duration));
            m_animations.back()->m_f = f;
        }

        static void EaseIn(float duration, std::function<void(float)>&& f) {
            m_animations.push_back(std::make_shared<EaseInAnimation>(duration));
            m_animations.back()->m_f = f;
        }

        static void EaseOut(float duration, std::function<void(float)>&& f) {
            m_animations.push_back(std::make_shared<EaseOutAnimation>(duration));
            m_animations.back()->m_f = f;
        }

        static void Curve(float duration, std::function<void(float)>&& f) {
            m_animations.push_back(std::make_shared<CurveAnimation>(duration));
            m_animations.back()->m_f = f;
        }

        static void Bounce(float duration, std::function<void(float)>&& f) {
            m_animations.push_back(std::make_shared<BounceAnimation>(duration));
            m_animations.back()->m_f = f;
        }

        static void Elastic(float duration, std::function<void(float)>&& f) {
            m_animations.push_back(std::make_shared<ElasticAnimation>(duration));
            m_animations.back()->m_f = f;
        }

        static void Back(float duration, std::function<void(float)>&& f) {
            m_animations.push_back(std::make_shared<BackAnimation>(duration));
            m_animations.back()->m_f = f;
        }

        static void Quadratic(float duration, std::function<void(float)>&& f) {
            m_animations.push_back(std::make_shared<QuadraticAnimation>(duration));
            m_animations.back()->m_f = f;
        }

        static void Cubic(float duration, std::function<void(float)>&& f) {
            m_animations.push_back(std::make_shared<CubicAnimation>(duration));
            m_animations.back()->m_f = f;
        }

        static void Quartic(float duration, std::function<void(float)>&& f) {
            m_animations.push_back(std::make_shared<QuarticAnimation>(duration));
            m_animations.back()->m_f = f;
        }

        static void Update(float dt) {
            for (auto& a : m_animations)
                a->update(dt);

            m_animations.erase(std::remove_if(m_animations.begin(), m_animations.end(), [dt](std::shared_ptr<Animation> a) {
                return a->finished(dt);
                }), m_animations.end());
        }

    private:
        static inline std::vector<std::shared_ptr<Animation>> m_animations;
    };
}

#endif // UI_ANIMATE