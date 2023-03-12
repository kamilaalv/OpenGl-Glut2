/*********
   CTIS164 - Template Source Program
----------
STUDENT : Kamila Aliyeva
SECTION :002
HOMEWORK:2
----------
PROBLEMS:             kinda too many global variables
                      angle starts from zero each time, i thought intially that it should start from zero, but couple hours before the deadline, noticed it is not
----------
ADDITIONAL FEATURES:  cool background
                      very cool responsive stars
                      cool menu
                      game is responsive
                      hovering over game button makes it darker
                      rocket in menu moves in sinus function, it is responsive too
                      rocket also turns according to sinus
                      I wanted to create a game, but did not have enough time
                      rocket can move and turn in game mood
                      my bird was going to have animation i even have its second model called birdFlying but i did not have enough time to implement it

*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include<time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 400

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

#define MENU 1
#define TUTORIAL 2
#define GAME 3
#define GAME_OVER 4

#define MAX_FIRE 20
#define FIRE_RATE 8

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, space = false;
int  winWidth, winHeight; // current Window width and height

/* Scene */
int curScene = MENU;

/* Basic Graphics */
//pixel
double px = 3.0;

typedef struct {
    double x;
    double y;
}point_t;

typedef struct {
    int r;
    int g;
    int b;
}color_t;

typedef struct {
    point_t pos;
    double angle;
    double radius;
}player_t;

typedef struct
{
    point_t pos;
    color_t color;
    int radius;
}bird_t;

typedef struct
{
    point_t pos;
    double angle;
    bool active;
}fire_t;

/* Rocket */
player_t p = { {-400, 0}, 0, 3 };
point_t old;

/*Enemies*/
bird_t bird;

/* Fire */
fire_t fire = { {-2 * winWidth,-2 * winHeight}, 0, false };
fire_t fires[MAX_FIRE];
int fire_rate = 0;

/* Stars */
bool bgOn = false;
int bgCheck = 1;
int bgCounter = 0;

typedef struct {
    point_t basic[3500];
    int bright[3500];
    point_t med[117];
    point_t big[117];
}stars_t;

stars_t stars;



double b2x = -100, b2y = 100;

/* Tutorial */
bool tutOn = false;
int timeCounter = 60000;

/* Game */
bool gameOn = false;


/*PLAY BUTTON COLORS*/ //menu
color_t playCol{ 48, 181, 71 };


//
// to draw circle, center at (x,y)
// radius r
//

void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

void vertex(point_t P, point_t Tr, double angle) {
    double xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
    double yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
    glVertex2d(xp, yp);
}

/* Stars Display */
void createStars() {
    bgOn = true;
    int i;
    int maxX = winWidth / 2 - 5, maxY = winHeight / 2 - 5;
    int dens = winWidth * winHeight * 0.0003125;

    srand(time(NULL));
    for (i = 0; i < dens; i++)
    {
        stars.basic[i].x = rand() % (winWidth + 1 - 5) - maxX;
        stars.basic[i].y = rand() % (winHeight + 1 - 5) - maxY;
        stars.bright[i] = rand() % 4 + 1;
    }
    dens /= 30;
    for (i = 0; i < dens; i++)
    {
        stars.big[i].x = rand() % (winWidth + 1 - 5) - maxX;
        stars.big[i].y = rand() % (winHeight + 1 - 5) - maxY;
    }

    for (i = 0; i < dens; i++)
    {
        stars.med[i].x = rand() % (winWidth + 1 - 5) - maxX;
        stars.med[i].y = rand() % (winHeight + 1 - 5) - maxY;
    }

}

void bigStar(int x, int y)
{
    glColor3f(0, 0, 6);
    glRectf(x, y + 6, x + 2, y - 8);
    glRectf(x - 6, y, x + 8, y - 2);

    glColor3f(1, 1, 1);
    glRectf(x, y + 2, x + 2, y - 4);
    glRectf(x - 2, y, x + 4, y - 2);
}

