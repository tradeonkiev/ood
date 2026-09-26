#include <iostream>
#include "../Picture/Picture.h"

namespace shapes::observer {
    class PictureObserver : public IObserver<shapes::Picture> {
    public:
        explicit PictureObserver(std::ostream &output, const Picture &picture) : m_output(output), m_picture(picture) {}

        void Update() override { m_output << "Picture changed. Shapes: " << m_picture.GetShapeCount() << '\n'; }

    private:
        std::ostream &m_output;
        const Picture &m_picture;
    };
} // namespace shapes::observer
