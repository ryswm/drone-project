/*******************************************************************
           Multi-Part Model Construction and Manipulation
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "Vector3D.h"
#include "QuadMesh.h"
#include "CubeMesh.h"
#include "Building.h"
#include "Drone.h"

Drone bird;

FILE *file;
float tempSfx,tempSfy, tempSfz, tempTx, tempTz, tempF;      // Read files
int tempT;

static Building tower;                                              // Buildings
static CubeMesh block;
Building allTowers[100];                                    //List of all towers
int towerInd;                           //Specific tower
float mathChange = 0.0;
int text = 2000;
int amountOfTowersFile;

const float DEFAULT_T = -29.0, DEFAULT_S = 1.0;                     //Default Values


static unsigned char currentKey;
bool isBuilt = false, scaleH = false, scaleS = false, transBuild = false, isExtruded = false;
float sizeH = 1.0, sizeX = 1.0, sizeZ = 1.0, forback = DEFAULT_T, lefrig = DEFAULT_T;


float mouseX = 0.0, mouseY = 0.0, cam = 10.0;
int changeX, changeY;

const int meshSize = 60;    // Default Mesh Size
const int vWidth = 650;     // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels

static int currentButton;
static unsigned char currentKey;

// Light properties
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Material properties
static GLfloat drone_mat_ambient[] = { 0.4F, 0.2F, 0.0F, 1.0F };
static GLfloat drone_mat_specular[] = { 0.1F, 0.1F, 0.0F, 1.0F };
static GLfloat drone_mat_diffuse[] = { 0.9F, 0.5F, 0.0F, 1.0F };
static GLfloat drone_mat_shininess[] = { 0.0F };

// A quad mesh representing the ground
static QuadMesh groundMesh;

// Structure defining a bounding box, currently unused
//struct BoundingBox {
//    Vector3D min;
//    Vector3D max;
//} BBox;

                        /*  Texture mapping  Decs  **Taken from texture cube example code** */
typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;
typedef unsigned short ushort;
typedef unsigned long  ulong;

typedef struct RGB
{
    byte r,g,b;
} RGB;

typedef struct RGBpixmap
{
    int nRows, nCols;
    RGB *pixel;
} RGBpixmap;

RGBpixmap texture[2];
                                    /*  End of - Texture mapping Decs  */

void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
Vector3D ScreenToWorld(int x, int y);


void writeBuildings(){
    file = fopen("city.txt", "w");
    fprintf(file, "%d,", towerInd);
    for(int i = 0; i < towerInd; i++){
        if(allTowers[i].sfy < 0.3){
            tempSfy = allTowers[i].sfy;
        } else {
            tempSfy = (allTowers[i].sfy)/2.0;
        }
        fprintf(file, "%f,", allTowers[i].sfx);
        fprintf(file, "%f,", tempSfy);
        fprintf(file, "%f,", allTowers[i].sfz);
        fprintf(file, "%f,", allTowers[i].tx);
        fprintf(file, "%f,", allTowers[i].tz);
        fprintf(file, "%f,", allTowers[i].func);
        fprintf(file, "%i,", allTowers[i].tex);
    }
    fclose(file);
    printf("City Saved in city.txt");
}


/*  Texture Mapping Functions  */

/**************************************************************************
 *  fskip                                                                 *
 *     Skips bytes in a file.                                             *
 **************************************************************************/
void fskip(FILE *fp, int num_bytes)
{
    int i;
    for (i=0; i<num_bytes; i++)
        fgetc(fp);
}
/**************************************************************************
 *                                                                        *
 *    Loads a bitmap file into memory.                                    *
 **************************************************************************/

ushort getShort(FILE *fp) //helper function
{ //BMP format uses little-endian integer types
    // get a 2-byte integer stored in little-endian form
    char ic;
    ushort ip;
    ic = fgetc(fp); ip = ic;  //first byte is little one
    ic = fgetc(fp);  ip |= ((ushort)ic << 8); // or in high order byte
    return ip;
}
//<<<<<<<<<<<<<<<<<<<< getLong >>>>>>>>>>>>>>>>>>>
ulong getLong(FILE *fp) //helper function
{  //BMP format uses little-endian integer types
    // get a 4-byte integer stored in little-endian form
    ulong ip = 0;
    char ic = 0;
    unsigned char uc = ic;
    ic = fgetc(fp); uc = ic; ip = uc;
    ic = fgetc(fp); uc = ic; ip |=((ulong)uc << 8);
    ic = fgetc(fp); uc = ic; ip |=((ulong)uc << 16);
    ic = fgetc(fp); uc = ic; ip |=((ulong)uc << 24);
    return ip;
}

