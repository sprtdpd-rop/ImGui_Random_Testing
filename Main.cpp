#include "Main.h"


using namespace std;

string digits = "0123456789";
string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";


string keywords[] = {
	
"VAR",
"TEST",
"CLASS"

};


bool contains(std::string sub, std::string str) {
	if (str.find(sub) != std::string::npos) return true;
	return false;
}

bool contains(char sub, std::string str) {
	return contains(std::string(1, sub), str);
}

int contains(std::string sub, std::string* str, int size) {
	for (int i = 0; i < size; i++) {
		if (sub.compare(str[i]) == 0) {
			return i;
		}
	}
	return -1;
}

Object::Object(string name, int type, SymbolTable* table) : name{ name }, type{ type }, table{ table } {};




Component::Component(ImVec2 pos) : pos{ pos } {}

int Component::render() {

	return 0;
}

void Component::added_update() {}

TextInfo::TextInfo(ImFont* font, ImColor* color) : font{ font }, color{ color } {}

SymbolTable::SymbolTable() {

	objects = new vector<Object*>;


}


Lexer::Lexer(string str, ColorData* data) : data{ data } {

	reset(str);

}

void Lexer::reset(string str) {

	index = 0;

	increment();

	this->str = str;

	table = new SymbolTable();


}

//VAR

//NUM

void Lexer::increment() {

	if (index < str.size()) {

		currentChar = str.at(index);
		index++;
		

	}
	else {

		currentChar = '\0';
	
	}

}

pair<string, ImColor> Lexer::lexToken() {

	if (index >= str.size()) return make_pair("\0", data->default_color);


	if (index < str.size()) {

		string token_string = "";

		if (contains(currentChar, " \n") && currentChar != '\0') {

			token_string += currentChar;
			increment();

			while (contains(currentChar, " \n") && currentChar != '\0') {

				token_string += currentChar;	
				increment();
			
			}

			tokens.push_back(make_pair(token_string, data->number_color));
			return make_pair(token_string, data->default_color);

		}

		if (contains(currentChar, digits) && currentChar != '\0') {

			token_string += currentChar;
			increment();

			while (contains(currentChar, digits) && currentChar != '\0') {

				token_string += currentChar;
				increment();

			}

			tokens.push_back(make_pair(token_string, data->number_color));

			return make_pair(token_string, data->number_color);

		}




		if (contains(currentChar, letters) && currentChar != '\0') {

			token_string += currentChar;
			increment();

			while (contains(currentChar, letters + digits + "_") && currentChar != '\0') {

				token_string += currentChar;
				increment();

			}

			bool keyword = false;

			for (string str : keywords) {

				if (str.compare(token_string) == 0) {

					if (token_string.compare("CLASS") == 0) {
					
						in_class = true;
					
					}


					keyword = true;
					break;


				}
			
			
			}

			if (keyword) {

				tokens.push_back(make_pair(token_string, data->keyword_color));

				return make_pair(token_string, data->keyword_color);
			
			
			}


			else {
				
				if (in_class) {

					if (tokens.at(max(0, (int)(tokens.size() - 2))).first.compare("CLASS") == 0) {

						table->objects->push_back(new Object(token_string, 1));

						table->objects->at(table->objects->size() - 1)->table = new SymbolTable();

						last_thing = token_string;

						return make_pair(token_string, data->identifier_color);					
					
					}
					else {


						for (int i = 0; i < table->objects->size(); i++) {

							if (table->objects->at(i)->name.compare(last_thing) == 0) {
								
								table->objects->at(i)->table->objects->push_back((new Object(token_string, 0)));

								return make_pair(token_string, data->identifier_color);

							
							}

						}
					}

				
				}
				else {

					table->objects->push_back(new Object(token_string, 0));

					return make_pair(token_string, data->identifier_color);

				}

			}

		}








		if (contains(currentChar, "\"") && currentChar != '\0') {

			token_string += currentChar;
			increment();


			while (currentChar != '\"' && currentChar != '\0') {

				token_string += currentChar;
				increment();

			}

			token_string += currentChar;
			increment();

			return make_pair(token_string, data->string_color);

		}

	}

	

}


void ColorData::addColor(string str, ImColor color) {


	lists.insert({ str, color });


}





TextDisplay::TextDisplay(string text, ImVec2 pos, TextInfo* info, Window* parent) : Component(pos), text{ text }, info{ info }, parent{ parent }{

	indexes = new vector<int>();


}

void TextDisplay::added_update() {

	lexer = new Lexer(text, parent->colorData);

}



void TextDisplay::calculateIndexes() {

	lines = 0;

	indexes = new vector<int>();

	indexes->push_back(0);

	if (text.size() == 0) return;	

	for (int i = 0; i < text.length(); i++) {

		if (text[i] == '\n') {
			
			indexes->push_back(i + 1);
			lines++;
		}

	}

}

int TextDisplay::currentLine() {

	int i = 0;
	for(i = 0; i < indexes->size(); i++){

		if (cursor < indexes->at(i)) return  i - 1;
	
	}

	return max(i - 1, 0);

}


string TextDisplay::getLine(int line) {

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

int TextDisplay::currentMouseCharacter() {

	ImGuiIO& io = ImGui::GetIO();

	ImVec2 mPos = io.MousePos;

	int line_number = currentMouseLine();

	string current_line = getLine(line_number);

	ImVec2 currentPos = parent->pos + pos;



	int index = 0;
	for (index; index < current_line.size() - 1; index++) {

		string str = string(1, current_line[index]);

		if (abs(currentPos.x - io.MousePos.x) < (info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, str.c_str()).x) / 2) { break; }

		if (currentPos.x > io.MousePos.x) { break; }


		currentPos.x += info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, str.c_str()).x;


	}

	return (*indexes)[line_number] + index;




}


