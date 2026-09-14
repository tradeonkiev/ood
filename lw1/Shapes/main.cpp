#include "Canvas/SvgCanvas.h"
#include "Commands/CommandProcessor.h"
#include "Picture/Picture.h"

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    const std::string drawingName = argc > 1 ? argv[1] : "picture";
    gfx::SvgCanvas canvas(drawingName);

    shapes::Picture picture;
    CommandProcessor commandProcessor(picture, canvas, std::cout);
    commandProcessor.Run(std::cin);
}
