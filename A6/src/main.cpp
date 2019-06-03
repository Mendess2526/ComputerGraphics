#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include <IL/il.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define ANG2RAD M_PI / 180.0

#define COWBOYS 8
#define RAIO_COWBOYS 5
#define INDIOS 16
#define RAIO_INDIOS 25
#define ARVORES 1000
#define STEP_COWBOY 1.0f
#define STEP_INDIO 0.5f

float step = 0.0;

float height = 2.0f;
float x = 0.0f;
float z = 0.0f;

int startX, startY, tracking = 0;
float CAM_R = 15.0;
float CAM_X = CAM_R * cos(0.0) * sin(0.0);
float CAM_Z = CAM_R * cos(0.0) * cos(0.0);
float CAM_Y = 0.0;

int alpha = 0, beta = 45, r = 50;

// escrever função de processamento do teclado

void processKeys(unsigned char key, int xx, int yy)
{
    static float _alpha = 0.0;
    static float _beta = 0.0;
    switch (key) {
    case 'j':
        if (_beta + 0.1 < M_PI / 2)
            _beta += 0.1;
        break;
    case 'k':
        if (_beta - 0.1 > -M_PI / 2)
            _beta -= 0.1;
        break;
    case 'l':
        _alpha += 0.1;
        break;
    case 'h':
        _alpha -= 0.1;
        break;
    case 'i':
        CAM_R -= 0.1;
        break;
    case 'o':
        CAM_R += 0.1;
        break;
    case 'I':
        CAM_R -= 1;
        break;
    case 'O':
        CAM_R += 1;
        break;
    case 'q':
        exit(0);
    }
    CAM_X = CAM_R * cos(_beta) * sin(_alpha);
    CAM_Z = CAM_R * cos(_beta) * cos(_alpha);
    CAM_Y = CAM_R * sin(_beta);
}

void processMouseButtons(int button, int state, int xx, int yy)
{
    if (state == GLUT_DOWN) {
        startX = xx;
        startY = yy;
        if (button == GLUT_LEFT_BUTTON)
            tracking = 1;
        else if (button == GLUT_RIGHT_BUTTON)
            tracking = 2;
        else
            tracking = 0;
    } else if (state == GLUT_UP) {
        if (tracking == 1) {
            alpha += (xx - startX);
            beta += (yy - startY);
        } else if (tracking == 2) {
            r -= yy - startY;
            if (r < 3)
                r = 3.0;
        }
        tracking = 0;
    }
}

void processMouseMotion(int xx, int yy)
{
    int deltaX, deltaY;
    int alphaAux, betaAux;
    int rAux;

    deltaX = xx - startX;
    deltaY = yy - startY;

    if (tracking == 1) {
        alphaAux = alpha + deltaX;
        betaAux = beta + deltaY;
        if (betaAux > 85.0)
            betaAux = 85.0;
        else if (betaAux < -85.0)
            betaAux = -85.0;
        rAux = r;
    } else if (tracking == 2) {
        alphaAux = alpha;
        betaAux = beta;
        rAux = r - deltaY;
        if (rAux < 3)
            rAux = 3;
    } else {
        return;
    }
    CAM_X = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
    CAM_Z = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
    CAM_Y = rAux * sin(betaAux * 3.14 / 180.0);
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

void drawPlane()
{
    glColor3f(0.2f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(-100.0, 0.0, 100.0);
    glVertex3f(100.0, 0.0, 100.0);
    glVertex3f(100.0, 0.0, -100.0);
    glVertex3f(-100.0, 0.0, -100.0);
    glEnd();
}

void drawTree()
{
    glPushMatrix();
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glColor3f(1.0, 1.0, 0.5);
    glutSolidCone(0.25f, 4, 5, 1);

    glColor3f(0.0f, 0.5f + rand() * 0.5f / RAND_MAX, 0.0f);
    glTranslatef(0.0f, 0.0f, 2.0f);
    glutSolidCone(2.0f, 5.0f, 5.0f, 1.0f);
    glPopMatrix();
}

void placeTrees()
{
    float r = 35.0;
    float alpha;
    float rr;
    float x, z;

    srand(31457);
    int arvores = 0;

    while (arvores < ARVORES) {

        rr = rand() * 150.0 / RAND_MAX;
        alpha = rand() * 6.28 / RAND_MAX;

        x = cos(alpha) * (rr + r);
        z = sin(alpha) * (rr + r);

        if (fabs(x) < 100 && fabs(z) < 100) {

            glPushMatrix();
            glTranslatef(x, 0.0, z);
            drawTree();
            glPopMatrix();
            arvores++;
        }
    }
}

void drawDonut()
{
    glPushMatrix();
    glTranslatef(0.0, 0.5, 0.0);
    glColor3f(1.0f, 0.0f, 1.0f);
    glutSolidTorus(0.5, 1.25, 8, 16);
    glPopMatrix();
}

void drawIndios()
{
    float angulo;
    glColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < INDIOS; i++) {
        angulo = i * 360.0 / INDIOS + step * STEP_INDIO;
        glPushMatrix();
        glRotatef(angulo, 0.0, 1.0, 0.0);
        glTranslatef(0.0, 0.0, RAIO_INDIOS);
        glutSolidTeapot(1);
        glPopMatrix();
    }
}

void drawCowboys()
{
    float angulo;
    glColor3f(0.0f, 0.0f, 1.0f);
    for (int i = 0; i < COWBOYS; i++) {
        angulo = i * 360.0 / COWBOYS + step * STEP_COWBOY;
        glPushMatrix();
        glRotatef(-angulo, 0.0, 1.0, 0.0);
        glTranslatef(RAIO_COWBOYS, 0.0, 0.0);
        glutSolidTeapot(1);
        glPopMatrix();
    }
}

void drawScene()
{
    drawPlane();
    placeTrees();
    drawDonut();
    glPushMatrix();
    // move teapots up so that they are placed on top of the ground plane
    glTranslatef(0.0, 1.0, 0.0);
    drawCowboys();
    drawIndios();
    glPopMatrix();
}

GLuint buffers[1];
void draw_grid(unsigned int tw, unsigned int th)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    for (unsigned int i = 0; i < tw; i++) {
        glDrawArrays(GL_TRIANGLE_STRIP, i, tw * 2);
    }
}

