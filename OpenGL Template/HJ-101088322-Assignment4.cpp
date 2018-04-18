//***********************************************************************************************
// HJ-101088322-Assignment4.cpp by Jamie Ching-chun Huang,101088322 (C) 2018 All Rights Reserved.
//
// Assignment 4 submission.
//
// Description:
// Insert the number of division and play.
//
//***********************************************************************************************

#include "stdlib.h"
#include <iostream>

#include <glew.h>
#include "time.h"
#include <glm.hpp>
#include <matrix_transform.hpp>
#include <GLUT/GLUT.h>
#include <LoadShaders.h>
#include <SOIL.h>

#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)

void keyOperations();


GLuint program;

GLuint MatrixID;

GLuint planeVao;
GLuint plane_tex;
GLuint lightVao;

glm::mat4 MVP;
glm::mat4 View;
glm::mat4 Projection;

GLfloat u = 0.0f, v = 0.05f, w = 0.0f;
//GLfloat u,v=0.1f,w;


glm::vec3 currentCamPos;
glm::vec3 currentCamVel;
glm::vec3 lightColor;
glm::vec3 lightPos = glm::vec3(u, v, w);
glm::vec3 viewPos;

const int division = 7;
const int row = division;
const int col = division;
float planeUnit = 1.0f;
float bound = planeUnit / 2.0f;
float dividedUnit = planeUnit/division;

const int NumVertices = division * division * 6;
glm::vec3 normals[NumVertices];

int frame=0,currentTime,timebase=0;
float deltaTime = 0;
bool keyStates[256] = {}; // Create an array of boolean values of length 256 (0-255)
float speed = 4.0;
float moveDist = 0;
float maxDis = 20;

void init(void){
    //Specifying the name of vertex and fragment shaders.
    ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "triangles.vert" },
        { GL_FRAGMENT_SHADER, "triangles.frag" },
        { GL_NONE, NULL }
    };
    
    //Loading and compiling shaders
    program = LoadShaders(shaders);
    glUseProgram(program);    //My Pipeline is set up
    
    
    // Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(program, "MVP");
    
    // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    
    currentCamPos = glm::vec3(0.0f,1.0f,1.0f);
    currentCamVel = glm::vec3(0.0f);

    // Camera matrix
    View = glm::lookAt(
                       currentCamPos, // Camera position in World Space
                       glm::vec3(0,0,0), // and looks at the origin
                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                       );
    
    glUniform3f(glGetUniformLocation(program, "lightColor"), 1.0f,1.0f,1.0f);
    glUniform3f(glGetUniformLocation(program, "lightPos"), u, v, w);
    glUniform3f(glGetUniformLocation(program, "viewPos"), currentCamPos.x,currentCamPos.y,currentCamPos.z);
    
}


void initPlane(){
    
    glGenVertexArrays(1, &planeVao);
    glBindVertexArray(planeVao);
    
    GLint width, height;
    glActiveTexture(GL_TEXTURE0);
    unsigned char* image = SOIL_load_image("/Users/jamie/Documents/2018Winter/GAME2012_3DGraphicsProgramming/02_assignments/Assignment4/2DTopdownTileset.png",&width, &height, 0, SOIL_LOAD_RGB);

    plane_tex = 0;
    glGenTextures(1, &plane_tex);
    glBindTexture(GL_TEXTURE_2D, plane_tex);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glUniform1i(glGetUniformLocation(program, "texture0"), 0);
    

    float*  planePoints;
    planePoints = new float[(row+1)*(col+1)*3];
    float mX, mY= 0.0f, mZ;
    int countV = 0;
    for(int i=0; i<=row; ++i){
        mZ = bound - (dividedUnit*i);
        for(int j=0; j<= col; ++j)
        {
            mX = -bound + (dividedUnit*j);

            planePoints[countV] = mX;
            planePoints[countV+1] = mY;
            planePoints[countV+2] = mZ;
            normals[countV] = {0.0f,1.0f,0.0f};
            normals[countV+1] = {0.0f,1.0f,0.0f};
            normals[countV+2] = {0.0f,1.0f,0.0f};
            countV +=3;
        }
    }


    GLuint planePointsVbo = 0;
    glGenBuffers(1, &planePointsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, planePointsVbo);
    glBufferData(GL_ARRAY_BUFFER, (row+1)*(col+1)*3*sizeof(float), planePoints, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    

    float* planeTexCoords;
    planeTexCoords = new float[(row+1)*(col+1)*2];
    int countT = 0;
    float tX, tY;
    for(int i=0; i<=row; ++i){
        for(int j=0; j<= col; ++j)
        {
            tY = 1.0f - (dividedUnit*i);
            tX = dividedUnit*j;

            planeTexCoords[countT] = tX;
            planeTexCoords[countT+1] = tY;

            countT +=2;
        }
    }
    
    
    GLuint planeVbo = 0;
    glGenBuffers(1, &planeVbo);
    glBindBuffer(GL_ARRAY_BUFFER, planeVbo);
    glBufferData(GL_ARRAY_BUFFER, (row+1)*(col+1)*2*sizeof(float), planeTexCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    
    GLuint planeNormal = 0;
    glGenBuffers(1, &planeNormal);
    glBindBuffer(GL_ARRAY_BUFFER, planeNormal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);


    short* PlaneIndexList;
    PlaneIndexList = new short[row*col*6];
    int count = 0;
    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){

            PlaneIndexList[count] = j + (i*(division+1));
            PlaneIndexList[count+1] = PlaneIndexList[count] + 1;
            PlaneIndexList[count+2] = PlaneIndexList[count] + 9;
            PlaneIndexList[count+3] = PlaneIndexList[count];
            PlaneIndexList[count+4] = PlaneIndexList[count+2];
            PlaneIndexList[count+5] = PlaneIndexList[count] + (division+1);

            count +=6;

        }
    }
    
    std::cout<<"triangle 1: " <<PlaneIndexList[0] <<", "<<PlaneIndexList[1]<<", "<<PlaneIndexList[2] <<std::endl;
    std::cout<<"triangle 2: " <<PlaneIndexList[3] <<", "<<PlaneIndexList[4]<<", "<<PlaneIndexList[5] <<std::endl;
    std::cout<<"triangle 3: " <<PlaneIndexList[6] <<", "<<PlaneIndexList[7]<<", "<<PlaneIndexList[8] <<std::endl;
    std::cout<<"triangle 4: " <<PlaneIndexList[9] <<", "<<PlaneIndexList[10]<<", "<<PlaneIndexList[11] <<std::endl;
    std::cout<<"triangle 5: " <<PlaneIndexList[12] <<", "<<PlaneIndexList[13]<<", "<<PlaneIndexList[14] <<std::endl;
    
    GLuint planeIBO;
    glGenBuffers(1, &planeIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, row*col*6*sizeof(short), PlaneIndexList, GL_STATIC_DRAW);
}



