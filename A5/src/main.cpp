#include <functional>
#include <iostream>
#include <random>
#include <stdio.h>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
using point = tuple<float, float>;

float alfa = 0.0f, beta = 0.5f, radius = 100.0f;
float camX, camY, camZ;

void spherical2Cartesian()
{
    camX = radius * cos(beta) * sin(alfa);
    camY = radius * sin(beta);
    camZ = radius * cos(beta) * cos(alfa);
}

void changeSize(int w, int h)
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if (h == 0)
        h = 1;

    // compute window's aspect ratio
    float ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}
bool in_circle(point p)
{
    return sqrt(pow(std::get<0>(p), 2) + pow(std::get<1>(p), 2)) < 50.0;
}

vector<point> gen_tree_pos()
{
    vector<point> trees;
    default_random_engine generator(42);
    uniform_int_distribution<int> distribution(-10000, 10000);
    auto rng = bind(distribution, generator);
    while (trees.size() != 1000) {
        float x = rng() / 100.0f;
        float y = rng() / 100.0f;
        point p = std::make_tuple(x, y);
        if (!in_circle(p)) {
            trees.push_back(p);
        }
    }
    return trees;
}

void draw_cowboys()
{
    static float _alpha = 0.0;
    const float delta = (2 * M_PI) / 8;
    float alpha = _alpha;
    glColor3f(0, 0, 1);
    for (int i = 0; i < 8; i++) {
        float x = 15.0 * sin(alpha);
        float z = 15.0 * cos(alpha);
        glPushMatrix();
        glTranslatef(x, 0.8, z);
        glRotatef((alpha / M_PI) * 180 - 90, 0, 1, 0);
        glutSolidTeapot(1);
        glPopMatrix();
        alpha += delta;
        if (alpha > (2 * M_PI))
            alpha -= (2 * M_PI);
    }
    _alpha -= 0.01;
    if (_alpha > (2 * M_PI))
        _alpha -= (2 * M_PI);
}

void draw_indians()
{
    static float _alpha = 0.0;
    const int n_indians = 16;
    const float delta = (2 * M_PI) / n_indians;
    float alpha = _alpha;
    glColor3f(1, 0, 0);
    for (int i = 0; i < n_indians; i++) {
        float x = 35.0 * sin(alpha);
        float z = 35.0 * cos(alpha);
        glPushMatrix();
        glTranslatef(x, 0.8, z);
        glRotatef((alpha / M_PI) * 180, 0, 1, 0);
        glutSolidTeapot(1);
        glPopMatrix();
        alpha += delta;
        if (alpha > (2 * M_PI))
            alpha -= (2 * M_PI);
    }
    _alpha += 0.01;
    if (_alpha > (2 * M_PI))
        _alpha -= (2 * M_PI);
}
void draw_tree(point p)
{
    glPushMatrix();
    glTranslatef(std::get<0>(p), 0, std::get<1>(p));
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.4, 0.2, 0);
    // tronco
    glutSolidCone(0.5, 2, 10, 10);
    glTranslatef(0, 0, 1.2);
    glColor3f(0, 0.3, 0);
    // folhas
    glutSolidCone(1, 3, 10, 10);
    glPopMatrix();
}

void renderScene(void)
{
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    gluLookAt(camX, camY, camZ,
        0.0, 0.0, 0.0,
        0.0f, 1.0f, 0.0f);

    glColor3f(0.2f, 0.8f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex3f(100.0f, 0, -100.0f);
    glVertex3f(-100.0f, 0, -100.0f);
    glVertex3f(-100.0f, 0, 100.0f);

    glVertex3f(100.0f, 0, -100.0f);
    glVertex3f(-100.0f, 0, 100.0f);
    glVertex3f(100.0f, 0, 100.0f);
    glEnd();
    auto trees = gen_tree_pos();
    for (size_t i = 0; i < trees.size(); i++) {
        draw_tree(trees[i]);
    }
    glColor3f(1, 0, .498);
    glutSolidTorus(0.5, 1.6, 10, 10);
    draw_cowboys();
    draw_indians();
    // End of frame
    glutSwapBuffers();
}

void processKeys(unsigned char c, int xx, int yy)
{
    switch (c) {
    case 'q':
        exit(1);
        break;
    }
}

void processSpecialKeys(int key, int xx, int yy)
{
    switch (key) {
    case GLUT_KEY_RIGHT:
        alfa -= 0.1;
        break;
    case GLUT_KEY_LEFT:
        alfa += 0.1;
        break;
    case GLUT_KEY_UP:
        beta += 0.1f;
        if (beta > 1.5f)
            beta = 1.5f;
        break;
    case GLUT_KEY_DOWN:
        beta -= 0.1f;
        if (beta < -1.5f)
            beta = -1.5f;
        break;
    case GLUT_KEY_PAGE_DOWN:
        radius -= 1.0f;
        if (radius < 1.0f)
            radius = 1.0f;
        break;
    case GLUT_KEY_PAGE_UP:
        radius += 1.0f;
        break;
    }
    spherical2Cartesian();
    glutPostRedisplay();
}

void printInfo()
{
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version: %s\n", glGetString(GL_VERSION));

    printf("\nUse Arrows to move the camera up/down and left/right\n");
    printf("Home and End control the distance from the camera to the origin");
}

int main(int argc, char** argv)
{
    // init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG@DI-UM");

    // Required callback registry
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    // Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);
    glutIdleFunc(renderScene);

    //  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    spherical2Cartesian();

    printInfo();

    // enter GLUT's main cycle
    glutMainLoop();

    return 1;
}
