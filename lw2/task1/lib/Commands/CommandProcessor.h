#pragma once

#include "Canvas/ICanvas.h"
#include "Picture/Picture.h"

#include <iosfwd>
#include <string>

class CommandProcessor {
public:
    CommandProcessor(shapes::Picture &picture, gfx::ICanvas &canvas, std::ostream &output);
    void Run(std::istream &input);
    void Execute(const std::string &commandLine);

private:
    std::unique_ptr<shapes::IShapeGeometry> ReadGeometry(const std::string &type, std::istream &input);

    shapes::Picture &m_picture;
    gfx::ICanvas &m_canvas;
    std::ostream &m_output;
};
