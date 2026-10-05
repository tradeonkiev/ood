#pragma once

#include "IObserver.h"
#include "Subscription.h"
#include <algorithm>
#include <map>
#include <memory>
#include <stdexcept>

namespace shapes::observer
{
template <typename Subject>
class Observable
{
public:
	Observable() = default;

	Subscription Subscribe(IObserver<Subject>& observer)
	{
		std::weak_ptr<bool> connected = AddObserver(observer);
		return Subscription([connected] {
			if (auto flag = connected.lock())
			{
				*flag = false;
			}
		});
	};

	void RegisterObserver(IObserver<Subject>& observer) { AddObserver(observer); };

	void RemoveObserver(IObserver<Subject>& observer)
	{
		auto it = FindObserver(observer);
		if (it == m_observers.end())
		{
			throw std::invalid_argument("Observer is not registered");
		}

		*it->second.connected = false;
		m_observers.erase(it);
	};

protected:
	void NotifyObservers()
	{
		const int lastId = m_nextId;

		auto it = m_observers.begin();
		while (it != m_observers.end() && it->first < lastId)
		{
			if (!*it->second.connected)
			{
				it = m_observers.erase(it);
				continue;
			}

			int id = it->first;
			it->second.observer->Update();
			it = m_observers.upper_bound(id);
		}
	};

private:
	struct WrappedObserver
	{
		IObserver<Subject>* observer;
		std::shared_ptr<bool> connected;
	};

	using Observers = std::map<int, WrappedObserver>;

	std::shared_ptr<bool> AddObserver(IObserver<Subject>& observer)
	{
		std::erase_if(m_observers, [](const auto& item) { return !*item.second.connected; });

		if (FindObserver(observer) != m_observers.end())
		{
			throw std::invalid_argument("Observer is already registered");
		}

		auto connected = std::make_shared<bool>(true);
		m_observers.emplace(m_nextId++, WrappedObserver{ &observer, connected });
		return connected;
	}

	typename Observers::iterator FindObserver(IObserver<Subject>& observer)
	{
		return std::find_if(m_observers.begin(), m_observers.end(), [&observer](const auto& item) {
			return item.second.observer == &observer && *item.second.connected;
		});
	}

	Observers m_observers;
	int m_nextId = 0;
};
} // namespace shapes::observer
