#pragma once

#include <functional>
#include <utility>

namespace shapes::observer
{
class Subscription
{
public:
	Subscription() = default;

	explicit Subscription(std::function<void()> disconnect)
		: m_disconnect(std::move(disconnect))
	{
	}

	~Subscription() { Disconnect(); }

	Subscription(Subscription&& other) noexcept
		: m_disconnect(std::exchange(other.m_disconnect, nullptr))
	{
	}

	Subscription& operator=(Subscription&& other) noexcept
	{
		Disconnect();
		m_disconnect = std::exchange(other.m_disconnect, nullptr);

		return *this;
	}

	void Disconnect()
	{
		if (!m_disconnect)
		{
			return;
		}

		auto disconnect = std::exchange(m_disconnect, nullptr);
		disconnect();
	}

private:
	std::function<void()> m_disconnect;
};
} // namespace shapes::observer
