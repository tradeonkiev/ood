#include "TextStrategy.h"
#include "Geometry/Validation.h"

#include <sstream>
#include <utility>

TextStrategy::TextStrategy(shapes::Point topLeft, double fontSize, std::string text) :
    m_topLeft(topLeft), m_fontSize(fontSize), m_text(std::move(text)) {
    shapes::detail::ValidateBounds({topLeft.x, topLeft.y, 0, m_fontSize});
}

void TextStrategy::Draw(gfx::ICanvas &canvas, gfx::Color color) const {
    canvas.SetColor(color);
    canvas.DrawText(m_topLeft, m_fontSize, m_text);
    canvas.Flush();
}

void TextStrategy::Move(int dx, int dy) { m_topLeft = {m_topLeft.x + dx, m_topLeft.y + dy}; }

shapes::Rect TextStrategy::GetBounds() const { return {m_topLeft.x, m_topLeft.y, 0, m_fontSize}; }

void TextStrategy::SetBounds(const shapes::Rect &bounds) {
    shapes::detail::ValidateBounds(bounds);
    m_topLeft = {bounds.left, bounds.top};
    m_fontSize = bounds.height;
}

std::string TextStrategy::GetType() const { return "text"; }

std::string TextStrategy::GetParameters() const {
    std::ostringstream output;
    output << m_topLeft.x << ' ' << m_topLeft.y << ' ' << m_fontSize << ' ' << m_text;
    return output.str();
}
