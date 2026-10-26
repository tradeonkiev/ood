#include "Model/Picture.h"
#include "View/SfmlEditorView.h"
#include "ViewModel/EditorViewModel.h"

#include <SFML/Graphics/RenderWindow.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 1280, 800 }), "edirot");
	window.setVerticalSyncEnabled(true);

	editor::Picture picture;
	editor::EditorViewModel viewModel(picture);
	editor::SfmlEditorView view(window, viewModel);

	view.Run();
}
