#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

float CAM_R = 5.0;
float CAM_X = CAM_R * cos(0.0) * sin(0.0);
float CAM_Z = CAM_R * cos(0.0) * cos(0.0);
float CAM_Y = 0.0;
int SLICES = 10;

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

void drawCylinder(float radius, float height, int slices)
{
    const double _alpha = (2 * M_PI) / slices;
    double alpha = 0.0;
    double rgb = 1.0 / slices;
    const double base = -1 * height / 2.0;
    const double top_base = base + height;
    for (int i = 0; i < slices; i++) {
        float d_alpha = alpha + _alpha;
        double a[3] = { radius * sin(d_alpha), base, radius * cos(d_alpha) };
        double b[3] = { 0.0, base, 0.0 };
        double c[3] = { radius * sin(alpha), base, radius * cos(alpha) };
        double al[3] = { radius * sin(d_alpha), top_base, radius * cos(d_alpha) };
        double bl[3] = { 0.0, top_base, 0.0 };
        double cl[3] = { radius * sin(alpha), top_base, radius * cos(alpha) };
        glColor3f(0, 0, rgb);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(c[0], c[1], c[2]);
            glVertex3f(b[0], b[1], b[2]);
            glVertex3f(a[0], a[1], a[2]);
        }
        glEnd();
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(al[0], al[1], al[2]);
            glVertex3f(bl[0], bl[1], bl[2]);
            glVertex3f(cl[0], cl[1], cl[2]);
        }
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(a[0], a[1], a[2]);
            glVertex3f(al[0], al[1], al[2]);
            glVertex3f(c[0], c[1], c[2]);
        }
        glEnd();
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(c[0], c[1], c[2]);
            glVertex3f(al[0], al[1], al[2]);
            glVertex3f(cl[0], cl[1], cl[2]);
        }
        glEnd();
        alpha += _alpha;
        rgb += 1.0 / slices;
    }
}

void renderScene(void)
{
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    gluLookAt(CAM_X, CAM_Y, CAM_Z, 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);

    drawCylinder(1, 2, SLICES);

    // End of frame
    glutSwapBuffers();
}

static float _alpha = 0.0;
static float _beta = 0.0;
void explorerCam(unsigned char key)
{
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
    }
    CAM_X = CAM_R * cos(_beta) * sin(_alpha);
    CAM_Z = CAM_R * cos(_beta) * cos(_alpha);
    CAM_Y = CAM_R * sin(_beta);
}

void fpsCam(unsigned char key)
{
    switch (key) {
    case 'j':
        // look down
        break;
    case 'k':
        // look up
        break;
    case 'l':
        // look right
        break;
    case 'h':
        // look left
        break;
    case 'w':
        // go forward
        break;
    case 's':
        // go backwards
        break;
    case 'd':
        // go right
        break;
    case 'a':
        // go left
        break;
    }
}

auto camera = explorerCam;

void processKeys(unsigned char key, int xx, int yy)
{
    camera(key);
    switch (key) {
    case 'S':
        SLICES += 1;
        break;
    case 's':
        SLICES -= 1;
        break;
    case 'c':
        camera = explorerCam;
        break;
    case 'C':
        camera = fpsCam;
        break;
    case 'q':
        exit(0);
    }
    renderScene();
}

int main(int argc, char** argv)
{
    // init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG-Assignment");

    // Required callback registry
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    // Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);

    //  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // enter GLUT's main cycle
    glutMainLoop();

    return 1;
}
