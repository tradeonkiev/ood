#pragma once

#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>
#include "Observer/Subscription.h"

namespace shapes::observer {
    using observer::Subscription;

    template<typename Signature>
    class Event;

    template<typename... Args>
    class Event<void(Args...)> {
    public:
        using Handler = std::function<void(Args...)>;

        Event() = default;

        Subscription Connect(Handler handler) {
            if (!handler) {
                throw std::invalid_argument("Handler cannot be null");
            }

            RemoveDisconnected();

            auto connected = std::make_shared<bool>(true);
            m_handlers.emplace(m_nextId++, Entry{std::make_shared<Handler>(std::move(handler)), connected});

            std::weak_ptr<bool> weakConnected = connected;
            return Subscription([weakConnected] {
                if (auto flag = weakConnected.lock()) {
                    *flag = false;
                }
            });
        }

        void Notify(Args... args) {
            const int lastId = m_nextId;

            auto it = m_handlers.begin();
            while (it != m_handlers.end() && it->first < lastId) {
                if (!*it->second.connected) {
                    it = m_handlers.erase(it);
                    continue;
                }

                const int id = it->first;
                auto handler = it->second.handler;
                (*handler)(args...);
                it = m_handlers.upper_bound(id);
            }
        }

    private:
        struct Entry {
            std::shared_ptr<Handler> handler;
            std::shared_ptr<bool> connected;
        };

        void RemoveDisconnected() {
            std::erase_if(m_handlers, [](const auto &item) { return !*item.second.connected; });
        }

        std::map<int, Entry> m_handlers;
        int m_nextId = 0;
    };
} // namespace shapes::observer
