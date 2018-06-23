#include "application.hpp"

Application &application = Application::shared;

int main() {
	application.Run();
}