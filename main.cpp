
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <vector>
#include <assert.h>
using namespace std;
using namespace glm;

#define PI 3.14159

GLFWwindow *window;
int w, h;
double mouseX, mouseY;

vector<vec2> controls = {vec2(-0.5f, -0.25f), vec2(0.f, -0.25f), vec2(0.25f, 0.f), vec2(0.f, 0.25f), vec2(0.5f, 0.25f), vec2(0.5f, 0.f)};
float cRadius = 0.01f;
int selected = -1;

vector<float> knots = {0.f, 0.f, 0.f, 1.f/3.f, 2.f/3.f, 1.f, 1.f, 1.f};
int order = 3;
float uParam = 0.f;

float getDelta(float u){

}

vec2 findPosAt(float u){
	vector<vec2> output;

	//find the delta value
	int delta = -1;
	for (int i = 0; i < (controls.size() - 1 + order); i++){
		if (u >= 1.f){
			delta = controls.size() - 2;
			u = 1.f;
		}
		else if (u < 0.f){
			delta = order;
			u = 0.f;
		}
		else if ((u >= knots[i]) && (u < knots[i+1])){
			delta = i;
			break;
		}
	}
	assert(delta != -1);

	//efficient algorithm follows:
	for (int i = 0; i <= order - 1; i++){
		output.push_back(controls[delta - i]);
	}
	for (int r = order; r >= 2; r--){
		int i = delta;
		for (int s = 0; s <= r-2; s++){
			float omega = (u - knots[i]) / (knots[i+r-1] - knots[i]);
			output[s] = (omega * output[s]) + ((1 - omega) * output[s+1]);
			i--;
		}
	}
	return output[0];
}

void render () {
	glEnable (GL_DEPTH_TEST);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Functions for changing transformation matrix
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glTranslatef (0.f, 0.f, 0.f);
	glRotatef (0.f, 0.f, 0.f, 1.f);
	glScalef (1.f, 1.f, 1.f);

	//Functions for changing projection matrix
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho (-1, 1, -1, 1, -1, 1);
	//gluPerspective (fov, aspect ratio, near plane, far plane)
	//glFrustum

	//Draws the points as little circles
	for (int i = 0; i < controls.size() - 1; i++){
		glBegin (GL_TRIANGLE_STRIP); //GL_LINE_STRIP, GL_POINTS, GL_QUADS, etc...
			glColor3f(1.f, 1.f, 1.f);
			for (float t = 0.f; t < 2*PI; t += 0.01f){
				vec2 circle = vec2(cRadius*cos(t), cRadius*sin(t));
				glVertex2f(controls[i].x + circle.x, controls[i].y + circle.y);
				glVertex2f(controls[i].x, controls[i].y);
			}
		glEnd ();
	}

	glBegin (GL_TRIANGLE_STRIP); //GL_LINE_STRIP, GL_POINTS, GL_QUADS, etc...
		glColor3f(0.f, 0.f, 1.f);
		for (float t = 0.f; t < 2*PI; t += 0.01f){
			int index = controls.size() - 1;
			vec2 circle = vec2(cRadius*cos(t), cRadius*sin(t));
			glVertex2f(controls[index].x + circle.x, controls[index].y + circle.y);
			glVertex2f(controls[index].x, controls[index].y);
		}
	glEnd ();

	glBegin (GL_LINE_STRIP);
	glColor3f(1.f, 1.f, 1.f);
	for(float u = knots[order - 1]; u <= knots[controls.size() + 1]; u += (0.001)){
		vec2 posVec = findPosAt(u);
		glVertex2f(posVec.x, posVec.y);
	}
	glEnd();

	glBegin (GL_LINES);
	vec2 uPos = findPosAt(uParam);
	glColor3f(1.f, 0.f, 0.f);
	glVertex2f(uPos.x - 2*cRadius, uPos.y + 2*cRadius);
	glVertex2f(uPos.x + 2*cRadius, uPos.y - 2*cRadius);
	glVertex2f(uPos.x + 2*cRadius, uPos.y + 2*cRadius);
	glVertex2f(uPos.x - 2*cRadius, uPos.y - 2*cRadius);
	glEnd();

	for (int i = 0; i < knots.size(); i++){
		glBegin (GL_TRIANGLE_STRIP); //GL_LINE_STRIP, GL_POINTS, GL_QUADS, etc...
			glColor3f(1.f, 0.f, 0.f);
			for (float t = 0.f; t < 2*PI; t += 0.01f){
				vec2 circle = vec2(cRadius*cos(t), cRadius*sin(t));
				vec2 knotPos = findPosAt(knots[i]);
				glVertex2f(knotPos.x + circle.x, knotPos.y + circle.y);
				glVertex2f(knotPos.x, knotPos.y);
			}
		glEnd();
	}
}

