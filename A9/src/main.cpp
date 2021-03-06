#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <math.h>

#define _PI_ 3.14159

float alfa = 0.0f, beta = 0.0f, radius = 5.0f;
float camX, camY, camZ;

GLuint vertexCount, vertices[2];

int timebase = 0, frame = 0;

void sphericalToCartesian()
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

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set the correct perspective
    gluPerspective(45, ratio, 1, 1000);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void prepareCilinder(float height, float radius, int sides)
{
    float* v = (float*)malloc(sizeof(float) * 4 * 3 * 3 * sides);
    float* n = (float*)malloc(sizeof(float) * 4 * 3 * 3 * sides);

    int vertex = 0;
    const float delta = 2.0f * _PI_ / sides;

    for (int i = 0; i < sides; ++i) {
        // top
        // central point
        v[vertex * 3 + 0] = 0.0f;
        v[vertex * 3 + 1] = height / 2.0f;
        v[vertex * 3 + 2] = 0.0f;

        n[vertex * 3 + 0] = 0.0f;
        n[vertex * 3 + 1] = 1;
        n[vertex * 3 + 2] = 0.0f;

        vertex++;
        v[vertex * 3 + 0] = radius * sin(i * delta);
        v[vertex * 3 + 1] = height / 2.0f;
        v[vertex * 3 + 2] = radius * cos(i * delta);
        n[vertex * 3 + 0] = 0.0f;
        n[vertex * 3 + 1] = 1;
        n[vertex * 3 + 2] = 0.0f;

        vertex++;
        v[vertex * 3 + 0] = radius * sin((i + 1) * delta);
        v[vertex * 3 + 1] = height / 2.0f;
        v[vertex * 3 + 2] = radius * cos((i + 1) * delta);
        n[vertex * 3 + 0] = 0.0f;
        n[vertex * 3 + 1] = 1;
        n[vertex * 3 + 2] = 0.0f;

        // body
        // tri�ngulo 1
        vertex++;
        v[vertex * 3 + 0] = radius * sin((i + 1) * delta);
        v[vertex * 3 + 1] = height / 2.0f;
        v[vertex * 3 + 2] = radius * cos((i + 1) * delta);
        n[vertex * 3 + 0] = sin((i + 1) * delta);
        n[vertex * 3 + 1] = 0;
        n[vertex * 3 + 2] = cos((i + 1) * delta);

        vertex++;
        v[vertex * 3 + 0] = radius * sin(i * delta);
        v[vertex * 3 + 1] = height / 2.0f;
        v[vertex * 3 + 2] = radius * cos(i * delta);
        n[vertex * 3 + 0] = sin(i * delta);
        n[vertex * 3 + 1] = 0;
        n[vertex * 3 + 2] = cos(i * delta);

        vertex++;
        v[vertex * 3 + 0] = radius * sin(i * delta);
        v[vertex * 3 + 1] = -height / 2.0f;
        v[vertex * 3 + 2] = radius * cos(i * delta);
        n[vertex * 3 + 0] = sin(i * delta);
        n[vertex * 3 + 1] = 0;
        n[vertex * 3 + 2] = cos(i * delta);

        // triangle 2
        vertex++;
        v[vertex * 3 + 0] = radius * sin((i + 1) * delta);
        v[vertex * 3 + 1] = -height / 2.0f;
        v[vertex * 3 + 2] = radius * cos((i + 1) * delta);
        n[vertex * 3 + 0] = sin((i + 1) * delta);
        n[vertex * 3 + 1] = 0;
        n[vertex * 3 + 2] = cos((i + 1) * delta);

        vertex++;
        v[vertex * 3 + 0] = radius * sin((i + 1) * delta);
        v[vertex * 3 + 1] = height / 2.0f;
        v[vertex * 3 + 2] = radius * cos((i + 1) * delta);
        n[vertex * 3 + 0] = sin((i + 1) * delta);
        n[vertex * 3 + 1] = 0;
        n[vertex * 3 + 2] = cos((i + 1) * delta);

        vertex++;
        v[vertex * 3 + 0] = radius * sin(i * delta);
        v[vertex * 3 + 1] = -height / 2.0f;
        v[vertex * 3 + 2] = radius * cos(i * delta);
        n[vertex * 3 + 0] = sin(i * delta);
        n[vertex * 3 + 1] = 0;
        n[vertex * 3 + 2] = cos(i * delta);

        // base
        // central point
        vertex++;
        v[vertex * 3 + 0] = 0.0f;
        v[vertex * 3 + 1] = -height / 2.0f;
        v[vertex * 3 + 2] = 0.0f;
        n[vertex * 3 + 0] = 0.0f;
        n[vertex * 3 + 1] = -1;
        n[vertex * 3 + 2] = 0.0f;

        vertex++;
        v[vertex * 3 + 0] = radius * sin((i + 1) * delta);
        v[vertex * 3 + 1] = -height / 2.0f;
        v[vertex * 3 + 2] = radius * cos((i + 1) * delta);
        n[vertex * 3 + 0] = 0.0f;
        n[vertex * 3 + 1] = -1;
        n[vertex * 3 + 2] = 0.0f;

        vertex++;
        v[vertex * 3 + 0] = radius * sin(i * delta);
        v[vertex * 3 + 1] = -height / 2.0f;
        v[vertex * 3 + 2] = radius * cos(i * delta);
        n[vertex * 3 + 0] = 0.0f;
        n[vertex * 3 + 1] = -1;
        n[vertex * 3 + 2] = 0.0f;

        vertex++;
    }

    vertexCount = vertex;

    glGenBuffers(2, vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vertices[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount * 3, v, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertices[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount * 3, n, GL_STATIC_DRAW);

    free(v);
    free(n);
}

void drawCilinder()
{
    float c[] = { .6, .6, .6, 1 };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, c);

    /* float specular_arr[] = { 0, 0, 0, 1 }; */
    /* glMaterialfv(GL_FRONT, GL_SPECULAR, specular_arr); */

    /* float emissive_arr[] = { 0, 0, 0, 1 }; */
    /* glMaterialfv(GL_FRONT, GL_EMISSION, emissive_arr); */

    /* float ambient_arr[] = { 0.2, 0.2, 0.2, 1 }; */
    /* glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_arr); */

    glBindBuffer(GL_ARRAY_BUFFER, vertices[0]);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vertices[1]);
    glNormalPointer(GL_FLOAT, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void renderScene(void)
{
    /* float pos[4] = { 1.0, 1.0, 1.0, 0.0 }; */
    float fps;
    int time;
    char s[64];

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    gluLookAt(camX, camY, camZ,
        0.0, 0.0, 0.0,
        0.0f, 1.0f, 0.0f);

    GLfloat amb[4] = { 0, 0, 0, 1 };
    GLfloat diff[4] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);

    GLfloat dir[4] = { 0.0, 1.0, 1.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, dir);

    drawCilinder();
    glPushMatrix();
    glTranslatef(0, 0, -20);
    drawCilinder();
    glPopMatrix();

    /* glTranslatef(0, 0, -10); */
    /* float em[] = {1, 1, 1, 1}; */
    /* glMaterialfv(GL_FRONT, GL_EMISSION, em); */
    /* glutSolidSphere(0.1, 10, 10); */
    /* float em_reset[] = {0, 0, 0, 1}; */
    /* glMaterialfv(GL_FRONT, GL_EMISSION, em_reset); */

    frame++;
    time = glutGet(GLUT_ELAPSED_TIME);
    if (time - timebase > 1000) {
        fps = frame * 1000.0 / (time - timebase);
        timebase = time;
        frame = 0;
        sprintf(s, "FPS: %f6.2", fps);
        glutSetWindowTitle(s);
    }

    // End of frame
    glutSwapBuffers();
}

void processKeys(unsigned char key, int xx, int yy)
{
    // put code to process regular keys in here
    switch (key) {
    case 'j':
        if (beta + 0.1 < M_PI / 2)
            beta += 0.1;
        break;
    case 'k':
        if (beta - 0.1 > -M_PI / 2)
            beta -= 0.1;
        break;
    case 'l':
        alfa += 0.1;
        break;
    case 'h':
        alfa -= 0.1;
        break;
    case 'i':
        radius -= 0.1;
        break;
    case 'o':
        radius += 0.1;
        break;
    case 'q':
        exit(0);
    }
    sphericalToCartesian();
    glutPostRedisplay();
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
        radius -= 0.1f;
        if (radius < 0.1f)
            radius = 0.1f;
        break;

    case GLUT_KEY_PAGE_UP:
        radius += 0.1f;
        break;
    }
    sphericalToCartesian();
}

void initGL()
{

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    // init
    sphericalToCartesian();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    prepareCilinder(2, 1, 16);
}

int main(int argc, char** argv)
{

    // init
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(320, 320);
    glutCreateWindow("CG-Assignment");

    // callback registration
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

    // keyboard callback registration
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

#ifndef __APPLE__
    // init GLEW
    glewInit();
#endif

    initGL();

    glutMainLoop();

    return 1;
}
