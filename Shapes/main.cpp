#include <iostream>
#include <string>
#include "Canvas/SvgCanvas.h"
#include "Commands/CommandProcessor.h"
#include "Observer/PictureObserver.h"
#include "Picture/Picture.h"

int main(int argc, char *argv[]) {

    const std::string drawingName = argc > 1 ? argv[1] : "picture";
    gfx::SvgCanvas canvas(drawingName);

    shapes::Picture picture;
    shapes::observer::PictureObserver obs(std::cout, picture);
    auto subscription = picture.Subscribe(obs);

    CommandProcessor commandProcessor(picture, canvas, std::cout);
    commandProcessor.Run(std::cin);
}