void buildKnots(){
	int numPoints = controls.size() - 1;
	/*	the denominator for the uniform step size is (numPoints - order + 2).
		if numPoints < order - 1 , then step size will be negative or have denominator 0.
		to avoid this we assert that numPoints > order - 2 */
	assert(numPoints > (order - 2));

	//add knots from current m and k values
	float stepSize = 1.f / float(numPoints - order + 1);
	for (int iter = order; iter <= numPoints; iter++){
		float value = knots[iter - 1] + stepSize;
		knots.insert(knots.begin() + iter, value);
	}

	//delete knots from previous knot sequence
	while (knots.size() > (numPoints + order)){
		knots.erase(knots.begin() + (numPoints + 1));
	}

	/*prints out the knot values
	for (int iter = 0; iter < knots.size(); iter++){
		cout << knots[iter] << " ";
	}
	cout << endl;
	*/
}

void keyboard (GLFWwindow *sender, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_UP && action == GLFW_PRESS){
		if (order + 1 <= controls.size() - 1){
			order++;
			knots.insert(knots.begin(), 0.f);
			knots.push_back(1.f);
			buildKnots();
		}
		else {
			cout << "The order of the curve is too big for the number of control points." << endl;
		}
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
		if (order > 2){
			order--;
			knots.erase(knots.begin());
			knots.pop_back();
			buildKnots();
		}
		else {
			cout << "The order of the curve must be bigger than 1." << endl;
		}
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
		if (uParam < knots[controls.size()]){
			uParam += 0.05;
			if (uParam > 1.f){
				uParam = 1.f;
			}
		}
		else {
			cout << "Highest u value reached." << endl;
		}
		//cout << uParam << endl;
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
		if (uParam > knots[order - 1]){
			uParam -= 0.05;
			if (uParam < 0.f){
				uParam = 0.f;
			}
		}
		else {
			cout << "Lowest u value reached." << endl;
		}
		//cout << uParam << endl;
	}
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS){
			cout << "Order: " << order << endl;
			cout << "Number of Points: " << controls.size() - 1 << " (Value of N: " << controls.size() - 2 << ")" << endl;
			cout << "u Value: " << uParam << endl;
	}
}

bool canMove = false;

void mouseClick (GLFWwindow *sender, int button, int action, int mods) {
	selected = -1;
	canMove = false;
	if (action == GLFW_PRESS){
		for (int i = 0; i < controls.size(); i++){
			if ((abs(controls[i].x - mouseX) <= cRadius) && (abs(controls[i].y - mouseY) <= cRadius)){
				selected = i;
			}
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (selected == -1){
				controls.push_back(vec2(mouseX, mouseY));
				cout << "New control point: " << controls.size() - 1 << endl;
			}
			else{
				if (selected == controls.size() - 1){
					cout << "Selected control point: Next Point to be Added" << endl;
				}
				else {
					cout << "Selected control point: " << selected + 1 << endl;
				}
			}
			canMove = true;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT){
			int ptsCheck = controls.size() - 3;
			int ordCheck = order - 2;
			if ((ptsCheck > ordCheck) && (ptsCheck > 0)){
				if (selected != -1){
					controls.erase(controls.begin() + selected);
					cout << "Deleted control point: " << selected << endl;
				}
			}
			else {
				cout << "There are too few control points for the current order of the curve." << endl;
			}
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			if (selected != -1){
				controls.insert(controls.begin() + selected, vec2(mouseX, mouseY));
				cout << "New control point: " << selected << endl;
			}
			canMove = true;
		}
		buildKnots();
	}
}

void mousePos (GLFWwindow *sender, double x, double y) {
	mouseX = (2 * x / w) - 1;
	mouseY = -((2 * y / h) - 1);
	if (selected != -1 && canMove){
		controls[selected].x = mouseX;
		controls[selected].y = mouseY;
	}
}

int main() {
	if (!glfwInit())
		return 1;

	window = glfwCreateWindow (640, 640, "My Window", NULL, NULL);
	if (!window)
		return 1;

	glfwMakeContextCurrent (window);
	glfwSetKeyCallback (window, keyboard);
	glfwSetMouseButtonCallback (window, mouseClick);
	glfwSetCursorPosCallback (window, mousePos);
	while (!glfwWindowShouldClose (window)) {
		glfwGetFramebufferSize (window, &w, &h);
		glViewport (0, 0, w, h);

		render ();

		glfwSwapBuffers (window);
		glfwPollEvents();
	}

	glfwDestroyWindow (window);
	glfwTerminate();
	return 0;
}

