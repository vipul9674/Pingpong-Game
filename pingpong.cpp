/Including libraries
#include <string>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <sstream>
#include <math.h>
#include <gl/GL.h>
#include <gl/glu.h>
#include <gl/glut.h>
using namespace std;
//Keycodes for W and S
#define VK_W 0x57
#define VK_S 0x53
//Window size and update rate (60fps) the number of times the update function is called
int interval = 1000 / 60;
int width = 1000;
int height = 400;
// Rackets dimensions in general
int racket_width = 10;
int racket_height = 80;
int racket_speed = 3;
// Left racket position
float racket_left_x = 10.0f;
float racket_left_y = 50.0f;
// Right racket position
float racket_right_x = width - racket_width - 10;
float racket_right_y = 50;
// Ball
float ball_pos_x = width / 2;
float ball_pos_y = height / 2;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;
int ball_size = 8;
int ball_speed = 2;
// Score
6
int score_left = 0;
int score_right = 0;
int level = 1;
//Reshape the screen
void reshape(int w, int h) {
glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}
//Change background color
void init() {
glClearColor(1,1,0,0); //color of window
}
//Function to print text character by character
void drawText(float x, float y, std::string text) {
glRasterPos2f(x, y);
for (int i = 0; i < text.length(); i++) {
glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);
}
}
// Draw a rectangle
void drawRect(float x, float y, float width, float height) {
glBegin(GL_QUADS);
glVertex2f(x, y);
glVertex2f(x + width, y);
glVertex2f(x + width, y + height);
glVertex2f(x, y + height);
glEnd();
}
//The draw function draws everything at the screen
void draw()
{
// Clear (has to be done at the beginning)
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glLoadIdentity();
// Draw rackets
drawRect(racket_left_x, racket_left_y, racket_width, racket_height);
drawRect(racket_right_x, racket_right_y, racket_width, racket_height);
// Draw ball at the centre of the screen
drawRect(ball_pos_x - ball_size / 2, ball_pos_y - ball_size / 2, ball_size, ball_size);
7
//Score
drawText(width / 2 - 10, height - 15,"(Player 1) " + to_string(score_left) + ":" +
to_string(score_right) + " (Player 2)");
drawText(width / 2 - 380, height - 15, to_string(level));
drawText(width / 2 - 450, height - 15, "Level - ");
// Swap buffers (has to be done at the end)
glFlush(); //display frame buffer
glutSwapBuffers();
}
// Configurations for 2D
void enable2D(int width, int height)
{
glViewport(0, 0, width, height);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
}
//Keyboard functions
//Left player can use W and S for up and down
//Right player can use UpArrow / DownArrow keys for up and down
void keyboard() {
// left racket
if (GetAsyncKeyState(VK_W)) racket_left_y += racket_speed;
if (GetAsyncKeyState(VK_S)) racket_left_y -= racket_speed;
// right racket
if (GetAsyncKeyState(VK_UP)) racket_right_y += racket_speed;
if (GetAsyncKeyState(VK_DOWN)) racket_right_y -= racket_speed;
}
/*We need it because we modified the ball_dir_x and ball_dir_y variables before.
But in order to have the ball flying at the same speed all the time,
the sum of both of those variables should always be exactly 1.
That's pretty much what vec2_norm does, it just sets the length of a vector to one.*/
void vec2_norm(float& x, float& y) {
// sets a vectors length to 1 (which means that x + y == 1)
float length = sqrt((x * x) + (y * y));
if (length != 0.0f) {
length = 1.0f / length;
8
x *= length;
y *= length;
}
}
//Moves ball in a new direction with each update call
//Updating for each time it hits something
// Every time we just check the ball's x and y values to see if it's inside the left racket,
//inside the right racket, above the top wall, below the bottom wall, left of the left wall or right of
the right wall.
//Then as explained above, we change directions, increase scores and so on.
void updateBall()
{
// fly a bit
ball_pos_x += ball_dir_x * ball_speed;
ball_pos_y += ball_dir_y * ball_speed;
// hit by left racket?
if (ball_pos_x < racket_left_x + racket_width &&
ball_pos_x > racket_left_x &&
ball_pos_y < racket_left_y + racket_height &&
ball_pos_y > racket_left_y) {
// set fly direction depending on where it hit the racket
// (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
float t = ((ball_pos_y - racket_left_y) / racket_height) - 0.5f;
ball_dir_x = fabs(ball_dir_x); // force it to be positive
ball_dir_y = t;
}
// hit by right racket?
if (ball_pos_x > racket_right_x &&
ball_pos_x < racket_right_x + racket_width &&
ball_pos_y < racket_right_y + racket_height &&
ball_pos_y > racket_right_y) {
// set fly direction depending on where it hit the racket
// (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
float t = ((ball_pos_y - racket_right_y) / racket_height) - 0.5f;
ball_dir_x = -fabs(ball_dir_x); // force it to be negative
ball_dir_y = t;
}
// hit left wall?
if (ball_pos_x < 0)
{
++score_right;
if (score_right > 2 && score_right <= 5)
9
{
ball_speed = 7;
}
if (score_right > 5 && score_right < 20)
{
ball_speed = 12;
}
if (score_right == 20)
{
cout << "Game Over ! Player 2 won";
exit(0);
}
ball_pos_x = width / 2;
ball_pos_y = height / 2;
ball_dir_x = fabs(ball_dir_x); // force it to be positive
ball_dir_y = 0;
}
// hit right wall?
if (ball_pos_x > width)
{
++score_left;
if (score_left > 2 && score_left <= 5)
{
ball_speed = 5;
level = 2;
}
if (score_left > 5 && score_left < 20)
{
ball_speed = 8;
level = 3;
}
if (score_left == 20)
{
cout << "Game Over ! Player 1 won";
exit(0);
}
ball_pos_x = width / 2;
ball_pos_y = height / 2;
ball_dir_x = -fabs(ball_dir_x); // force it to be negative
ball_dir_y = 0;
}
// hit top wall?
if (ball_pos_y > height)
{
10
ball_dir_y = -fabs(ball_dir_y); // force it to be negative
}
// hit bottom wall?
if (ball_pos_y < 0)
{
ball_dir_y = fabs(ball_dir_y); // force it to be positive
}
// make sure that length of dir stays at 1
vec2_norm(ball_dir_x, ball_dir_y);
}
//The update function calculates the ball and racket movements and also keyhandlind
void update(int value)
{
// input handling
keyboard();
// update ball
updateBall();
// Call update() again in 'interval' milliseconds
glutTimerFunc(interval, update, 0);
// Redisplay frame
glutPostRedisplay();
}
int main(int argc, char** argv) {
glutInit(&argc, argv); //Initialize glut library
glutInitDisplayMode(GLUT_RGB); //Initialize display mode
glutInitWindowPosition(200, 100); //Window Position
glutInitWindowSize(width, height); //Window Size
glutCreateWindow("Ping Pong"); //Window Name
glutDisplayFunc(draw); //Calling the draw function
glutTimerFunc(interval, update, 0);
glutReshapeFunc(reshape);
init();
enable2D(width, height);
glColor3f(1,0,0);
glutMainLoop();
}