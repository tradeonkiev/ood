#include <iostream>
#include <string>
#include "Canvas/SvgCanvas.h"
#include "Commands/CommandProcessor.h"
#include "Observer/PictureObserver.h"
#include "Picture/Picture.h"

int main(int argc, char *argv[]) {

    const std::string drawingName = argc > 1 ? argv[1] : "picture";
    gfx::SvgCanvas canvas(drawingName);

    shapes::observer::PictureObserver obs(std::cout);
    shapes::Picture picture;
    picture.RegisterObserver(obs);

    CommandProcessor commandProcessor(picture, canvas, std::cout);
    commandProcessor.Run(std::cin);
}


// TODO: обьеденить лабу 1 и 2 так, как вторая лаба является логическим продолжением первой (мы просто добавили новый
// паттерн)
