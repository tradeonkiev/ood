#include <iostream>
#include "../Picture/Picture.h"

namespace shapes::observer {
    class PictureObserver : public IObserver<shapes::Picture> {
    public:
        explicit PictureObserver(std::ostream &output) : m_output(output) {}

        void Update(const Picture &picture) override {
            m_output << "Picture changed. Shapes: " << picture.GetShapeCount() << '\n';
        }

    private:
        std::ostream &m_output;
    };
} // namespace shapes::observer
