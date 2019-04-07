#include <cstring>
#include <iostream>
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

void cylinder_triangles(float, float, int);
void cylinder0(float, float, float);
void cylinder_vbo_simple(float, float, float);

float CAM_R = 5.0;
float CAM_X = CAM_R * cos(0.0) * sin(0.0);
float CAM_Z = CAM_R * cos(0.0) * cos(0.0);
float CAM_Y = 0.0;
int SLICES = 10;
float _alpha = 0.0;
float _beta = 0.0;
auto cylinder_render = cylinder_triangles;

void spherical2Cartesian()
{
    CAM_X = CAM_R * cos(_beta) * sin(_alpha);
    CAM_Z = CAM_R * cos(_beta) * cos(_alpha);
    CAM_Y = CAM_R * sin(_beta);
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

/*-----------------------------------------------------------------------------------
	Draw Cylinder with strips and fans

	  parameters: radius, height, sides

-----------------------------------------------------------------------------------*/

void cylinder0(float radius, float height, int sides)
{
    int i;
    float step;

    step = 360.0 / sides;

    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLE_FAN);

    glVertex3f(0, height * 0.5, 0);
    for (i = 0; i <= sides; i++) {
        glVertex3f(cos(i * step * M_PI / 180.0) * radius, height * 0.5, -sin(i * step * M_PI / 180.0) * radius);
    }
    glEnd();

    glColor3f(0, 1, 0);
    glBegin(GL_TRIANGLE_FAN);

    glVertex3f(0, -height * 0.5, 0);
    for (i = 0; i <= sides; i++) {
        glVertex3f(cos(i * step * M_PI / 180.0) * radius, -height * 0.5, sin(i * step * M_PI / 180.0) * radius);
    }
    glEnd();

    glColor3f(0, 0, 1);
    glBegin(GL_TRIANGLE_STRIP);

    for (i = 0; i <= sides; i++) {
        glVertex3f(cos(i * step * M_PI / 180.0) * radius, height * 0.5, -sin(i * step * M_PI / 180.0) * radius);
        glVertex3f(cos(i * step * M_PI / 180.0) * radius, -height * 0.5, -sin(i * step * M_PI / 180.0) * radius);
    }
    glEnd();
}

/*-----------------------------------------------------------------------------------
	Draw Cylinder

		parameters: radius, height, sides

-----------------------------------------------------------------------------------*/

void cylinder_triangles(float radius, float height, int slices)
{
    const float step = (2 * M_PI) / slices;
    float alpha = 0.0;
    float rgb = 1.0 / slices;
    const float base = -1 * height / 2.0;
    const float top_base = base + height;
    for (int i = 0; i < slices; i++) {
        float dstep = alpha + step;
        float a[3] = { radius * sin(dstep), base, radius * cos(dstep) };
        float b[3] = { 0.0, base, 0.0 };
        float c[3] = { radius * sin(alpha), base, radius * cos(alpha) };
        float al[3] = { radius * sin(dstep), top_base, radius * cos(dstep) };
        float bl[3] = { 0.0, top_base, 0.0 };
        float cl[3] = { radius * sin(alpha), top_base, radius * cos(alpha) };
        glColor3f(0, 0, rgb);
        glBegin(GL_TRIANGLES);
        {
            //base
            glVertex3f(c[0], c[1], c[2]);
            glVertex3f(b[0], b[1], b[2]);
            glVertex3f(a[0], a[1], a[2]);
            //base'
            glVertex3f(al[0], al[1], al[2]);
            glVertex3f(bl[0], bl[1], bl[2]);
            glVertex3f(cl[0], cl[1], cl[2]);
            //side
            glVertex3f(a[0], a[1], a[2]);
            glVertex3f(al[0], al[1], al[2]);
            glVertex3f(c[0], c[1], c[2]);
            //side
            glVertex3f(c[0], c[1], c[2]);
            glVertex3f(al[0], al[1], al[2]);
            glVertex3f(cl[0], cl[1], cl[2]);
        }
        glEnd();
        alpha += step;
        rgb += 1.0 / slices;
    }
}

class CylinderVBO {
public:
    GLuint buffers[1];
    float radius, height;
    int slices;
    bool vbo_rendered;

    CylinderVBO(float radius, float height, int slices)
        : radius(radius)
        , height(height)
        , slices(slices)
        , vbo_rendered(false)
    {
        glGenBuffers(1, buffers);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    };

