#include <iostream>
#include <string>
#include "Canvas/SvgCanvas.h"
#include "Commands/CommandProcessor.h"
#include "Log/ChangeLog.h"
#include "Picture/Picture.h"

int main(int argc, char *argv[]) {

    const std::string drawingName = argc > 1 ? argv[1] : "buster";
    gfx::SvgCanvas canvas(drawingName);
    shapes::Picture picture;
    shapes::ChangeLog log(std::cout);

    auto addedSubscription =
            picture.SubscribeToShapeAdded([&log](const shapes::Shape &shape) { log.OnShapeAdded(shape); });
    auto deletedSubscription =
            picture.SubscribeToShapeRemoved([&log](const shapes::Shape &shape) { log.OnShapeDeleted(shape); });
    auto movedSubscription = picture.SubscribeToShapeMoved(
            [&log](const shapes::Shape &shape, int dx, int dy) { log.OnShapeMoved(shape, dx, dy); });
    auto colorSubscription = picture.SubscribeToShapeColorChanged(
            [&log](const shapes::Shape &shape, gfx::Color oldColor, gfx::Color newColor) {
                log.OnShapeColorChanged(shape, oldColor, newColor);
            });
    auto geometrySubscription = picture.SubscribeToShapeGeometryChanged(
            [&log](const shapes::Shape &shape) { log.OnShapeGeometryChanged(shape); });

    CommandProcessor commandProcessor(picture, canvas, std::cout);
    commandProcessor.Run(std::cin);
}
