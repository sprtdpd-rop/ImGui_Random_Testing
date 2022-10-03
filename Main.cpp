#include "Main.h"


using namespace std;
/*

void StyleParser::styleString(string str) {
	for (int i = 0; i < language->regs.size(); i++) {
		smatch m;

		regex_search(str, m, regex(language->regs.at(i)));

		
		
	}

}
*/

Component::Component(ImVec2 pos) : pos{ pos } {}

int Component::render() {

	return 0;
}

TextInfo::TextInfo(ImFont* font, ImColor* color) : font{ font }, color{ color } {}


TextDisplay::TextDisplay(string text, ImVec2 pos, TextInfo* info, Window* parent) : Component(pos), text{ text }, info{ info }, parent{ parent } {}

int TextDisplay::render() {

	parent->drawList->AddText(info->font, info->font->FontSize, parent->pos + pos, ImGui::ColorConvertFloat4ToU32(info->color->operator ImVec4()), text.c_str());

	ImGuiIO& io = ImGui::GetIO();

	ImVec2 mPos = io.MousePos;

	//float spaceSize = info->font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, text.c_str()).x;

	float currentPos = parent->pos.x + pos.x;

	int index = 0;

	for (index; index < text.size(); index++) {

		string str = string(1, text[index]);

		if (abs(currentPos - io.MousePos.x) < (info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, str.c_str()).x) / 2) { break; }

		if (currentPos > io.MousePos.x) { break; }

		currentPos += info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, str.c_str()).x;

	
	}

	bool* keys = io.KeysDown;

	io.KeyRepeatDelay = 0.5f;


	for (int i = 32; i < 128; i++) {

		char ch = i;

		if (!io.KeyShift && ch >= 65 && ch <= 90) ch = ch + 32;

		if(keys[i] == true) text.insert(index, string(1,(char)ch));

		//cout << keys[i] << ", ";
	
	}

	//if (*io.KeysDown) { text.insert(index, "l"); }

	parent->drawList->AddRect(ImVec2(currentPos, parent->pos.y + pos.y), ImVec2(currentPos, parent->pos.y + pos.y + 30), ImGui::ColorConvertFloat4ToU32(info->color->operator ImVec4()));


	cout << text << "  " << currentPos<<endl;
 

	return 0;
}





Window::Window(string title, ImVec2 pos, ImVec2 size) :title{ title }, pos{ pos }, size{ size } {


}

int Window::addComponent(Component* component) {

	components.push_back(component);

	return 0;
}

void Window::render() {

	ImGui::SetNextWindowPos(pos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(size, ImGuiCond_Once);

	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin(title.c_str(), nullptr, 0);

	pos = ImGui::GetWindowPos();

	drawList = ImGui::GetWindowDrawList();

	for (int i = 0; i < components.size(); i++) {

		components[i]->render();

	}

	//pDrawList->AddText(pos + ImVec2(30, 30), ImColor(255, 0, 0), "test");

	ImGui::End();

}





Application::Application(string label, ImVec2 size) {

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(size.x, size.y, label.c_str(), NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}


	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	gladLoadGL();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImFont* defaultFont = io.Fonts->AddFontDefault();



}

int Application::addWindow(Window* window) {

	windowList.push_back(window);

	return 0;

}

int Application::render() {

	while (!glfwWindowShouldClose(window))
	{

		glClear(GL_COLOR_BUFFER_BIT);


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		for (int i = 0; i < windowList.size(); i++) {

			windowList[i]->render();

		}


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}









int main()
{


	Application* app = new Application("Test app", ImVec2(1024, 1024));



	ImGuiIO& io = ImGui::GetIO(); (void)io;



	ImFont* font_1 = io.Fonts->AddFontFromFileTTF("font.otf", 24.0f);

	//ImFont* font_2 = io.Fonts->AddFontFromFileTTF("font.otf", 24.0f);

	//ImFont* font_1 = io.Fonts->AddFontDefault();

	//ImFont* font_2 = io.Fonts->AddFontDefault();

	ImColor* red = new ImColor(255, 0, 0, 255);

	//ImColor* green = new ImColor(0, 255, 0, 255);



	Window* window = new Window("Main window", ImVec2(0, 0), ImVec2(300, 300));

	//Window* window_2 = new Window("Another window", ImVec2(0, 300), ImVec2(300, 300));


	window->addComponent(new TextDisplay("abcdef", ImVec2(20, 20), new TextInfo(font_1, red), window));

	//window->addComponent(new TextDisplay("II", ImVec2(60, 60), new TextInfo(font_2, green), window));



	//window_2->addComponent(new TextDisplay("III", ImVec2(30, 60), new TextInfo(font_1, red), window_2));

	//window_2->addComponent(new TextDisplay("w", ImVec2(60, 60), new TextInfo(font_2, green), window_2));



	app->addWindow(window);

	//app->addWindow(window_2);


	app->render();


}