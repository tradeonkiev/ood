#include "Model/Document.h"
#include "View/EditorApp.h"

#include <memory>

int main()
{
	editor::EditorApp app;
	app.OpenWindow(std::make_shared<editor::Document>());
	app.Run();
}
