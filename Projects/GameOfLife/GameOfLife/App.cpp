#include "App.h"

wxIMPLEMENT_APP(App);

App::App() {

}

App::~App() {

}
 
bool App::OnInit() {
	MainWindow* pWindow = new MainWindow();
	pWindow->Show();
	return true;
}