#pragma once

namespace shapes::observer {
    template<typename Subject>
    class IObserver {
    public:
        virtual ~IObserver() = default;
        virtual void Update() = 0;
    };
} // namespace shapes::observer
