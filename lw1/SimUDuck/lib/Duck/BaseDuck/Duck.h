#ifndef DUCK_H
#define DUCK_H

#include "../Fly/IFlyBehavior.h"
#include "../Quack/IQuakBehavior.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

#include "../Dance/IDanceBehavior.h"

class Duck {
public:
    Duck(std::unique_ptr<IFlyBehavior> &&flyBehavior, std::unique_ptr<IQuackBehavior> &&quackBehavior,
         std::unique_ptr<IDanceBehavior> &&danceBehavior) : m_quackBehavior(std::move(quackBehavior)) {
        assert(m_quackBehavior);
        SetFlyBehavior(std::move(flyBehavior));
        SetDanceBehavior(std::move(danceBehavior));
    }

    void Quack() const { m_quackBehavior->Quack(); }

    static void Swim() { std::cout << "I m swimming" << std::endl; }

    void Fly() const {
        if (!m_flyBehavior->CanFly()) {
            return;
        }

        m_flyBehavior->Fly();

        if (const unsigned flightCount = m_flyBehavior->GetFlightCount(); flightCount % 2 == 0 && flightCount > 0) {
            m_quackBehavior->Quack();
        }
    }

    unsigned GetFlightCount() const { return m_flyBehavior->GetFlightCount(); }

    virtual void Dance() { m_danceBehavior->Dance(); }

    void SetFlyBehavior(std::unique_ptr<IFlyBehavior> &&flyBehavior) {
        assert(flyBehavior);
        m_flyBehavior = std::move(flyBehavior);
    }

    void SetDanceBehavior(std::unique_ptr<IDanceBehavior> &&danceBehavior) {
        assert(danceBehavior);
        m_danceBehavior = std::move(danceBehavior);
    }

    virtual void Display() const = 0;
    virtual ~Duck() = default;

private:
    std::unique_ptr<IFlyBehavior> m_flyBehavior;
    std::unique_ptr<IQuackBehavior> m_quackBehavior;
    std::unique_ptr<IDanceBehavior> m_danceBehavior;
};

#endif