void medStar(int x, int y)
{
    glColor3f(0, 0, 6);
    glRectf(x, y + 2, x + 1, y - 3);
    glRectf(x - 2, y, x + 3, y - 1);

    glColor3f(1, 1, 1);
    glRectf(x, y + 1, x + 1, y - 2);
    glRectf(x - 1, y, x + 2, y - 1);
}

void displayStars() {
    if (!bgOn)
        createStars();
    int i;
    int dens = winWidth * winHeight * 0.0003125;
    for (i = 0; i < dens; i++)
    {
        glColor3f(0, 0, 0.6);
        glRectf(stars.basic[i].x, stars.basic[i].y, stars.basic[i].x + stars.bright[i], stars.basic[i].y + stars.bright[i]);
    }
    dens /= 30;
    for (i = 0; i < dens; i++)
    {
        medStar(stars.big[i].x, stars.big[i].y);
        bigStar(stars.big[i].x, stars.big[i].y);
    }

}

/* Enimies */
void displayBird(double b1x, double b1y)
{
    //245 238 34    
    glColor3ub(bird.color.r, bird.color.g, bird.color.b);
    glRectf(b1x - px, b1y - px, b1x - 2 * px, b1y + 2 * px);
    glRectf(b1x - 2 * px, b1y, b1x - 3 * px, b1y + 3 * px);
    glRectf(b1x - 3 * px, b1y + px, b1x - 4 * px, b1y + 4 * px);
    glRectf(b1x - 4 * px, b1y, b1x - 5 * px, b1y + 4 * px);
    glRectf(b1x - 5 * px, b1y + 3 * px, b1x - 6 * px, b1y - 2 * px);
    glRectf(b1x - 7 * px, b1y + 2 * px, b1x - 6 * px, b1y - 4 * px);

    glRectf(b1x + 2 * px, b1y - px, b1x + 3 * px, b1y + 2 * px);
    glRectf(b1x + 3 * px, b1y, b1x + 4 * px, b1y + 3 * px);
    glRectf(b1x + 4 * px, b1y + px, b1x + 5 * px, b1y + 4 * px);
    glRectf(b1x + 5 * px, b1y, b1x + 6 * px, b1y + 4 * px);
    glRectf(b1x + 6 * px, b1y + 3 * px, b1x + 7 * px, b1y - 2 * px);
    glRectf(b1x + 7 * px, b1y + 2 * px, b1x + 8 * px, b1y - 4 * px);

    glColor3ub(164, 68, 184);
    glRectf(b1x - px, b1y - 2 * px, b1x + 2 * px, b1y + px);
    glRectf(b1x, b1y + px, b1x + px, b1y + 3 * px);
    glRectf(b1x - 3 * px, b1y - 2 * px, b1x - px, b1y - 3 * px);
    glRectf(b1x - 3 * px, b1y - 3 * px, b1x - 4 * px, b1y - 4 * px);
    glRectf(b1x + 2 * px, b1y - 2 * px, b1x + 4 * px, b1y - 3 * px);
    glRectf(b1x + 5 * px, b1y - 3 * px, b1x + 4 * px, b1y - 4 * px);

    glColor3ub(228, 180, 237);
    glRectf(b1x - px, b1y + 4 * px, b1x, b1y + 3 * px);
    glRectf(b1x + px, b1y + 4 * px, b1x + 2 * px, b1y + 3 * px);

}

