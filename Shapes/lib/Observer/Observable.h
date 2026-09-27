#pragma once

#include <algorithm>
#include <cstddef>
#include <map>
#include <stdexcept>

namespace shapes::observer {
    template<typename Subject>
    class Observable {
    public:
        void RegisterObserver(IObserver<Subject> &observer) {
            if (FindObserver(observer) != m_observers.end()) {
                throw std::invalid_argument("Observer is already registered");
            }

            m_observers.emplace(m_nextId++, &observer);
        };

        void RemoveObserver(IObserver<Subject> &observer) {
            auto it = FindObserver(observer);
            if (it == m_observers.end()) {
                throw std::invalid_argument("Observer is not registered");
            }

            m_observers.erase(it);
        };

    protected:
        void NotifyObservers() {
            const int lastId = m_nextId;

            auto it = m_observers.begin();
            while (it != m_observers.end() && it->first < lastId) {
                int id = it->first;
                it->second->Update();
                it = m_observers.upper_bound(id);
            }
        };

    private:
        typename std::map<int, IObserver<Subject> *>::iterator FindObserver(IObserver<Subject> &observer) {
            return std::find_if(m_observers.begin(), m_observers.end(),
                                [&observer](const auto &entry) { return entry.second == &observer; });
        }

        std::map<int, IObserver<Subject> *> m_observers;
        int m_nextId = 0;
    };
} // namespace shapes::observer
