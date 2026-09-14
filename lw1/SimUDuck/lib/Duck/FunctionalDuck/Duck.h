#ifndef OOD_FUNCTIONAL_DUCK_H
#define OOD_FUNCTIONAL_DUCK_H

#include <cassert>
#include <functional>
#include <iostream>
#include <utility>

namespace functional {

    class FuncDuck {
    public:
        FuncDuck(std::function<unsigned()> flyBehavior, std::function<void()> quackBehavior,
                 std::function<void()> danceBehavior) : m_quackBehavior(std::move(quackBehavior)) {
            assert(m_quackBehavior);
            SetFlyBehavior(std::move(flyBehavior));
            SetDanceBehavior(std::move(danceBehavior));
        }

        void Quack() const { m_quackBehavior(); }

        static void Swim() { std::cout << "I m swimming" << std::endl; }

        void Fly() {
            m_flightCount = m_flyBehavior();
            if (m_flightCount == 0) {
                return;
            }

            if (m_flightCount % 2 == 0) {
                m_quackBehavior();
            }
        }

        unsigned GetFlightCount() const { return m_flightCount; }

        virtual void Dance() { m_danceBehavior(); }

        void SetFlyBehavior(std::function<unsigned()> flyBehavior) {
            assert(flyBehavior);
            m_flyBehavior = std::move(flyBehavior);
            m_flightCount = 0;
        }

        void SetDanceBehavior(std::function<void()> danceBehavior) {
            assert(danceBehavior);
            m_danceBehavior = std::move(danceBehavior);
        }

        virtual void Display() const = 0;
        virtual ~FuncDuck() = default;

    private:
        std::function<unsigned()> m_flyBehavior;
        std::function<void()> m_quackBehavior;
        std::function<void()> m_danceBehavior;
        unsigned m_flightCount = 0;
    };

} // namespace functional

#endif // OOD_FUNCTIONAL_DUCK_H
