#pragma once

namespace shapes::observer {
    template<typename Subject>
    class IObserver {
    public:
        virtual ~IObserver() = default;
        virtual void Update(const Subject &subject) = 0;
    };
} // namespace shapes::observer
