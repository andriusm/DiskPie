#include <stdio.h>
#include <math.h>
#include <GL/glfw.h>

#include "FileGraph.h"

FileGraph fg;
int width, height;
int centerX, centerY;
bool running = true;
const int circleWidth = 60;
const int circleWidthSquared = circleWidth * circleWidth;

void drawGraph(DirList dl);
void drawScene();
void glKill();

bool dirComparator (DirList a, DirList b)
{
    return (a.fcount >= b.fcount);
}

void GLFWCALL resizeWindow(int nWidth, int nHeight)
{
    width = nWidth;
    height = nHeight;
    height = height > 0 ? height : 1;
    centerX = width / 2;
    centerY = height / 2;
    //drawScene();
}

void GLFWCALL refreshWindow(void)
{
    //drawScene();
}

void GLFWCALL mousePosChanged(int mx, int my)
{
    //cout << "mouse: " << mx << " " << my << endl;

    mx -= centerX;
    my -= centerY;
    double angle = atan2(my, mx);
    mx *= mx;
    my *= my;

    angle = angle * 180 / M_PI + 90;
    angle += 360;
    if(angle>=360) angle -= 360;

    cout << "angle2: " << angle << endl;

    //cout << fg.dl.dirs[2].sector.startAngle << "  " << fg.dl.dirs[2].sector.angle << endl;


    int inner = fg.dl.dirs[2].depth * circleWidth;
    inner *= inner;

    int outter = (fg.dl.dirs[2].depth + 1) * circleWidth;
    outter *= outter;

    if(mx + my > inner && mx + my < outter &&
            angle > fg.dl.dirs[2].sector.startAngle &&
            angle < (fg.dl.dirs[2].sector.startAngle + fg.dl.dirs[2].sector.angle) )
    {
        fg.dl.dirs[2].sector.highlighted = TRUE;
    }
    else
    {
        fg.dl.dirs[2].sector.highlighted = FALSE;
    }

    //if(mx*mx + my*my < circleWidthSquared) fg.dl.dirs[2].sector.highlighted = TRUE;
    //else fg.dl.dirs[2].sector.highlighted = FALSE;
}

void GLFWCALL keyPressed(int key, int action)
{
    if(key==GLFW_KEY_ESC)
    {
        glKill();
    }
}

void drawScene()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0, width, 0, height );

    /*
    if ( width > height ){
    	float aspect = (GLfloat) width / height;
    	gluOrtho2D( -300.0*aspect, 300.0*aspect, -300.0, 300.0);
    }else{
    	float aspect = (GLfloat) height / width;
    	gluOrtho2D( -300.0, 300.0, -300.0*aspect, 300.0*aspect);
    }
    */


    glMatrixMode( GL_MODELVIEW );

    glPushMatrix();
    glTranslated(centerX, centerY, -1);
    drawGraph(fg.dl);
    glPopMatrix();
    glfwSwapBuffers();
}

int glInit()
{
    glfwInit();
    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 0);
    if( !glfwOpenWindow( 700, 700, 0, 0, 0, 0, 0, 0, GLFW_WINDOW ) )
    {
        glfwTerminate();
        return -1;
    }

    glfwSetWindowTitle("Disk Pie");

    glfwSwapInterval(1);

    //various callbacks
    glfwSetWindowSizeCallback(resizeWindow);
    glfwSetWindowRefreshCallback(refreshWindow);
    glfwSetKeyCallback(keyPressed);
    glfwSetMousePosCallback(mousePosChanged);

    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );

    return 0;
}

void glKill()
{
    glfwTerminate();
    exit(0);
}

void DrawSector( double nStartAngle, double nAngle, double nStartRadius, double nRadius, int nColor, bool bLit )
{
    GLUquadricObj *quadObj;
    quadObj= gluNewQuadric();
    if(quadObj)
    {
        glPushMatrix();
        long slices=(long)(nAngle/3.0);
        slices= (slices >1 ?slices:2);

        gluQuadricDrawStyle( quadObj, GLU_SILHOUETTE );
        glColor3ub(0, 0, 0);
        gluPartialDisk( quadObj, nStartRadius, nRadius, slices, 1, nStartAngle, nAngle );

        //---

        if(!bLit) glColor3ub(255 - nColor, 240, 220 - nColor);
        else glColor3ub(255, 240 - nColor, 200 - nColor);

        gluQuadricDrawStyle (quadObj, GLU_FILL );
        gluQuadricNormals( quadObj, GLU_NONE );
        gluPartialDisk( quadObj, nStartRadius, nRadius, slices, 1, nStartAngle, nAngle );

        glPopMatrix();
        gluDeleteQuadric(quadObj);
    }
}

void drawGraph(DirList dl)
{
    int dirCnt = dl.dirs.size();
    for(int i=0; i<dirCnt; i++)
    {
        DrawSector(dl.dirs[i].sector.startAngle, dl.dirs[i].sector.angle, dl.dirs[i].sector.startRadius, dl.dirs[i].sector.radius, i*30, dl.dirs[i].sector.highlighted);
        drawGraph(dl.dirs[i]);
    }
}

void prepareGraph(DirList &dl, int startAngle = 0, int endAngle = 360)
{
    int tmpCnt = 0;
    int angleLength = endAngle - startAngle;
    int dirCnt = dl.dirs.size();

    sort (dl.dirs.begin(), dl.dirs.end(), dirComparator);
    for(int i=0; i<dirCnt; i++)
    {
        float scale = (float)dl.dirs[i].fcount / (float)dl.fcount;

        dl.dirs[i].sector.highlighted = FALSE;
        dl.dirs[i].sector.startAngle = (angleLength * (float)tmpCnt / (float)dl.fcount ) + startAngle;
        dl.dirs[i].sector.angle = angleLength * scale;
        dl.dirs[i].sector.startRadius = circleWidth + (dl.depth) * circleWidth;
        dl.dirs[i].sector.radius = circleWidth + (dl.depth+1) * circleWidth;

        tmpCnt += dl.dirs[i].fcount;
        prepareGraph(dl.dirs[i], dl.dirs[i].sector.startAngle, dl.dirs[i].sector.startAngle + dl.dirs[i].sector.angle);
    }
}

int main()
{
    //fg.FindFiles("c:\\andrius\\src");
    fg.FindFiles("c:\\downloads\\mybatis\\mybatis-generator-core-1.3.1");
    if(glInit() != 0) return -1;

    prepareGraph(fg.dl);

    while(running)
    {
        drawScene();
        glfwSleep(0.03);
    }

    return 0;
}
