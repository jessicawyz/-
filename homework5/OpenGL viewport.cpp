#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <iostream>
#include<cmath>
// ������ѧ�����ڴ��������;�������
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164

// ���ڵĿ��Ⱥ͸߶�
int width = 3840;
int height = 2160;

// ÿ�������ӿڵĿ��Ⱥ͸߶�
int viewportWidth = 1920;
int viewportHeight = 1080;


bool isPerspective = true;
//����glFrustum��glOrtho��������ز���

GLfloat nearPlane =  10;
GLfloat farPlane = 100.0;
GLfloat fovy = 45.0;
GLfloat aspect = (GLfloat)width / (GLfloat)height;
GLfloat top = nearPlane * tan(fovy * M_PI / 360.0);
GLfloat bottom = -top;
GLfloat right = top * aspect;
GLfloat left = -right;

GLfloat moveX = 0.1;
GLfloat moveZ = 0.1;
GLfloat moveY = 0.1;


// 添加模型数据结构
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;
};

// 加载模型的函数
bool loadModel(const char* path, Mesh& mesh) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, 
        aiProcess_Triangulate | 
        aiProcess_GenNormals | 
        aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    aiMesh* aiMesh = scene->mMeshes[0];

    // 获取顶点数据
    for(unsigned int i = 0; i < aiMesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.Position = glm::vec3(
            aiMesh->mVertices[i].x,
            aiMesh->mVertices[i].y,
            aiMesh->mVertices[i].z
        );
        
        if(aiMesh->HasNormals()) {
            vertex.Normal = glm::vec3(
                aiMesh->mNormals[i].x,
                aiMesh->mNormals[i].y,
                aiMesh->mNormals[i].z
            );
        }

        if(aiMesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(
                aiMesh->mTextureCoords[0][i].x,
                aiMesh->mTextureCoords[0][i].y
            );
        }

        mesh.vertices.push_back(vertex);
    }

    // 获取索引数据
    for(unsigned int i = 0; i < aiMesh->mNumFaces; i++) {
        aiFace face = aiMesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            mesh.indices.push_back(face.mIndices[j]);
    }

    // 设置VAO和VBO
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

    // 设置顶点属性指针
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);

    return true;
}

// ʼOpenGL
void initOpenGL(int argc, char** argv);
// ����ͶӰ����
void setProjection1(int viewportWidth, int viewportHeight);
void setProjection2(int viewportWidth, int viewportHeight);
void setProjection3(int viewportWidth, int viewportHeight); 
void setProjection4(int viewportWidth, int viewportHeight);

void DrawTable();
// ����2DͶӰ���ڻ��Ʒָ���
void set2DProjection();
// ����ˮƽ�ָ���
void drawHorizontalLine();
// ���ƴ�ֱ�ָ���
void drawVerticalLine();
// ��ʾ�ص����������ڻ����������������������ͷָ��߻��Ƶȣ�
void display(void);
// ���ڴ�С�ı�Ļص�����
void reshape(int w, int h);

GLfloat cameraX = 0.0, cameraY = 5.0, cameraZ = 23.0;

void keyboard(unsigned char key, int x, int y) {
    switch (key)
    {
    case 'p':
        isPerspective = !isPerspective; 
        glutPostRedisplay();  // �����ػ棬ʹͶӰģʽ�л���Ч
        break;

    case 'w':
        if (cameraY <= 15.0) {
            cameraY += moveY;
            glutPostRedisplay();
        }  
        break;
    case 'a':
        if (cameraY >= -5.0) {
            cameraY -= moveY;
            glutPostRedisplay();
        }
        break;
    case 's':
        if (cameraX >= -10.0) {
            cameraX -= moveX;
            glutPostRedisplay();
        }
        break;
    case 'd':
        if (cameraX <= 10.0) {
            cameraX += moveX;
            glutPostRedisplay();
        }
        break;

    default:
        break;
    }

}
void initOpenGL(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("OpenGL viewport (4K)");

    glEnable(GL_DEPTH_TEST);  // ������Ȳ��ԣ�ȷ����ȷ����Ⱦ˳��
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);  // ���ñ�����ɫΪ���ɫ
    // ע��ص�����
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard); // ע������¼�����
}