void prepare_grid(unsigned int tw, unsigned int th, unsigned char* imageData)
{
    if (tw == 1)
        exit(1);
    unsigned int strip_width = tw * 2;
    unsigned int n_strips = th - 1;
    float strip[n_strips * strip_width * 3];
    for (unsigned int h = 0; h < n_strips; h++) {
        for (unsigned int w = 0; w < strip_width * 3; w += 6) {
            printf("Vou fazer um traco, h: %d, w: %d\n", h, w / 6);
            strip[h * strip_width + w + 0] = (w / 6) - (tw / 2);                         //x1
            strip[h * strip_width + w + 1] = 1; //imageData[h * strip_width + (w / 6)];       //y1
            strip[h * strip_width + w + 2] = h - (th / 2);                               //z1
            strip[h * strip_width + w + 3] = (w / 6) - (tw / 2);                         //x2
            strip[h * strip_width + w + 4] = 1; //imageData[(h + 1) * strip_width + (w / 6)]; //y2
            strip[h * strip_width + w + 5] = (h + 1) - (th / 2);                         //z2
        }
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glGenBuffers(1, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, strip_width * th * 3 * sizeof(float), strip, GL_STATIC_DRAW);
}

void renderScene(void)
{
    /* float pos[4] = { -1.0, 1.0, 1.0, 0.0 }; */

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(CAM_X, CAM_Y, CAM_Z,
        0.0, 0.0, 0.0,
        0.0f, 1.0f, 0.0f);

    drawScene();
    step++;

    unsigned int tw = ilGetInteger(IL_IMAGE_WIDTH);
    unsigned int th = ilGetInteger(IL_IMAGE_HEIGHT);
    glColor3f(1, 1, 1);
    draw_grid(tw, th);
    // End of frame
    glutSwapBuffers();
}

void init()
{
    // Colocar aqui load da imagem que representa o mapa de alturas
    unsigned int t;
    ilGenImages(1, &t);
    ilBindImage(t);
    ilLoadImage((ILstring) "terreno.jpg");
    ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
    int tw = ilGetInteger(IL_IMAGE_WIDTH);
    int th = ilGetInteger(IL_IMAGE_HEIGHT);
    unsigned char* imageData = ilGetData();
    prepare_grid(tw, th, imageData);

    // alguns settings para OpenGL
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
}

int main(int argc, char** argv)
{
    // inicialização
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(1000, 1000);
    glutCreateWindow("CG-Assignment");

    // registo de funções
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

    // pôr aqui registo da funções do teclado e rato
    glutKeyboardFunc(processKeys);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);

    glewInit();
    ilInit();
    init();

    // entrar no ciclo do GLUT
    glutMainLoop();

    return 0;
}