void displayFlying()
{

    glColor3ub(245, 238, 34);
    glRectf(b2x, b2y - px, b2x + px, b2y - 2 * px);
    glRectf(b2x, b2y, b2x - 2 * px, b2y - 3 * px);
    glRectf(b2x - 2 * px, b2y - px, b2x - 4 * px, b2y - 4 * px);
    glRectf(b2x - 5 * px, b2y - 5 * px, b2x - 4 * px, b2y - 2 * px);
    glRectf(b2x - 6 * px, b2y - 6 * px, b2x - 5 * px, b2y - 3 * px);

    glRectf(b2x + 2 * px, b2y, b2x + 3 * px, b2y + px);
    glRectf(b2x + px, b2y + 3 * px, b2x + 4 * px, b2y + px);
    glRectf(b2x + 2 * px, b2y + 5 * px, b2x + 5 * px, b2y + 3 * px);
    glRectf(b2x + 3 * px, b2y + 5 * px, b2x + 6 * px, b2y + 6 * px);
    glRectf(b2x + 4 * px, b2y + 6 * px, b2x + 7 * px, b2y + 7 * px);

    glColor3ub(164, 68, 184);
    glRectf(b2x, b2y + px, b2x - px, b2y + 2 * px);
    glRectf(b2x, b2y + px, b2x + 2 * px, b2y - px);
    glRectf(b2x + px, b2y, b2x + 3 * px, b2y - 2 * px);
    glRectf(b2x + 2 * px, b2y - px, b2x + 4 * px, b2y - 3 * px);
    glRectf(b2x + 4 * px, b2y - 3 * px, b2x + 5 * px, b2y - 4 * px);
    glRectf(b2x + 6 * px, b2y - 5 * px, b2x + 5 * px, b2y - 4 * px);

    glColor3ub(228, 180, 237);
    glRectf(b2x - px, b2y + px, b2x - 2 * px, b2y + 2 * px);
    glRectf(b2x - px, b2y + 2 * px, b2x, b2y + 3 * px);
}

void displayFire() {
    glColor3f(1, 1, 1);
    double ang = fire.angle * D2R;
    glBegin(GL_QUADS);
    glBegin(GL_QUADS);
    vertex({ px + fire.pos.x, fire.pos.y + px * 2 }, fire.pos, ang);
    vertex({ -px + fire.pos.x, fire.pos.y + px * 2 }, fire.pos, ang);
    vertex({ -px + fire.pos.x, fire.pos.y - px * 2 }, fire.pos, ang);
    vertex({ px + fire.pos.x, fire.pos.y - px * 2 }, fire.pos, ang);
    glEnd();
}



