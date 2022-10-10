#include "Main.h"


using namespace std;





Component::Component(ImVec2 pos) : pos{ pos } {}

int Component::render() {

	return 0;
}

TextInfo::TextInfo(ImFont* font, ImColor* color) : font{ font }, color{ color } {}




TextDisplay::TextDisplay(string text, ImVec2 pos, TextInfo* info, Window* parent) : Component(pos), text{ text }, info{ info }, parent{ parent } {

	indexes = new vector<int>();


}



void TextDisplay::calculateIndexes() {

	lines = 0;

	indexes = new vector<int>();

	if (text.size() == 0) return;

	indexes->push_back(0);

	for (int i = 0; i < text.length(); i++) {

		if (text[i] == '\n') {
			i++;
			indexes->push_back(i);
			lines++;
		}

	}

	for (int i = 0; i < indexes->size(); i++) {

		cout << (*indexes)[i] << ", ";

	}

}

float TextDisplay::getLineWidth(int line) {

	calculateIndexes();

	int index = indexes->at(line);

	char current = text[index];

	float size = 0;

	while (index < text.length() && text[index] != '\n') {

		current = text[index];

		size += info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, string(1, current).c_str()).x;

		index++;

	}

	return size;


}

int TextDisplay::currentLine() {

	int i = 0;
	for(i = 0; i < indexes->size(); i++){

		if (cursor < indexes->at(i)) return  i - 1;
	
	}

	return max(i - 1, 0);

}


string TextDisplay::getLine(int line) {

	calculateIndexes();

	int index = indexes->at(line);

	string current = string(1, text[index]);

	float size = 0;

	while (index < text.length() && text[index] != '\n') {

		current = current + string(1, text[index]);

		index++;

	}

	return current;


}


int TextDisplay::currentMouseLine() {

	ImGuiIO& io = ImGui::GetIO();

	ImVec2 mPos = io.MousePos;


	ImVec2 currentPos = parent->pos + pos;

	float y_pos = currentPos.y;

	float standard_y = info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, " ").y;
	
	int i;

	for (i = 0; i < lines; i++) {

		if (mPos.y > y_pos && mPos.y < y_pos + standard_y) break;


		y_pos += standard_y;
	}

	return i;

}

float TextDisplay::currentMouseLinePixel() {

	ImGuiIO& io = ImGui::GetIO();

	ImVec2 mPos = io.MousePos;


	ImVec2 currentPos = parent->pos + pos;

	float y_pos = currentPos.y;

	float standard_y = info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, " ").y;

	int i;

	for (i = 0; i < lines; i++) {

		if (mPos.y > y_pos && mPos.y < y_pos + standard_y) break;


		y_pos += standard_y;
	}

	return y_pos;

}

int TextDisplay::currentMouseCharacter() {

	ImGuiIO& io = ImGui::GetIO();

	ImVec2 mPos = io.MousePos;

	int line_number = currentMouseLine();

	string current_line = getLine(line_number);

	ImVec2 currentPos = parent->pos + pos;

	currentPos.y = currentMouseLinePixel();


	int index = 0;
	for (index; index < current_line.size() - 1; index++) {

		string str = string(1, current_line[index]);

		if (abs(currentPos.x - io.MousePos.x) < (info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, str.c_str()).x) / 2) { break; }

		if (currentPos.x > io.MousePos.x) { break; }


		currentPos.x += info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, str.c_str()).x;


	}

	cout << endl << (*indexes)[line_number] + index << endl;

	return (*indexes)[line_number] + index;




}


void TextDisplay::drawCursor() {

	int c_line = currentLine();

	int excessIndex = cursor - indexes->at(c_line);

	float y_pos = parent->pos.y + pos.y + (c_line * (info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, "I").y));

	float x_pos = parent->pos.x + pos.x;

	for (int i = 0; i < excessIndex; i++) { 

		char currentChar = text[(indexes->at(c_line) + i)];
		
		x_pos += (info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, string(1, currentChar).c_str()).x);
	
	
	}

	parent->drawList->AddRect(ImVec2(x_pos, y_pos), ImVec2(x_pos + 1, y_pos + 10), ImGui::ColorConvertFloat4ToU32(info->color->operator ImVec4()));


}


int TextDisplay::render() {

	parent->drawList->AddText(info->font, info->font->FontSize, parent->pos + pos, ImGui::ColorConvertFloat4ToU32(info->color->operator ImVec4()), text.c_str());

	


	if (parent->focused) {

		calculateIndexes();

		drawCursor();

		ImGuiIO& io = ImGui::GetIO();

		ImVec2 mPos = io.MousePos;

		cout << currentMouseLine() << endl;

		
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			cursor = currentMouseCharacter();
		}

		
		if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
		
			text.insert(cursor, "\n");

			cursor++;
		
		
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
			if (cursor >= 1) {
				text.erase(cursor - 1, 1);

				cursor--;
			}

		}

		if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
			if (cursor >= 1) cursor--;
		}

		if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
			if(cursor <= text.size() - 1) cursor++;


		}

		if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {

			int cl = currentLine();

			if (cl <= 0) {}
			else {

				int difference = indexes->at(cl) - indexes->at(cl - 1);

				cursor -= difference;

				cursor = max(cursor, 0);


			}


		}

		if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {

			int cl = currentLine();

			if (cl >= indexes->size() - 1) {}
			else {
			
				int difference = indexes->at(cl + 1) - indexes->at(cl);

				cursor += difference;


				cursor = min(cursor, (int)text.size());

			
			}


		}

		if (!io.InputQueueCharacters.empty())
		{
			for (int i = 0; i < io.InputQueueCharacters.Size; i++)
			{
				auto c = io.InputQueueCharacters[i];


				if (c != 0 && (c == '\n' || c >= 32))
					text.insert(cursor, string(1, (char)c));
					cursor++;
			}
			io.InputQueueCharacters.resize(0);
		}
		


	}




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

	ImGui::Begin(title.c_str(), nullptr);

	pos = ImGui::GetWindowPos();

	focused = ImGui::IsWindowFocused();

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



	//ImFont* font_1 = io.Fonts->AddFontFromFileTTF("font.otf", 24.0f);

	//ImFont* font_2 = io.Fonts->AddFontFromFileTTF("font.otf", 24.0f);

	ImFont* font_1 = io.Fonts->AddFontDefault();

	ImFont* font_2 = io.Fonts->AddFontDefault();

	ImColor* red = new ImColor(255, 0, 0, 255);



	Window* window = new Window("Main window", ImVec2(0, 0), ImVec2(300, 300));

	Window* window_2 = new Window("Another window", ImVec2(0, 300), ImVec2(300, 300));


	window->addComponent(new TextDisplay("This is some text \n second line", ImVec2(20, 20), new TextInfo(font_1, red), window));


	



	app->addWindow(window);

	app->addWindow(window_2);


	app->render();


}