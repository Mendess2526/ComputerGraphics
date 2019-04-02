#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

float camX = 0, camY, camZ = 5;
int startX, startY, tracking = 0;

int alpha = 0, beta = 0, r = 5;

#define POINT_COUNT 5
// Points that make up the loop for catmull-rom interpolation
float p[POINT_COUNT][3] = { { -1, -1, 0 }, { -1, 1, 0 }, { 1, 1, 0 }, { 0, 0, 0 }, { 1, -1, 0 } };

/* float p[POINT_COUNT][3] = { */
/*     { 0, 7, 0 }, */
/*     { 4, 7, 1 }, */
/*     { 6, 11, 0 }, */
/*     { 8, 7, 1 }, */
/*     { 12, 7, 0 }, */
/*     { 8, 4, 1 }, */
/*     { 9, 0, 0 }, */
/*     { 6, 4, 1 }, */
/*     { 3, 0, 0 }, */
/*     { 4, 4, 1 } */
/* }; */

void buildRotMatrix(float* x, float* y, float* z, float* m)
{
    m[0] = x[0];
    m[1] = x[1];
    m[2] = x[2];
    m[3] = 0;
    m[4] = y[0];
    m[5] = y[1];
    m[6] = y[2];
    m[7] = 0;
    m[8] = z[0];
    m[9] = z[1];
    m[10] = z[2];
    m[11] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;
}

void cross(float* a, float* b, float* res)
{
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
}

void normalize(float* a)
{
    float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    a[0] = a[0] / l;
    a[1] = a[1] / l;
    a[2] = a[2] / l;
}

float length(float* v)
{
    float res = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    return res;
}

void multMatrixVector(const float m[4][4], const float v[4], float res[4])
{
    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j][k];
        }
    }
}

void getCatmullRomPoint(
    float t, float* p0, float* p1, float* p2, float* p3, float* pos, float* deriv)
{
    // catmull-rom matrix
    const float m[4][4] = {
        { -0.5f, +1.5f, -1.5f, +0.5f },
        { +1.0f, -2.5f, +2.0f, -0.5f },
        { -0.5f, +0.0f, +0.5f, +0.0f },
        { +0.0f, +1.0f, +0.0f, +0.0f }
    };

    // Compute A = M * P
    float aX[4];
    float aY[4];
    float aZ[4];
    float pX[4] = { p0[0], p1[0], p2[0], p3[0] };
    multMatrixVector(m, pX, aX);
    float pY[4] = { p1[1], p1[1], p2[1], p3[1] };
    multMatrixVector(m, pY, aY);
    float pZ[4] = { p2[2], p1[2], p2[2], p3[2] };
    multMatrixVector(m, pZ, aZ);

    float tv[4] = { t * t * t, t * t, t, 1 };

    // Compute pos = T * A
    pos[0] = tv[0] * aX[0] + tv[1] * aX[1] + tv[2] * aX[2] + tv[3] * aX[3];
    pos[1] = tv[0] * aY[0] + tv[1] * aY[1] + tv[2] * aY[2] + tv[3] * aY[3];
    pos[2] = tv[0] * aZ[0] + tv[1] * aZ[1] + tv[2] * aZ[2] + tv[3] * aZ[3];
    // compute deriv = T' * A
    float tvl[4] = { 3 * t * t, 2 * t, 1, 0 };
    deriv[0] = tvl[0] * aX[0] + tvl[1] * aX[1] + tvl[2] * aX[2] + tvl[3] * aX[3];
    deriv[1] = tvl[0] * aY[0] + tvl[1] * aY[1] + tvl[2] * aY[2] + tvl[3] * aY[3];
    deriv[2] = tvl[0] * aZ[0] + tvl[1] * aZ[1] + tvl[2] * aZ[2] + tvl[3] * aZ[3];
}

// given  global t, returns the point in the curve
void getGlobalCatmullRomPoint(float gt, float* pos, float* deriv)
{
    float t = gt * POINT_COUNT; // this is the real global t
    int index = floor(t);       // which segment
    t = t - index;              // where within  the segment

    // indices store the points
    int indices[4];
    indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
    indices[1] = (indices[0] + 1) % POINT_COUNT;
    indices[2] = (indices[1] + 1) % POINT_COUNT;
    indices[3] = (indices[2] + 1) % POINT_COUNT;

    getCatmullRomPoint(t, p[indices[0]], p[indices[1]], p[indices[2]], p[indices[3]], pos, deriv);
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

void renderCatmullRomCurve()
{
    float gt = 0.0;
    const float NUM_STEPS = 100;
    const float gt_step = 1.0 / NUM_STEPS;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < NUM_STEPS; i++) {
        float pos[3];
        float deriv[3];
        getGlobalCatmullRomPoint(gt, pos, deriv);
        glVertex3f(pos[0], pos[1], pos[2]);
        gt += gt_step;
    }
    glEnd();
}

void renderScene(void)
{
    static float gt = 0;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(camX, camY, camZ,
        0.0, 0.0, 0.0,
        0.0f, 1.0f, 0.0f);

    renderCatmullRomCurve();
    float X[3];
    float Y[3] = { 0, 1, 0 };
    float Z[3];
    float pos[4];
    getGlobalCatmullRomPoint(gt, pos, X);
    glTranslatef(pos[0], pos[1], pos[2]);

    normalize(X);
    // Z = norm(X x Y)
    cross(X, Y, Z);
    normalize(Z);
    // Y = norm(X x Z)
    cross(Z, X, Y);
    normalize(Y);
    // buildRotMatrix()
    float m[16];
    buildRotMatrix(X, Y, Z, m);
    glMultMatrixf(m);
    glutWireTeapot(0.1);

    glutSwapBuffers();
    gt += 0.001;
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

    if (!tracking)
        return;

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
    }
    camX = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
    camZ = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
    camY = rAux * sin(betaAux * 3.14 / 180.0);
}

void keybindings(unsigned char key, int x, int y)
{
    if (key == 'q')
        exit(0);
}

int main(int argc, char** argv)
{
    // inicialization
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(320, 320);
    glutCreateWindow("CG-Assignment");

    // callback registration
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

    // mouse callbacks
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);
    glutKeyboardFunc(keybindings);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // enter GLUT's main cycle
    glutMainLoop();

    return 1;
}