void displayRocket() {
    glColor3f(1, 0, 0);
    double angle = p.angle * D2R;
    glBegin(GL_QUADS);
    vertex({ px, 6 * px }, p.pos, angle);
    vertex({ 0, 6 * px }, p.pos, angle);
    vertex({ 0, 8 * px }, p.pos, angle);
    vertex({ px, 8 * px }, p.pos, angle);
    glEnd();
    glBegin(GL_QUADS);
    vertex({ -px, 6 * px }, p.pos, angle);
    vertex({ 2 * px, 6 * px }, p.pos, angle);
    vertex({ 2 * px, -7 * px }, p.pos, angle);
    vertex({ -px, -7 * px }, p.pos, angle);
    glEnd();
    glBegin(GL_QUADS);
    vertex({ 3 * px, 3 * px }, p.pos, angle);
    vertex({ -2 * px, 3 * px }, p.pos, angle);
    vertex({ -2 * px, -3 * px }, p.pos, angle);
    vertex({ 3 * px, -3 * px }, p.pos, angle);
    glEnd();

    for (int i = 0; i < 3; i++)
    {
        glBegin(GL_QUADS);
        vertex({ -2 * px - i * px, 4 * px + i * px }, p.pos, angle);
        vertex({ -3 * px - i * px, 4 * px + i * px }, p.pos, angle);
        vertex({ -3 * px - i * px, px + i * px }, p.pos, angle);
        vertex({ -2 * px - i * px, px + i * px }, p.pos, angle);
        glEnd();
    }

    for (int i = 0; i < 3; i++)
    {
        glBegin(GL_QUADS);
        vertex({ 3 * px + i * px, 4 * px + i * px }, p.pos, angle);
        vertex({ 4 * px + i * px, 4 * px + i * px }, p.pos, angle);
        vertex({ 4 * px + i * px, px + i * px }, p.pos, angle);
        vertex({ 3 * px + i * px, px + i * px }, p.pos, angle);
        glEnd();
    }

    for (int i = 0; i < 4; i++)
    {
        glBegin(GL_QUADS);
        vertex({ -2 * px - i * px, -px - i * px }, p.pos, angle);
        vertex({ -3 * px - i * px, -px - i * px }, p.pos, angle);
        vertex({ -3 * px - i * px, -4 * px - i * px }, p.pos, angle);
        vertex({ -2 * px - i * px, -4 * px - i * px }, p.pos, angle);
        glEnd();
    }

    for (int i = 0; i < 4; i++)
    {
        glBegin(GL_QUADS);
        vertex({ 4 * px + i * px, -px - i * px }, p.pos, angle);
        vertex({ 3 * px + i * px, -px - i * px }, p.pos, angle);
        vertex({ 3 * px + i * px, -4 * px - i * px }, p.pos, angle);
        vertex({ 4 * px + i * px, -4 * px - i * px }, p.pos, angle);
        glEnd();
    }

    glColor3f(1, 1, 1);

    for (int i = 0; i < 2; i++)
    {
        glBegin(GL_QUADS);
        vertex({ -6 * px + 12 * i * px, 7 * px }, p.pos, angle);
        vertex({ -5 * px + 12 * i * px, 7 * px }, p.pos, angle);
        vertex({ -5 * px + 12 * i * px, 2 * px }, p.pos, angle);
        vertex({ -6 * px + 12 * i * px, 2 * px }, p.pos, angle);
        glEnd();
    }

    for (int i = 0; i < 2; i++)
    {
        glBegin(GL_QUADS);
        vertex({ -7 * px + 14 * i * px, -2 * px }, p.pos, angle);
        vertex({ -6 * px + 14 * i * px, -2 * px }, p.pos, angle);
        vertex({ -6 * px + 14 * i * px, -8 * px }, p.pos, angle);
        vertex({ -7 * px + 14 * i * px, -8 * px }, p.pos, angle);
        glEnd();
    }

    for (int i = 0; i < 3; i++)
    {
        glBegin(GL_QUADS);
        vertex({ -2 * px + 2 * i * px, -6 * px }, p.pos, angle);
        vertex({ -px + 2 * i * px, -6 * px }, p.pos, angle);
        vertex({ -px + 2 * i * px, -8 * px }, p.pos, angle);
        vertex({ -2 * px + 2 * i * px, -8 * px }, p.pos, angle);
        glEnd();
    }
    glBegin(GL_QUADS);
    vertex({ 0, 4 * px }, p.pos, angle);
    vertex({ px, 4 * px }, p.pos, angle);
    vertex({ px, 0 }, p.pos, angle);
    vertex({ 0, 0 }, p.pos, angle);
    glEnd();

    glColor3f(1, 1, 0);
    glBegin(GL_QUADS);
    vertex({ 0, 3 * px }, p.pos, angle);
    vertex({ px, 3 * px }, p.pos, angle);
    vertex({ px, px }, p.pos, angle);
    vertex({ 0, px }, p.pos, angle);
    glEnd();

    glBegin(GL_QUADS);
    vertex({ 0, -2 * px }, p.pos, angle);
    vertex({ px, -2 * px }, p.pos, angle);
    vertex({ px, -5 * px }, p.pos, angle);
    vertex({ 0, -5 * px }, p.pos, angle);
    glEnd();


}

void displayMenu()
{
    px = 2;
    int i, j;
    displayRocket();
    glColor3ub(255, 255, 255);
    glLineWidth(12);
    vprint2(-220, 0, 0.8, "PHOENIX");
    glLineWidth(2);
    vprint2(winWidth / 2 - 25, winHeight / 2 - 30, 0.2, "?");
    glColor3f(0.7, 0.8, 0.2);

    //play button
    glColor3ub(playCol.r, playCol.g, playCol.b);
    circle(0, -70, 40);
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLES);
    glVertex2f(-15, -90);
    glVertex2f(-15, -50);
    glVertex2f(25, -70);
    glEnd();
    vprint(-120, -winHeight / 2 + 20, GLUT_BITMAP_8_BY_13, "Press \"?\" to start the actual homework ");
}