    size_t prepare_vbo()
    {
        const size_t vertexes_size = (slices * 3 + slices * 6 + slices * 3) * 3;
        float* vertexes = (float*)malloc(sizeof(float) * vertexes_size);
        int vi = 0;
        const float step = (2 * M_PI) / slices;
        float alpha = 0.0;
        float rgb = 1.0 / slices;
        const float base = -1 * height / 2.0;
        const float top_base = base + height;
        for (int i = 0; i < slices; i++) {
            const float dstep = alpha + step;
            const float a[3] = { radius * sin(dstep), base, radius * cos(dstep) };
            const float b[3] = { 0.0, base, 0.0 };
            const float c[3] = { radius * sin(alpha), base, radius * cos(alpha) };
            const float al[3] = { radius * sin(dstep), top_base, radius * cos(dstep) };
            const float bl[3] = { 0.0, top_base, 0.0 };
            const float cl[3] = { radius * sin(alpha), top_base, radius * cos(alpha) };
            const size_t v_size = sizeof(float) * 3;
            //base
            memcpy(vertexes + vi, c, v_size);
            vi += 3;
            memcpy(vertexes + vi, b, v_size);
            vi += 3;
            memcpy(vertexes + vi, a, v_size);
            vi += 3;
            //base'
            memcpy(vertexes + vi, al, v_size);
            vi += 3;
            memcpy(vertexes + vi, bl, v_size);
            vi += 3;
            memcpy(vertexes + vi, cl, v_size);
            vi += 3;
            //side
            memcpy(vertexes + vi, a, v_size);
            vi += 3;
            memcpy(vertexes + vi, al, v_size);
            vi += 3;
            memcpy(vertexes + vi, c, v_size);
            vi += 3;
            //side
            memcpy(vertexes + vi, c, v_size);
            vi += 3;
            memcpy(vertexes + vi, al, v_size);
            vi += 3;
            memcpy(vertexes + vi, cl, v_size);
            vi += 3;
            alpha += step;
            rgb += 1.0 / slices;
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexes_size, vertexes, GL_STATIC_DRAW);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        free(vertexes);
        vbo_rendered = true;
        return vertexes_size / 3;
    }

    void set_slices(int slices)
    {
        this->vbo_rendered = vbo_rendered && this->slices == slices;
        this->slices = slices;
    }

    void render()
    {
        static size_t n_vertexes = 0;
        if (!vbo_rendered) {
            n_vertexes = this->prepare_vbo();
        }
        glDrawArrays(GL_TRIANGLES, 0, n_vertexes);
    }
};

void cylinder_vbo_simple(float radius, float height, int slices)
{
    static CylinderVBO vbo(radius, height, slices);
    vbo.set_slices(slices);
    vbo.render();
}

float fps_counter()
{
    static int frame = 0;
    static int timebase = 0;
    int et = glutGet(GLUT_ELAPSED_TIME);
    frame++;
    static float fps = 0;
    if (et - timebase > 1000) {
        fps = frame * 1000.0 / (et - timebase);
        timebase = et;
        frame = 0;
    }
    return fps;
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

    glColor3f(1, 1, 1);
    cylinder_render(1, 2, SLICES);

    // End of frame
    glutSwapBuffers();
    char title[1024];
    sprintf(title, "SIDES: %d | FPS: %4f", SLICES, fps_counter());
    glutSetWindowTitle(title);
}

void processKeys(unsigned char key, int xx, int yy)
{
    // put code to process regular keys in here
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
    case 'S':
        SLICES += 10;
        break;
    case 's':
        SLICES -= 1;
        break;
    case 'T':
        SLICES += 20000;
        break;
    case 't':
        SLICES = 10;
        break;
    case 'q':
        exit(0);
    }
    spherical2Cartesian();
    glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy)
{
    switch (key) {
    case GLUT_KEY_RIGHT:
        _alpha -= 0.1;
        break;
    case GLUT_KEY_LEFT:
        _alpha += 0.1;
        break;
    case GLUT_KEY_UP:
        _beta += 0.1f;
        if (_beta > 1.5f)
            _beta = 1.5f;
        break;
    case GLUT_KEY_DOWN:
        _beta -= 0.1f;
        if (_beta < -1.5f)
            _beta = -1.5f;
        break;
    case GLUT_KEY_PAGE_DOWN:
        CAM_R -= 0.1f;
        if (CAM_R < 0.1f)
            CAM_R = 0.1f;
        break;
    case GLUT_KEY_PAGE_UP:
        CAM_R += 0.1f;
        break;
    case GLUT_KEY_F1:
        cylinder_render = cylinder_triangles;
        break;
    case GLUT_KEY_F2:
        cylinder_render = cylinder_vbo_simple;
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
    printf("Home and End control the distance from the camera to the origin\n");
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
    glutIdleFunc(renderScene);

    // Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

#ifndef __APPLE__
    glewInit();
#endif

    //  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableClientState(GL_VERTEX_ARRAY);

    spherical2Cartesian();

    printInfo();

    // enter GLUT's main cycle
    glutMainLoop();

    return 1;
}