void readBMPFile(RGBpixmap *pm, char *file)
{
    FILE *fp;
    long index;
    int k,row,col,numPadBytes, nBytesInRow;
    char ch1,ch2;
    ulong fileSize;
    ushort reserved1;    // always 0
    ushort reserved2;     // always 0
    ulong offBits; // offset to image - unreliable
    ulong headerSize;     // always 40
    ulong numCols; // number of columns in image
    ulong numRows; // number of rows in image
    ushort planes;      // always 1
    ushort bitsPerPixel;    //8 or 24; allow 24 here
    ulong compression;      // must be 0 for uncompressed
    ulong imageSize;       // total bytes in image
    ulong xPels;    // always 0
    ulong yPels;    // always 0
    ulong numLUTentries;    // 256 for 8 bit, otherwise 0
    ulong impColors;       // always 0
    long count;
    char dum;
    
    /* open the file */
    if ((fp = fopen(file,"rb")) == NULL)
    {
        printf("Error opening file %s.\n",file);
        exit(1);
    }
    
    /* check to see if it is a valid bitmap file */
    if (fgetc(fp)!='B' || fgetc(fp)!='M')
    {
        fclose(fp);
        printf("%s is not a bitmap file.\n",file);
        exit(1);
    }
    
    fileSize =      getLong(fp);
    reserved1 =    getShort(fp);    // always 0
    reserved2=     getShort(fp);     // always 0
    offBits =        getLong(fp); // offset to image - unreliable
    headerSize =     getLong(fp);     // always 40
    numCols =        getLong(fp); // number of columns in image
    numRows =         getLong(fp); // number of rows in image
    planes=        getShort(fp);      // always 1
    bitsPerPixel=   getShort(fp);    //8 or 24; allow 24 here
    compression =    getLong(fp);      // must be 0 for uncompressed
    imageSize =     getLong(fp);       // total bytes in image
    xPels =        getLong(fp);    // always 0
    yPels =        getLong(fp);    // always 0
    numLUTentries = getLong(fp);    // 256 for 8 bit, otherwise 0
    impColors =     getLong(fp);       // always 0
    
    if(bitsPerPixel != 24)
    { // error - must be a 24 bit uncompressed image
        printf("Error bitsperpixel not 24\n");
        exit(1);
    }
    //add bytes at end of each row so total # is a multiple of 4
    // round up 3*numCols to next mult. of 4
    nBytesInRow = ((3 * numCols + 3)/4) * 4;
    numPadBytes = nBytesInRow - 3 * numCols; // need this many
    pm->nRows = numRows; // set class's data members
    pm->nCols = numCols;
    pm->pixel = (RGB *) malloc(3 * numRows * numCols);//make space for array
    if(!pm->pixel) return; // out of memory!
    count = 0;
    
    for(row = 0; row < numRows; row++) // read pixel values
    {
        for(col = 0; col < numCols; col++)
        {
            int r,g,b;
            b = fgetc(fp); g = fgetc(fp); r = fgetc(fp); //read bytes
            pm->pixel[count].r = r; //place them in colors
            pm->pixel[count].g = g;
            pm->pixel[count++].b = b;
        }
        fskip(fp,numPadBytes);
    }
    fclose(fp);
}

void setTexture(RGBpixmap *p, GLuint textureID)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, p->nCols, p->nRows, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, p->pixel);
}


        /*  End of - Texture Mapping Functions **Taken from texture cube example code**  */


int main(int argc, char **argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(vWidth, vHeight);
    glutInitWindowPosition(200, 30);
    glutCreateWindow("City Builder");

    // Initialize GL
    initOpenGL(vWidth, vHeight);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotionHandler);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(functionKeys);

    // Start event loop, never returns
    glutMainLoop();

    return 0;
}

