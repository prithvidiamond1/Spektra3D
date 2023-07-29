
#include "App.h"

int main(int argc, char** argv) {
	std::string title = "Spektra";
	App app(title, 1280, 720, argc, argv);
	app.run();

	return 0;
}