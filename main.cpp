#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// =========================================================
// ENUMS
// =========================================================

enum ObjectType
{
    NORMAL_EGG,
    BLUE_EGG,
    GOLDEN_EGG,
    POOP,
    BIG_BASKET
};

enum GameState
{
    MENU,
    PLAYING,
    PAUSED,
    HELP,
    GAME_OVER
};

// =========================================================
// STRUCT
// =========================================================

struct FallingObject
{
    float x, y;
    int type;
    float speed;
};

// =========================================================
// GLOBAL VARIABLES
// =========================================================

GameState currentState = MENU;

vector<FallingObject> objects;

float basketX = 0;
float basketWidth = 25;

float chickenX[3] = {-70, 0, 70};
float chickenY[3] = {75, 45, 15};

bool chickenDir[3] = {true, false, true};

float windForce = 0;
bool windRight = true;

float rotationAngle = 0;

int score = 0;
int highScore = 0;
int gameTime = 60;

int basketPowerTimer = 0;

// =========================================================
// TEXT
// =========================================================

void drawText(float x, float y, string text)
{
    glRasterPos2f(x, y);

    for(char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

// =========================================================
// CIRCLE
// =========================================================

void drawCircle(float cx, float cy, float r)
{
    glBegin(GL_POLYGON);

    for(int i=0; i<360; i++)
    {
        float theta = i * 3.1416f / 180;

        float x = r * cos(theta);
        float y = r * sin(theta);

        glVertex2f(cx + x, cy + y);
    }

    glEnd();
}

// =========================================================
// CHICKEN
// =========================================================

void drawChicken(float x, float y)
{
    // bamboo stick
    glColor3f(0.7, 0.5, 0.2);

    glLineWidth(5);

    glBegin(GL_LINES);
    glVertex2f(-100, y-7);
    glVertex2f(100, y-7);
    glEnd();

    // body
    glColor3f(1,1,1);
    drawCircle(x, y, 5);

    // head
    drawCircle(x+5, y+5, 3);

    // eye
    glColor3f(0,0,0);
    drawCircle(x+6, y+6, 0.5);

    // beak
    glColor3f(1,0.5,0);

    glBegin(GL_TRIANGLES);
    glVertex2f(x+8, y+5);
    glVertex2f(x+11, y+6);
    glVertex2f(x+8, y+7);
    glEnd();
}

// =========================================================
// BASKET
// =========================================================

void drawBasket()
{
    glColor3f(0.5, 0.2, 0);

    glBegin(GL_POLYGON);

    glVertex2f(basketX - basketWidth/2, -95);
    glVertex2f(basketX + basketWidth/2, -95);
    glVertex2f(basketX + basketWidth/2 - 4, -80);
    glVertex2f(basketX - basketWidth/2 + 4, -80);

    glEnd();
}

// =========================================================
// OBJECTS
// =========================================================

void drawObject(const FallingObject& obj)
{
    if(obj.type == NORMAL_EGG)
        glColor3f(1,1,1);

    else if(obj.type == BLUE_EGG)
        glColor3f(0,0,1);

    else if(obj.type == GOLDEN_EGG)
        glColor3f(1,0.8,0);

    else if(obj.type == POOP)
        glColor3f(0.3,0.2,0);

    else if(obj.type == BIG_BASKET)
        glColor3f(0,1,0);

    glPushMatrix();

    glTranslatef(obj.x, obj.y, 0);

    if(obj.type == GOLDEN_EGG)
        glRotatef(rotationAngle, 0,0,1);

    drawCircle(0,0,3);

    glPopMatrix();
}

// =========================================================
// MENU
// =========================================================

void drawMenu()
{
    glColor3f(1,1,0);

    drawText(-30, 50, "CATCH THE EGG DELUXE");

    glColor3f(1,1,1);

    drawText(-20, 20, "1. Start Game");
    drawText(-20, 0, "2. Help");
    drawText(-20, -20, "3. Exit");
}

// =========================================================
// HELP SCREEN
// =========================================================

void drawHelp()
{
    glColor3f(1,1,1);

    drawText(-40, 50, "HELP MENU");

    drawText(-80, 20, "LEFT / RIGHT Arrow = Move Basket");
    drawText(-80, 0, "Mouse = Move Basket");
    drawText(-80, -20, "P = Pause Game");
    drawText(-80, -40, "Catch Eggs, Avoid Poop");

    drawText(-60, -70, "Press M for Menu");
}

// =========================================================
// PAUSE
// =========================================================

void drawPause()
{
    glColor3f(1,1,0);

    drawText(-15, 10, "GAME PAUSED");

    glColor3f(1,1,1);

    drawText(-25, -10, "Press R to Resume");
}

// =========================================================
// SPAWN OBJECTS
// =========================================================

void spawnObject()
{
    if(rand()%20 != 0)
        return;

    FallingObject obj;

    int c = rand()%3;

    obj.x = chickenX[c];
    obj.y = chickenY[c] - 8;

    int r = rand()%100;

    if(r < 50)
        obj.type = NORMAL_EGG;

    else if(r < 70)
        obj.type = BLUE_EGG;

    else if(r < 85)
        obj.type = GOLDEN_EGG;

    else if(r < 95)
        obj.type = POOP;

    else
        obj.type = BIG_BASKET;

    obj.speed = 1 + rand()%2;

    objects.push_back(obj);
}

// =========================================================
// COLLISION
// =========================================================

void checkCollision()
{
    for(auto it = objects.begin(); it != objects.end(); )
    {
        bool caught =
        (
            it->y < -80 &&
            it->x > basketX - basketWidth/2 &&
            it->x < basketX + basketWidth/2
        );

        if(caught)
        {
            if(it->type == NORMAL_EGG)
                score += 1;

            else if(it->type == BLUE_EGG)
                score += 5;

            else if(it->type == GOLDEN_EGG)
                score += 10;

            else if(it->type == POOP)
                score = max(0, score - 10);

            else if(it->type == BIG_BASKET)
            {
                basketWidth = 45;
                basketPowerTimer = 10;
            }

            it = objects.erase(it);
        }
        else if(it->y < -100)
        {
            it = objects.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// =========================================================
// DISPLAY
// =========================================================

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    if(currentState == MENU)
    {
        drawMenu();
    }

    else if(currentState == HELP)
    {
        drawHelp();
    }

    else if(currentState == PAUSED)
    {
        drawPause();
    }

    else if(currentState == PLAYING)
    {
        // chickens
        for(int i=0; i<3; i++)
            drawChicken(chickenX[i], chickenY[i]);

        // basket
        drawBasket();

        // objects
        for(auto &obj : objects)
            drawObject(obj);

        glColor3f(1,1,1);

        drawText(-95, 92, "Score: " + to_string(score));
        drawText(60, 92, "Time: " + to_string(gameTime));

        if(windForce > 0)
            drawText(-10, 80, "Wind -->");
        else
            drawText(-10, 80, "Wind <--");
    }

    else if(currentState == GAME_OVER)
    {
        glColor3f(1,0,0);

        drawText(-20, 20, "GAME OVER");

        glColor3f(1,1,1);

        drawText(-25, 0, "Final Score: " + to_string(score));
        drawText(-25, -20, "High Score: " + to_string(highScore));

        drawText(-40, -60, "Press M for Menu");
    }

    glutSwapBuffers();
}

// =========================================================
// UPDATE
// =========================================================

void update(int value)
{
    if(currentState == PLAYING)
    {
        // move chickens
        for(int i=0; i<3; i++)
        {
            if(chickenDir[i])
                chickenX[i] += 0.5f + i*0.1f;
            else
                chickenX[i] -= 0.5f + i*0.1f;

            if(chickenX[i] > 85)
                chickenDir[i] = false;

            if(chickenX[i] < -85)
                chickenDir[i] = true;
        }

        // wind
        if(windRight)
            windForce += 0.002f;
        else
            windForce -= 0.002f;

        if(windForce > 0.5f)
            windRight = false;

        if(windForce < -0.5f)
            windRight = true;

        // rotation
        rotationAngle += 2;

        // spawn
        spawnObject();

        // move objects
        for(auto &obj : objects)
        {
            obj.y -= obj.speed;
            obj.x += windForce;
        }

        checkCollision();
    }

    glutPostRedisplay();

    glutTimerFunc(16, update, 0);
}

// =========================================================
// TIMER
// =========================================================

void timer(int value)
{
    if(currentState == PLAYING)
    {
        gameTime--;

        if(basketPowerTimer > 0)
        {
            basketPowerTimer--;

            if(basketPowerTimer == 0)
                basketWidth = 25;
        }

        if(gameTime <= 0)
        {
            currentState = GAME_OVER;

            if(score > highScore)
                highScore = score;
        }
        else
        {
            glutTimerFunc(1000, timer, 0);
        }
    }
}

// =========================================================
// KEYBOARD
// =========================================================

void keyboard(unsigned char key, int x, int y)
{
    if(currentState == MENU)
    {
        if(key == '1')
        {
            score = 0;
            gameTime = 60;
            basketWidth = 25;

            objects.clear();

            currentState = PLAYING;

            glutTimerFunc(1000, timer, 0);
        }

        else if(key == '2')
        {
            currentState = HELP;
        }

        else if(key == '3')
        {
            exit(0);
        }
    }

    else if(currentState == HELP)
    {
        if(key == 'm' || key == 'M')
            currentState = MENU;
    }

    else if(currentState == PLAYING)
    {
        if(key == 'p' || key == 'P')
            currentState = PAUSED;
    }

    else if(currentState == PAUSED)
    {
        if(key == 'r' || key == 'R')
        {
            currentState = PLAYING;
            glutTimerFunc(1000, timer, 0);
        }
    }

    else if(currentState == GAME_OVER)
    {
        if(key == 'm' || key == 'M')
            currentState = MENU;
    }
}

// =========================================================
// SPECIAL KEYBOARD
// =========================================================

void specialKeyboard(int key, int x, int y)
{
    if(currentState != PLAYING)
        return;

    if(key == GLUT_KEY_LEFT)
        basketX -= 8;

    if(key == GLUT_KEY_RIGHT)
        basketX += 8;

    if(basketX < -90)
        basketX = -90;

    if(basketX > 90)
        basketX = 90;
}

// =========================================================
// MOUSE
// =========================================================

void mouseMotion(int x, int y)
{
    if(currentState != PLAYING)
        return;

    basketX = (float(x)/1000.0f)*200.0f - 100.0f;

    if(basketX < -90)
        basketX = -90;

    if(basketX > 90)
        basketX = 90;
}

// =========================================================
// INIT
// =========================================================

void init()
{
    glClearColor(0.4f, 0.7f, 1.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(-100, 100, -100, 100);

    glMatrixMode(GL_MODELVIEW);
}

// =========================================================
// MAIN
// =========================================================

int main(int argc, char** argv)
{
    srand(time(0));

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(1000, 700);

    glutCreateWindow("Catch The Egg Deluxe");

    init();

    glutDisplayFunc(display);

    glutKeyboardFunc(keyboard);

    glutSpecialFunc(specialKeyboard);

    glutPassiveMotionFunc(mouseMotion);

    glutTimerFunc(0, update, 0);

    glutMainLoop();

    return 0;
}
