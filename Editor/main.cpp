#include "Picture/Picture.h"
#include "Presenter/EditorPresenter.h"
#include "View/SfmlEditorView.h"

#include <SFML/Graphics/RenderWindow.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 1280, 800 }), "edirot");
	window.setVerticalSyncEnabled(true);

	shapes::Picture picture;
	editor::SfmlEditorView view(window);
	editor::EditorPresenter presenter(picture, view);

	view.Run();
}
