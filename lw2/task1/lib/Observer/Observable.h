#pragma once

#include <algorithm>
#include <vector>

namespace shapes::observer {
    template<typename Subject>
    class Observable {
    public:
        bool RegisterObserver(IObserver<Subject> &observer) {
            if (std::find(m_observers.begin(), m_observers.end(), &observer) != m_observers.end()) {
                return false;
                // TODO: throw std::invalid_argument("Observer is already registered");
            }
            m_observers.push_back(&observer);
            return true;
        };
        
        bool RemoveObserver(IObserver<Subject> &observer) {
            auto it = std::find(m_observers.begin(), m_observers.end(), &observer);
            if (it != m_observers.end()) {
                m_observers.erase(it);
                return true;
            }

            return false;
        };

    protected:
        void NotifyObservers(const Subject &subject) {
            for (auto *observer: m_observers) {
                observer->Update(subject);
            }
        };

    private:
        std::vector<IObserver<Subject> *> m_observers;
    };
} // namespace shapes::observer
