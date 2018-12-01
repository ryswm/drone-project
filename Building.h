#include <stdbool.h>
#include "Vector3D.h"

typedef struct Building
{
    
    float tx, tz;
    float sfx, sfy, sfz;
    float floors;
    float func;
    int tex;
    //float height = sfy*2;
    //float floors = height*2;
    
    
    
    // Material properties for drawing
    float mat_ambient[4];
    float mat_specular[4];
    float mat_diffuse[4];
    float mat_shininess[1];
    
    // Material properties if selected
    float highlightMat_ambient[4];
    float highlightMat_specular[4];
    float highlightMat_diffuse[4];
    float highlightMat_shininess[1];
} Building;

Building newBuilding(float scaleX, float scaleY, float scaleZ, float translX, float translZ, float mathFunc, int texture);
void drawBuilding(Building *tower);
