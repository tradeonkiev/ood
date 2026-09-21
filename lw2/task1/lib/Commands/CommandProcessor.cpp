#include "CommandProcessor.h"

#include "Strategies/Ellipse/EllipseStrategy.h"
#include "Strategies/Line/LineStrategy.h"
#include "Strategies/Rectangle/RectangleStrategy.h"
#include "Strategies/Text/TextStrategy.h"
#include "Strategies/Triangle/TriangleStrategy.h"

#include <array>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>

CommandProcessor::CommandProcessor(shapes::Picture &picture, gfx::ICanvas &canvas, std::ostream &output) :
    m_picture(picture), m_canvas(canvas), m_output(output) {}

void CommandProcessor::Run(std::istream &input) {
    std::string commandLine;
    while (std::getline(input, commandLine)) {
        try {
            Execute(commandLine);
        } catch (const std::exception &error) {
            m_output << "Error: " << error.what() << '\n';
        }
    }
}

void CommandProcessor::Execute(const std::string &commandLine) {
    std::istringstream input(commandLine);
    std::string command;
    if (!(input >> command)) {
        return;
    }

    if (command == "AddShape") {
        std::string id;
        gfx::Color color;
        std::string type;
        input >> id >> color >> type;
        m_picture.AddShape(std::make_unique<shapes::Shape>(id, color, ReadGeometry(type, input)));
        return;
    }

    if (command == "MoveShape") {
        std::string id;
        double dx = 0;
        double dy = 0;
        input >> id >> dx >> dy;

        auto &shape = m_picture.GetShape(id);
        shape.Move(dx, dy);
        return;
    }

    if (command == "MovePicture") {
        double dx = 0;
        double dy = 0;
        input >> dx >> dy;
        m_picture.Move(dx, dy);
        return;
    }

    if (command == "DeleteShape") {
        std::string id;
        input >> id;
        m_picture.DeleteShape(id);
        return;
    }

    if (command == "List") {
        for (std::size_t index = 0; index < m_picture.GetShapeCount(); ++index) {
            const auto &shape = m_picture.GetShapeAt(index);
            m_output << index + 1 << ' ' << shape.GetType() << ' ' << shape.GetId() << ' '
                     << shape.GetColor().ToHexString() << ' ' << shape.GetParameters() << '\n';
        }
        return;
    }

    if (command == "ChangeColor") {
        std::string id;
        gfx::Color color;
        input >> id >> color;
        m_picture.GetShape(id).SetColor(color);
        return;
    }

    if (command == "ChangeShape") {
        std::string id;
        std::string type;
        input >> id >> type;
        auto strategy = ReadGeometry(type, input);
        m_picture.GetShape(id).SetStrategy(std::move(strategy));
        return;
    }

    if (command == "DrawShape") {
        std::string id;
        input >> id;
        m_canvas.Clear();
        m_picture.GetShape(id).Draw(m_canvas);
        return;
    }

    if (command == "DrawPicture") {
        m_picture.Draw(m_canvas);
        return;
    }

    if (command == "Help") {
        std::cout << std::endl
                  << "===========================" << std::endl
                  << "AddShape <id> <цвет> <тип> <параметры>" << std::endl
                  << "MoveShape <id> <dx> <dy>" << std::endl
                  << "MovePicture <dx> <dy>" << std::endl
                  << "DeleteShape <id>" << std::endl
                  << "---------------------------" << std::endl
                  << "ChangeShape <id> <тип> <параметры>" << std::endl
                  << "ChangeColor <id> <цвет>" << std::endl
                  << "---------------------------" << std::endl
                  << "DrawShape <id>" << std::endl
                  << "DrawPicture" << std::endl
                  << "---------------------------" << std::endl
                  << "List" << std::endl
                  << std::endl;
        return;
    }

    throw std::invalid_argument("Unknown command: " + command);
}

std::unique_ptr<shapes::IShapeGeometry> CommandProcessor::ReadGeometry(const std::string &type, std::istream &input) {
    if (type == "circle") {
        double x = 0;
        double y = 0;
        double radius = 0;
        input >> x >> y >> radius;
        return std::make_unique<EllipseStrategy>(shapes::Point{x, y}, radius, radius);
    }

    if (type == "rectangle") {
        double left = 0;
        double top = 0;
        double width = 0;
        double height = 0;
        input >> left >> top >> width >> height;
        return std::make_unique<RectangleStrategy>(shapes::Rect{left, top, width, height});
    }

    if (type == "triangle") {
        std::array<shapes::Point, 3> vertices{};
        for (auto &vertex: vertices) {
            input >> vertex.x >> vertex.y;
        }
        return std::make_unique<TriangleStrategy>(vertices);
    }

    if (type == "line") {
        double x1 = 0;
        double y1 = 0;
        double x2 = 0;
        double y2 = 0;
        input >> x1 >> y1 >> x2 >> y2;
        return std::make_unique<LineStrategy>(shapes::Point{x1, y1}, shapes::Point{x2, y2});
    }

    if (type == "text") {
        double left = 0;
        double top = 0;
        double fontSize = 0;
        input >> left >> top >> fontSize;

        std::string text;
        std::getline(input >> std::ws, text);
        return std::make_unique<TextStrategy>(shapes::Point{left, top}, fontSize, std::move(text));
    }

    throw std::invalid_argument("Unknown shape type: " + type);
}