void birdInit()
{
   
    bird.radius = 20;
    bird.pos.x = rand() % (winWidth - 31) - (winWidth / 2 - 31);
    bird.pos.y = rand() % (winHeight - 25) - (winHeight / 2 - 25);
    bird.color.r = rand() % 256;
    bird.color.b = rand() % 256;
    bird.color.g = rand() % 256;

}
void fireInit() {
    fire.pos = p.pos;
    fire.angle = p.angle + 90;
}


void tutorialInit()
{
    /*for (int i = 0; i < 8; i += 2) {
        bird[i / 2].pos.x = rand() % (winWidth -31) - (winWidth/2 - 31);
        bird[i / 2].pos.y = rand() % (winHeight - 25) - (winHeight / 2 - 25);
        speed[i / 2] = rand() % 4 + 1;
    }*/
    birdInit();
    fireInit();
    px = 3;
    p.pos.x = 0;
    p.pos.y = -100;
    p.angle = 0;
    

}
double findNewAngle() {
    double dy = bird.pos.y - p.pos.y;
    double dx = bird.pos.x - p.pos.x;
    double nAng = atan2(dy, dx) / D2R;
    if (nAng <= 90)
        return nAng + 270;
    else
        return nAng - 90;
}

void displayTutorial() {
    displayBird(bird.pos.x, bird.pos.y);
    displayRocket();
    //displayFire();
    glColor3f(1, 1, 1);
    if (fire.active) {
        circle(fire.pos.x, fire.pos.y, 3);
    }
    if (p.angle < 0)
        vprint(-20, -147, GLUT_BITMAP_8_BY_13, "angle = %.0f", p.angle + 360);
    else
        vprint(-30, -145, GLUT_BITMAP_8_BY_13, "angle = %.0f", p.angle);
    vprint(-40, -winHeight / 2, GLUT_BITMAP_8_BY_13, "time: %d", timeCounter / 1000);
    vprint(-100, -winHeight / 2 + 15, GLUT_BITMAP_8_BY_13, "press F1 to return to menu", timeCounter / 1000);

    
}

void gameInit() {
    px = 3.0;
    p.pos.x = 0;
    p.pos.y = -100;
    p.angle = 0;
}

void displayGame()
{
    //displayFire();
    glColor3f(1, 1, 1);
    if (fire.active) {
        circle(fire.pos.x, fire.pos.y, 3);
    }
 
    displayRocket();
}
//
// To display onto window using OpenGL commands
//
void display() {
    //
    // clear window to black
    //
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    displayStars();
    switch (curScene) {
    case MENU:
    {
        displayMenu();
        break;
    }
    case TUTORIAL:
    {
        if (!tutOn)
        {
            tutorialInit();
            tutOn = true;
        }
        displayTutorial();
        break;
    }
    case GAME:
    {
        if (!gameOn)
        {
            gameInit();
            gameOn = true;
        }
        displayGame();
        break;
    }
    case GAME_OVER:
    {
        break;
    }
    }



    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 32)
        space = true;
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    case GLUT_KEY_F1:
        if (curScene == TUTORIAL)
            curScene = MENU;
        break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//MENU IN PLAY BUTTON
bool insidePlay(int x, int y)
{
    double dist = sqrt((y + 70) * (y + 70) + x * x);
    if (dist <= 40)
        return true;
    return false;
}

bool insideQuestion(int x, int y)
{
    if ((x > winWidth / 2 - 25) && (y > winHeight / 2 - 30))
        return true;
    return false;
}
 
int findAvailableFire() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fires[i].active == false) return i;
    }
    return -1;
}