void setProjection1(int viewportWidth, int viewportHeight) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (isPerspective) {
        glFrustum(left, 0, bottom, 0.2 * top, nearPlane, farPlane);
    }
    else {
        /*glOrtho(left, 0, bottom, 0.2 * top, nearPlane, farPlane);*/
        glOrtho(left, 0, bottom, 0.2 * top, nearPlane, farPlane);
    }
   
    glMatrixMode(GL_MODELVIEW);
}
void setProjection2(int viewportWidth, int viewportHeight) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (isPerspective) {
        glFrustum(0, right, bottom, 0.2 * top, nearPlane, farPlane);
    }
    else {
        //glOrtho(0, right, bottom, 0.2 * top, nearPlane, farPlane);
        glOrtho(0, right, bottom, 0.2 * top, nearPlane, farPlane);

    }
    glMatrixMode(GL_MODELVIEW);
}
void setProjection3(int viewportWidth, int viewportHeight) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (isPerspective) {
        glFrustum(left, 0, 0.2 * top, top, nearPlane, farPlane);
    }
    else {
        //glOrtho(left, 0, 0.2 * top, top, nearPlane, farPlane);
        glOrtho(left, 0, 0.2 * top, top, nearPlane, farPlane);

    }
    glMatrixMode(GL_MODELVIEW);
}
void setProjection4(int viewportWidth, int viewportHeight) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (isPerspective) {
        glFrustum(0, right, 0.2 * top, top,  nearPlane, farPlane);
    }
    else {
        //glOrtho(0, right, 0.2 * top, top, nearPlane, farPlane);
        glOrtho(0, right, 0.2 * top, top, nearPlane, farPlane);


    }
    glMatrixMode(GL_MODELVIEW);
}


void DrawTable()
{
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(0, 2.0, 2);
    glScalef(5, 1, 4);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(1.5, 0, 3);
    glScalef(1, 3, 1);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(-1.5, 0, 3);
    glScalef(1, 3, 1);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(1.5, 0, 1);
    glScalef(1, 3, 1);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(-1.5, 0, 1);
    glScalef(1, 3, 1);
    glutSolidCube(1.0);
    glPopMatrix();
}

void set2DProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, 0, 1);  // ��������ͶӰ���ڻ���2D�ķָ���
    glMatrixMode(GL_MODELVIEW);
}

void drawHorizontalLine() {
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(0, height / 2);
    glVertex2f(width, height / 2);
    glEnd();
    glPopMatrix();
}

void drawVerticalLine() {
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(width / 2, 0);
    glVertex2f(width / 2, height);
    glEnd();
    glPopMatrix();
}

void display(void) {
   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset viewport to cover the entire window for drawing the lines
    glViewport(0, 0, width, height);
    glLoadIdentity();
    set2DProjection();  // Set the appropriate 2D projection for drawing the lines

    glColor3f(1.0f, 1.0f, 1.0f);
    // Draw the horizontal and vertical lines in the middle of the window
    drawHorizontalLine();
    drawVerticalLine();

        // First viewport (bottom-left)
    glViewport(0, 0, viewportWidth, viewportHeight);//(0,0)�ӿ������½���ʾ
    setProjection1(viewportWidth, viewportHeight);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); 
    DrawTable();

        // Second viewport (bottom-right)
    glViewport(viewportWidth, 0, viewportWidth, viewportHeight);//(viewportWidth, 0)->(x,y)�ӿ������½���ʾ
    setProjection2(viewportWidth, viewportHeight);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);  
    DrawTable();

        // Third viewport (top-left)
    glViewport(0, viewportHeight, viewportWidth, viewportHeight);
    setProjection3(viewportWidth, viewportHeight);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); 
    DrawTable();

        // Fourth viewport (top-right)
    glViewport(viewportWidth, viewportHeight, viewportWidth, viewportHeight);
    setProjection4(viewportWidth, viewportHeight);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);  
    DrawTable();

    

    glutSwapBuffers();  // ����ǰ�󻺳�����ʵ��˫������ƣ�ʹ����������ʾ
}

void reshape(int w, int h) {
    width = w;
    height = h;
    // �����´��ڴ�С�����¼���ÿ�������ӿڵĴ�С
    viewportWidth = width / 2;
    viewportHeight = height / 2;
    glViewport(0, 0, width, height);
    
}

int main(int argc, char** argv) {
    initOpenGL(argc, argv);
    glutMainLoop();
    return 0;
}