#pragma once

#include <algorithm>
#include <vector>

namespace shapes::observer {
    template<typename Subject>
    class Observable {
    public:
        void RegisterObserver(IObserver<Subject> &observer) {
            if (std::find(m_observers.begin(), m_observers.end(), &observer) != m_observers.end()) {
                throw std::invalid_argument("Observer is already registered");
            }
            m_observers.push_back(&observer);
        };

        void RemoveObserver(IObserver<Subject> &observer) {
            auto it = std::find(m_observers.begin(), m_observers.end(), &observer);
            if (it != m_observers.end()) {
                m_observers.erase(it);

                return;
            }

            throw std::invalid_argument("Observer is not registered");
        };

    protected:
        void NotifyObservers() {
            for (auto *observer: m_observers) {
                observer->Update();
            }
        };

    private:
        std::vector<IObserver<Subject> *> m_observers;
    };
} // namespace shapes::observer