bool birdDead()
{
    double dx = fire.pos.x - bird.pos.x;
    double dy = fire.pos.y - bird.pos.y;
    return sqrt(dx * dx + dy * dy) <= bird.radius;

}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    x = x - winWidth / 2;
    y = winHeight / 2 - y;
    // Write your codes here.
    if (curScene == MENU)
    {
        if (insidePlay(x, y))
            curScene = GAME;
        else if (insideQuestion(x, y))
            curScene = TUTORIAL;
    }
    


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    bgCheck = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.
    x = x - winWidth / 2;
    y = winHeight / 2 - y;
    // Write your codes here.
    if (curScene == MENU)
    {
        if (insidePlay(x, y))
            playCol = { 36, 130, 61 };
        else
            playCol = { 48, 181, 71 };
    }


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void turnPlayer(player_t* p, double turn) {
    p->angle += turn;
    if (p->angle < 0) p->angle += 360;
    if (p->angle >= 360) p->angle -= 360;
}

void turnPlayerTut(player_t* p, double turn, double nAng) {
    if (nAng > 180)
        p->angle -= turn;
    else
        p->angle += turn;
}

void movePlayer(player_t* p, double speed) {
    point_t oldPos = p->pos;
    double dx = speed * cos((p->angle + 90) * D2R);
    double dy = speed * sin((p->angle + 90) * D2R);

    p->pos = { p->pos.x + dx, p->pos.y + dy };
    if (p->pos.x > (winWidth / 2 - p->radius) || p->pos.x < (-winWidth / 2 + p->radius) || p->pos.y >(winHeight / 2 - p->radius) || p->pos.y < (-winHeight / 2 + p->radius)) {
        p->pos = oldPos;
    }
}





double fx(double x)
{
    double period = 6.4 / winWidth, amp = -winHeight * 0.475;
    return -amp * sin(x * period);
}


#if TIMER_ON == 1
void onTimer(int v) {
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.

    //changing background stars

    if (((winWidth > 800 || winHeight > 400) && bgCheck == 1) || bgCounter == 126)
    {
        bgCounter = 0;
        bgOn = false;
        bgCheck = 0;
    }
    bgCounter++;

    switch (curScene) {
    case MENU:
    {

        old = p.pos;
        p.pos.x += 1.5;
        if (p.pos.x > winWidth / 2)
            p.pos.x = -winWidth / 2;
        p.pos.y = fx(p.pos.x);
        p.angle = asin((p.pos.y - old.y) / sqrt((p.pos.y - old.y) * (p.pos.y - old.y) + (p.pos.x - old.x) * (p.pos.x - old.x))) / D2R - 90;
        break;
    }
    case TUTORIAL:
    {
        if (timeCounter > 0) {
            double nAng = findNewAngle();
            int ang = (nAng > 180) ? p.angle + 360 : p.angle;
            if (ang != int(nAng))
                turnPlayerTut(&p, 1, nAng);

            else {
                if (!fire.active)
                {
                    fireInit();
                    fire.active = true;
                }
                if (fire.active)
                {
                    fire.pos.x += 4 * cos(fire.angle * D2R);
                    fire.pos.y += 4 * sin(fire.angle * D2R);

                    if (birdDead())
                    {
                        fire.active = false;
                        tutorialInit();
                    }
                }
            }
            timeCounter -= TIMER_PERIOD;

            if (timeCounter == 0)
            {
                curScene = MENU;
            }
        }
        break;
    }
    case GAME:
    {
        if (right) {
            turnPlayer(&p, -2);
        }

        // turn the player counter-clockwise direction
        if (left) {
            turnPlayer(&p, 2);
        }

        if (up) {
            movePlayer(&p, 5);
        }

        // move backward
        if (down) {
            movePlayer(&p, -5);
        }
        break;
    }
    case GAME_OVER:
    {
        break;
    }
    }



    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("Kamila : Phoenix Game");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}