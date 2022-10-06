#pragma once
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"
#include <functional>
#include <climits>
#include <string>
#include <sstream>
#include <cwchar>
#include <cctype>
#include <algorithm>
#include <cmath>
#include <regex>
#include <map>

#include <sys/stat.h>
#if defined (WIN32) || defined (_WIN32) || defined (__WIN32)
#define OSWIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <dirent.h>
#endif // defined (WIN32) || defined (_WIN32)

class Component;
class TextInfo;
class TextDisplay;
class Window;
class Application;

class SmartObject;


class Component {

public:
	ImVec2 pos;

	Component(ImVec2 pos);

	virtual int render();

};



class TextInfo {
public:
	ImFont* font;
	ImColor* color;

	TextInfo(ImFont* font, ImColor* color);
};

class TextDisplay : public Component {
public:
	std::string text;

	TextInfo* info;

	Window* parent;

	TextDisplay(std::string text, ImVec2 pos, TextInfo * info, Window * parent);

	int render();


};







class SmartObject {

public:
	Component* parent;

	ImVec2 pos;

	bool focus;

	bool isMouseOver();

	bool isClicked();

};

class SmartText : public SmartObject {

public:
	std::string str;




};



class Window {
public:

	std::string title;

	ImVec2 pos;

	ImVec2 size;

	bool focused;

	std::vector<Component*> components;

	ImDrawList* drawList;

	Window(std::string title, ImVec2 pos, ImVec2 size);

	int addComponent(Component* component);

	void render();



};



class Application {

public:

	GLFWwindow* window;

	std::vector<Window*> windowList;


	Application(std::string label, ImVec2 size);

	int addWindow(Window* window);

	int render();
};








