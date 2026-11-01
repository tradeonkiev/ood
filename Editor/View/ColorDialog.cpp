#include "ColorDialog.h"

#include <windows.h>

#include <iostream>

namespace editor
{

ColorDialog::ColorDialog(void* owner)
	: m_owner(owner)
{
	m_customColors.fill(RGB(255, 255, 255));
}

std::optional<Color> ColorDialog::AskColor(Color initColor)
{
	CHOOSECOLORW dialog{};
	dialog.lStructSize = sizeof(dialog);
	dialog.hwndOwner = static_cast<HWND>(m_owner);
	dialog.rgbResult = RGB(initColor.red, initColor.green, initColor.blue);
	dialog.lpCustColors = m_customColors.data();
	dialog.Flags = CC_RGBINIT;

	if (!ChooseColorW(&dialog))
	{
		std::cout << "ChooseColorW failed with error code: " << CommDlgExtendedError() << std::endl;
		return std::nullopt;
	}

	return Color{ GetRValue(dialog.rgbResult), GetGValue(dialog.rgbResult), GetBValue(dialog.rgbResult) };
}

} // namespace editor