void TextDisplay::drawCursor() {

	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	if (millis % 1000 > 500) {
		float y_comp = info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, "I").y;

		int c_line = currentLine();

		int excessIndex = cursor - indexes->at(c_line);

		float y_pos = parent->pos.y + pos.y + (c_line * (y_comp));

		float x_pos = parent->pos.x + pos.x;

		for (int i = 0; i < excessIndex; i++) {

			char currentChar = text[(indexes->at(c_line) + i)];

			x_pos += (info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, string(1, currentChar).c_str()).x);


		}

		parent->drawList->AddRect(ImVec2(x_pos, y_pos), ImVec2(x_pos + 0.5, y_pos + y_comp), ImGui::ColorConvertFloat4ToU32(info->color->operator ImVec4()));
	}

}

void TextDisplay::handleKeys() {

	ImGuiIO& io = ImGui::GetIO();

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

	if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
		if (cursor <= text.size() - 1) {
			text.erase(cursor, 1);
		}

	}

	if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
		if (cursor >= 1) cursor--;
	}

	if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
		if (cursor <= text.size() - 1) cursor++;


	}

	if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {

		int cl = currentLine();

		if (cl <= 0) {}
		else {

			int difference = indexes->at(cl) - indexes->at(cl - 1);

			if (cursor - difference >= indexes->at(cl)) {

				cursor = indexes->at(cl) - 1;

			}
			else {
				cursor -= difference;
			}
			cursor = max(cursor, 0);


		}


	}

	if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {

		int cl = currentLine();

		if (cl >= indexes->size() - 1) {}
		else {

			int difference = indexes->at(cl + 1) - indexes->at(cl);

			if (cl < indexes->size() - 2 && cursor + difference > indexes->at(cl + 2)) {

				cursor = indexes->at(cl + 2) - 1;

			}
			else {

				cursor += difference;
			}


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

void TextDisplay::drawText() {

	lexer->reset(text);

	
	ImVec2 text_position = parent->pos + pos;

	int start_x = parent->pos.x + pos.x;

	float standard_y = info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, " ").y;

	float standard_x = info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, " ").x;

	pair<std::string, ImColor> currentToken;

	currentToken = lexer->lexToken();


	while (currentToken.first != "\0") {

		
		if (currentToken.first.at(0) == '\n', currentToken.first.at(0) == ' ') {

			for (int i = 0; i < currentToken.first.size(); i++) {

				if (currentToken.first.at(i) == '\n') {

					text_position.x = start_x;

					text_position.y += standard_y;

				}

				if (currentToken.first.at(i) == ' ') {

					text_position.x += standard_x;

				}
			}
		}

		else {

			parent->drawList->AddText(info->font, info->font->FontSize, ImVec2(text_position.x, text_position.y), ImGui::ColorConvertFloat4ToU32(currentToken.second.operator ImVec4()), currentToken.first.c_str());

			text_position.x += info->font->CalcTextSizeA(info->font->FontSize, FLT_MAX, -1.0f, currentToken.first.c_str()).x;

		}

		currentToken = lexer->lexToken();
	}



}



int TextDisplay::render() {

		drawText();

		for (int i = 0; i < lexer->table->objects->size(); i++) {
			
			

		
			cout << lexer->table->objects->at(i)->name << endl;

			if (lexer->table->objects->at(i)->type == 1 && lexer->table->objects->size() > 0) {

				//cout <<"\t\t" << lexer->table->objects->at(i)->name << ": " << lexer->table->objects->at(i)->table->objects->at(0)->name << endl;


			}
		
		}
		


		if (parent->focused) {

			calculateIndexes();

			drawCursor();

			handleKeys();		


		}

	return 0;
}





Window::Window(string title, ImVec2 pos, ImVec2 size) :title{ title }, pos{ pos }, size{ size } {

}

int Window::addComponent(Component* component) {

	components.push_back(component);

	component->added_update();

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





Application::Application(string label, ImVec2 size, ColorData* colorData) : colorData {colorData}{

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

	window->parent = this;

	window->colorData = colorData;

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


	ColorData* data = new ColorData();

	data->addColor("VAR", ImColor(64, 64, 255, 255));

	data->default_color = ImColor(255, 255, 255, 255);

	data->number_color = ImColor(255, 0, 255, 255);

	data->keyword_color = ImColor(0, 0, 255, 255);

	data->identifier_color = ImColor(192, 255, 192, 255);

	data->string_color = ImColor(128, 255, 128, 255);



	Application* app = new Application("Test app", ImVec2(1024, 1024), data);



	ImGuiIO& io = ImGui::GetIO(); (void)io;



	//ImFont* font_1 = io.Fonts->AddFontFromFileTTF("font.otf", 24.0f);

	//ImFont* font_2 = io.Fonts->AddFontFromFileTTF("font.otf", 24.0f);

	ImFont* font_1 = io.Fonts->AddFontDefault();

	//ImFont* font_2 = io.Fonts->AddFontDefault();

	ImColor* red = new ImColor(255, 0, 0, 255);



	Window* window = new Window("Main window", ImVec2(0, 0), ImVec2(300, 300));

	//Window* window_2 = new Window("Another window", ImVec2(0, 300), ImVec2(300, 300));

	
	app->addWindow(window);

	window->addComponent(new TextDisplay("5656 \"string\"", ImVec2(20, 20), new TextInfo(font_1, red), window));



	//app->addWindow(window_2);


	app->render();


}