void transformObject(glm::vec3 scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation){
    
    glm::mat4 Model;
    Model = glm::mat4(1.0f);
    Model = glm::translate(Model, translation);
    Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
    Model = glm::scale(Model, scale);
    MVP = Projection * View * Model;
    
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
}


void drawPlane(){
    glBindVertexArray(planeVao);
    glBindTexture(GL_TEXTURE_2D, plane_tex);
    
    glDrawElements(GL_TRIANGLES, 6*49, GL_UNSIGNED_SHORT, 0);
    
}


//---------------------------------------------------------------------
//
// display
//
void display(void){
    keyOperations();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.02, 0.14, 0.34, 1.0);
    
    
    transformObject(glm::vec3(1.0f),Y_AXIS,0,glm::vec3(0,0,0));
    drawPlane();
    

    glutSwapBuffers();
    deltaTime = (glutGet(GLUT_ELAPSED_TIME) - currentTime)/1000.0f;
    currentTime = glutGet(GLUT_ELAPSED_TIME);
}

void keyDown(unsigned char key, int x, int y){
    keyStates[key] = true; // Set the state of the current key to pressed
}

void keyUp(unsigned char key, int x, int y){
    keyStates[key] = false; // Release the state of the current key to pressed
}

void keyOperations (void) {
    float cameraSpeed = 10.0f;
    if (keyStates['w']) { // If the 'w' key has been pressed
        currentCamPos.z -= cameraSpeed * (deltaTime);
    }
    if (keyStates['s']) { // If the 's' key has been pressed
        currentCamPos.z += cameraSpeed * (deltaTime);
    }
    if (keyStates['a']) { // If the 'a' key has been pressed
        currentCamPos.x -= cameraSpeed * (deltaTime);
    }
    if (keyStates['d']) { // If the 'd' key has been pressed
        currentCamPos.x += cameraSpeed * (deltaTime);
    }
    if (keyStates['r']) { // If the 'r' key has been pressed
        currentCamPos.y += cameraSpeed * (deltaTime);
    }
    if (keyStates['f']) { // If the 'f' key has been pressed
        currentCamPos.y -= cameraSpeed * (deltaTime);
    }
    View = glm::lookAt(
                       currentCamPos,
                       glm::vec3(0,0,0), // and looks at the origin
                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                       );
    
    float lightSpeed = 1.0f;
    if (keyStates['i']) {
        w -= lightSpeed * (deltaTime);
    }
    if (keyStates['k']) {
        w += lightSpeed * (deltaTime);
    }
    if (keyStates['j']) {
        u -= lightSpeed * (deltaTime);
    }
    if (keyStates['l']) {
        u += lightSpeed * (deltaTime);
    }
//    lightPos = glm::vec3(u,v,w);
    glUniform3f(glGetUniformLocation(program, "lightPos"), u, v, w);
    glUniform3f(glGetUniformLocation(program, "viewPos"), currentCamPos.x,currentCamPos.y,currentCamPos.z);
    
}


void idle(){
    
}

void Timer(int id){
    glutPostRedisplay();
    glutTimerFunc(15, Timer, 0);
}

//---------------------------------------------------------------------
//
// main
//
int main(int argc, char** argv){
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_DEPTH);
    
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(300, 100);
    glutCreateWindow("Huang, Jamie Ching-chun, 101088322");
    
    glewExperimental = true;
    glewInit();    //Initializes the glew and prepares the drawing pipeline.
    
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
    glEnable(GL_DEPTH_TEST);
    
    init();
    initPlane();
    
    
    glutTimerFunc(15, Timer, 0);
    
    glutDisplayFunc(display);
    
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    
    glutIdleFunc(idle);
    
    glutMainLoop();
}


