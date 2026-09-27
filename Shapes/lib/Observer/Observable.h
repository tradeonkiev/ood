#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <stdexcept>
#include "IObserver.h"
#include "Subscription.h"

namespace shapes::observer {
    template<typename Subject>
    class Observable {
    public:
        Observable() = default;

        Subscription Subscribe(IObserver<Subject> &observer) {
            const int id = AddObserver(observer);

            std::weak_ptr<Observers> opserverPtr = m_observers;
            return Subscription([opserverPtr, id] {
                if (auto observers = opserverPtr.lock()) {
                    observers->erase(id);
                }
            });
        };

        void RegisterObserver(IObserver<Subject> &observer) { AddObserver(observer); };

        void RemoveObserver(IObserver<Subject> &observer) {
            auto it = FindObserver(observer);
            if (it == m_observers->end()) {
                throw std::invalid_argument("Observer is not registered");
            }

            m_observers->erase(it);
        };

    protected:
        void NotifyObservers() {
            const int lastId = m_nextId;

            auto it = m_observers->begin();
            while (it != m_observers->end() && it->first < lastId) {
                int id = it->first;
                it->second->Update();
                it = m_observers->upper_bound(id);
            }
        };

    private:
        using Observers = std::map<int, IObserver<Subject> *>;

        int AddObserver(IObserver<Subject> &observer) {
            if (FindObserver(observer) != m_observers->end()) {
                throw std::invalid_argument("Observer is already registered");
            }

            const int id = m_nextId++;
            m_observers->emplace(id, &observer);
            return id;
        }

        typename Observers::iterator FindObserver(IObserver<Subject> &observer) {
            return std::find_if(m_observers->begin(), m_observers->end(),
                                [&observer](const auto &object) { return object.second == &observer; });
        }

        std::shared_ptr<Observers> m_observers = std::make_shared<Observers>();
        int m_nextId = 0;
    };
} // namespace shapes::observer