void initOpenGL(int w, int h)
{
    // Set up and enable lighting
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glEnable(GL_LIGHT1);   // This light is currently off

    // Other OpenGL setup
    glEnable(GL_DEPTH_TEST);    // Remove hidded surfaces
    glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
    glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
    glClearDepth(1.0f);
    glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

    // Set up ground quad mesh
    Vector3D origin = NewVector3D(-30.0f, 0.0f, 30.0f);
    Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
    Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
    groundMesh = NewQuadMesh(meshSize);
    InitMeshQM(&groundMesh, meshSize, origin, 60.0, 60.0, dir1v, dir2v);

    Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
    Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
    Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
    SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);
    
    printf("To use this program: \n");
    printf("P: Make cube\n");
    printf("T: Initialize translation, use arrowkeys\n");
    printf("H: Initialize height scale, use up and down arrowkeys\n");
    printf("S: Initialize X, Z scale, use arrowkeys\n");
    printf("E: Extrude cube to building\n");
    printf("Z: Zoom In\n");
    printf("X: Zoom Out\n");
    printf("7: Save city to city.txt\n");
    printf("Use mouse to move world view\n");
    
    bird = newDrone(0.0, 0.0, 0.0, 0.0, 0.0);
    block = newCube();

    file = fopen("city.txt", "r");
    if(file){
        fscanf(file, "%d,", &amountOfTowersFile);
        towerInd = amountOfTowersFile;
        for(int i = 0; i < towerInd; i++){
            fscanf(file, "%f,", &tempSfx);
            fscanf(file, "%f,", &tempSfy);
            fscanf(file, "%f,", &tempSfz);
            fscanf(file, "%f,", &tempTx);
            fscanf(file, "%f,", &tempTz);
            fscanf(file, "%f,", &tempF);
            fscanf(file, "%i,", &tempT);
            tower = newBuilding(tempSfx, tempSfy, tempSfz, tempTx, tempTz, tempF, tempT);
            allTowers[i] = tower;
        }
        fclose(file);
    } else {
        towerInd = 0;
    }
    
    
    //Set textures
    readBMPFile(&texture[0],"clover01.bmp");
    setTexture(&texture[0],2000);
    
   
    
    // Set up the bounding box of the scene
    // Currently unused. You could set up bounding boxes for your objects eventually.
    //Set(&BBox.min, -8.0f, 0.0, -8.0);
    //Set(&BBox.max, 8.0f, 6.0, 8.0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set drone material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, drone_mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, drone_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, drone_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SHININESS, drone_mat_shininess);
    
    
    glLoadIdentity();
    gluLookAt(0.0, cam, cam, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glRotatef(mouseY, 1.0, 0.0, 0.0); //Rotate world
    glRotatef(mouseX, 0.0, 1.0, 0.0); //Rotate world
    
    glPushMatrix();
    drawDrone(&bird);
    glPopMatrix();
    
    glPushMatrix(); //Cube
    if(isBuilt){
        glTranslatef(lefrig, 0.0, forback);
        glScalef(sizeX, sizeH, sizeZ);
        glTranslatef(0.0, 1.0, 0.0);
        drawCube(&block);
    }
    glPopMatrix(); //Cube
    
    //Draw Current cubes extrusion
    tower = newBuilding(sizeX, sizeH, sizeZ, lefrig, forback, mathChange, text);
    if(isExtruded){
        allTowers[towerInd-1] = tower;
    }
    
     glEnable(GL_TEXTURE_2D);
    //Draw all created buildings
    for(int i = 0; i < towerInd; i++){
        Building tempT = allTowers[i];
        drawBuilding(&tempT);
        
    }
     glDisable(GL_TEXTURE_2D);
    
    
    // Draw ground mesh
    DrawMeshQM(&groundMesh, meshSize);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (GLdouble)w / h, 0.2, 300.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'p':
        currentKey = 'p';
        isBuilt = true;
        isExtruded = false;
        lefrig = DEFAULT_T;
        forback = DEFAULT_T;
        sizeH = DEFAULT_S;
        sizeX = DEFAULT_S;
        sizeZ = DEFAULT_S;
        mathChange = 0;
        break;
    case 't':
        currentKey = 't';
        transBuild = true;
        scaleH = false;
        scaleS = false;
        break;
    case 'h':
        currentKey = 'h';
        scaleH = true;
        transBuild = false;
        scaleS = false;
        break;
    case 's':
        currentKey = 's';
        scaleS = true;
        transBuild = false;
        scaleH = false;
        break;
    case 'e':
        currentKey = 'e';
        isExtruded = true;
        isBuilt = false;
        transBuild = false;
        scaleS = false;
        scaleH = false;
        towerInd++;
        printf("You may choose one of the following keys \nto set a predefined shape: \n1: Upside down pyramid\n");
        printf("0: Reset\n");
        break;
    case '7':
        writeBuildings();
        break;
            
    case 'z':
        cam++;
        break;
    case 'x':
        cam--;
        break;
            
    //Changes to building shape : predefined
    case '1':           //Upside down pyramid
        mathChange = 1.0;
        break;
    case '0':           //Reset
        mathChange = 0.0;
        break;
    }
    glutPostRedisplay();
}

void functionKeys(int key, int x, int y)
{
    
    if (key == GLUT_KEY_F1)
    {
        
    }
    else if (key == GLUT_KEY_LEFT)
    {
        if(transBuild){
            lefrig -= 1.0;
        }
        else if(scaleS){
            sizeX -= 0.1;
        }
    }
    else if (key == GLUT_KEY_RIGHT)
    {
        if(transBuild){
            lefrig += 1.0;
        }
        else if(scaleS){
            sizeX += 0.1;
            
        }
    }
    else if (key == GLUT_KEY_UP){
        if(scaleH){
            if(sizeH < 8.0)
                sizeH += 0.20;
        }
        else if(scaleS){
            sizeZ += 0.1;
        }
        else if(transBuild){
            forback -= 1.0;
        }
    }
    else if (key == GLUT_KEY_DOWN){
        if(scaleH){
            sizeH -= 0.20;
        }
        else if(scaleS){
            sizeZ -= 0.1;
        }
        else if(transBuild){
            forback += 1.0;
        }
    }
    
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    currentButton = button;

    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        changeX = x;
        changeY = y;
        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN)
        {
            ;
        }
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void mouseMotionHandler(int xMouse, int yMouse)
{
    if(currentButton == GLUT_LEFT_BUTTON){
        mouseX += (xMouse - changeX)*0.01;
        mouseY += (yMouse - changeY)*0.01;
    }

    glutPostRedisplay();
}

Vector3D ScreenToWorld(int x, int y)
{
    // you will need to finish this if you use the mouse
    return NewVector3D(0, 0, 0);
}



