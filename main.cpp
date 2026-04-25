#include <GL/glut.h>
#include <iostream>
#include <vector>

float basketX = 0, eggX = 0, eggY = 450, chickenX = 0;
int score = 0, timeRemaining = 60; // Simple timer
bool eggActive = true;

void drawRect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + w, y);
    glVertex2f(x + w, y + h); glVertex2f(x, y + h);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw Chicken (Simple Block)
    glColor3f(1, 0, 0); // Red
    drawRect(chickenX, 460, 40, 30);

    // Draw Egg
    if(eggActive) {
        glColor3f(1, 1, 0); // Yellow (Normal Egg)
        drawRect(eggX, eggY, 15, 20);
    }

    // Draw Basket
    glColor3f(0.5, 0.35, 0.05); // Brown
    drawRect(basketX, 10, 60, 20);

    glutSwapBuffers();
}

void update(int value) {
    // Chicken Movement
    static float dir = 2.0f;
    chickenX += dir;
    if(chickenX > 460 || chickenX < 0) dir *= -1;

    // Egg Physics
    eggY -= 3.0f; // Falling speed

    // Collision Detection
    if(eggY < 30 && eggX > basketX && eggX < basketX + 60) {
        score++;
        eggY = 450; eggX = chickenX;
    } else if (eggY < 0) { // Missed egg
        eggY = 450; eggX = chickenX;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // ~60 FPS
}

void keyboard(unsigned char key, int x, int y) {
    if(key == 'a') basketX -= 15; // Keyboard control
    if(key == 'd') basketX += 15;
    if(key == 27) exit(0); // Exit
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Catch The Eggs");
    gluOrtho2D(0, 500, 0, 500);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}
