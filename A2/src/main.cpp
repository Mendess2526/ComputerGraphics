#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>

float CAM_X = sin(0.0) * 5;
float CAM_Y = 5.0;
float CAM_Z = cos(0.0) * 5;
float PIM_TOP = 1.0;
float PIM_CENTER_X = 0.0;
float PIM_CENTER_Y = 0.0;
float PIM_CENTER_Z = 0.0;

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

void renderScene(void)
{

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    gluLookAt(CAM_X, CAM_Y, CAM_Z,
        0.0, 0.0, 0.0,
        0.0f, 1.0f, 0.0f);

    // put the geometric transformations here
    glTranslatef(PIM_CENTER_X, PIM_CENTER_Y, PIM_CENTER_Z);

    // put drawing instructions here
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(1.0f, 0.0f, -1.0f); // A
        glVertex3f(1.0f, 0.0f, 1.0f);  // B
        glVertex3f(-1.0f, 0.0f, 1.0f); // C
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    {
        glVertex3f(1.0f, 0.0f, -1.0f);  // A
        glVertex3f(-1.0f, 0.0f, 1.0f);  // C
        glVertex3f(-1.0f, 0.0f, -1.0f); // D
    }
    glEnd();

    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(1.0f, 0.0f, -1.0f);   // A
        glVertex3f(0.0f, PIM_TOP, 0.0f); // F
        glVertex3f(1.0f, 0.0f, 1.0f);    // B
    }
    glEnd();

    glColor3f(0, 1, 0);
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(1.0f, 0.0f, 1.0f);    // B
        glVertex3f(0.0f, PIM_TOP, 0.0f); // F
        glVertex3f(-1.0f, 0.0f, 1.0f);   // C
    }
    glEnd();

    glColor3f(0, 0, 1);
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(-1.0f, 0.0f, 1.0f);   // C
        glVertex3f(0.0f, PIM_TOP, 0.0f); // F
        glVertex3f(-1.0f, 0.0f, -1.0f);  // D
    }
    glEnd();

    glColor3f(0, 1, 1);
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(-1.0f, 0.0f, -1.0f);  // D
        glVertex3f(0.0f, PIM_TOP, 0.0f); // F
        glVertex3f(1.0f, 0.0f, -1.0f);   // A
    }
    glEnd();
    // End of frame
    glutSwapBuffers();
}

// write function to process keyboard events
void key_bindings(unsigned char key, int _x, int _y)
{
    static float px = 0.0;
    static float pz = 0.0;
    switch (key) {
    case 'j':
        CAM_Y -= 0.1;
        break;
    case 'k':
        CAM_Y += 0.1;
        break;
    case 'l':
        CAM_X = sin(px) * 5;
        px += 0.1;
        CAM_Z = cos(pz) * 5;
        pz += 0.1;
        break;
    case 'h':
        CAM_X = sin(px) * 5;
        px -= 0.1;
        CAM_Z = cos(pz) * 5;
        pz -= 0.1;
        break;
    case 'b':
        PIM_TOP += 0.1;
        break;
    case 'v':
        PIM_TOP -= 0.1;
        break;
    case 'w':
        PIM_CENTER_X += 0.1;
        break;
    case 's':
        PIM_CENTER_X -= 0.1;
        break;
    case 'e':
        PIM_CENTER_Y += 0.1;
        break;
    case 'd':
        PIM_CENTER_Y -= 0.1;
        break;
    case 'r':
        PIM_CENTER_Z += 0.1;
        break;
    case 'f':
        PIM_CENTER_Z -= 0.1;
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
    glutCreateWindow("CG-Keyboard");

    // Required callback registry
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    // put here the registration of the keyboard callbacks
    glutKeyboardFunc(key_bindings);

    //  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // enter GLUT's main cycle
    glutMainLoop();

    return 1;
}
