#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h>   // The GL Header File
#include <GL/gl.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

GLuint gProgram;
int gWidth, gHeight;
int satirSayisi, sutunSayisi;
GLfloat objeGenisligi, objeYuksekligi;
float hucreGenisligi;
float hucreYuksekligi;

struct Vertex {
    Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) {}

    GLfloat x, y, z;
};

struct Texture {
    Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) {}

    GLfloat u, v;
};

struct Normal {
    Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) {}

    GLfloat x, y, z;
};

struct Face {
    Face(int v[], int t[], int n[]) {
        vIndex[0] = v[0];
        vIndex[1] = v[1];
        vIndex[2] = v[2];
        tIndex[0] = t[0];
        tIndex[1] = t[1];
        tIndex[2] = t[2];
        nIndex[0] = n[0];
        nIndex[1] = n[1];
        nIndex[2] = n[2];
    }

    GLuint vIndex[3], tIndex[3], nIndex[3];
};

vector<Vertex> gVertices;
vector<Texture> gTextures;
vector<Normal> gNormals;
vector<Face> gFaces;

GLuint gVertexAttribBuffer, gIndexBuffer;
GLint gInVertexLoc, gInNormalLoc;
int gVertexDataSizeInBytes, gNormalDataSizeInBytes;

bool ParseObj(const string &fileName) {
    fstream myfile;

    // Open the input 
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open()) {
        string curLine;

        while (getline(myfile, curLine)) {
            stringstream str(curLine);
            GLfloat c1, c2, c3;
            GLuint index[9];
            string tmp;

            if (curLine.length() >= 2) {
                if (curLine[0] == '#') // comment
                {
                    continue;
                } else if (curLine[0] == 'v') {
                    if (curLine[1] == 't') // texture
                    {
                        str >> tmp; // consume "vt"
                        str >> c1 >> c2;
                        gTextures.push_back(Texture(c1, c2));
                    } else if (curLine[1] == 'n') // normal
                    {
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        gNormals.push_back(Normal(c1, c2, c3));
                    } else // vertex
                    {
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        gVertices.push_back(Vertex(c1, c2, c3));
                    }
                } else if (curLine[0] == 'f') // face
                {
                    str >> tmp; // consume "f"
                    char c;
                    int vIndex[3], nIndex[3], tIndex[3];
                    str >> vIndex[0];
                    str >> c >> c; // consume "//"
                    str >> nIndex[0];
                    str >> vIndex[1];
                    str >> c >> c; // consume "//"
                    str >> nIndex[1];
                    str >> vIndex[2];
                    str >> c >> c; // consume "//"
                    str >> nIndex[2];

                    assert(vIndex[0] == nIndex[0] &&
                           vIndex[1] == nIndex[1] &&
                           vIndex[2] == nIndex[2]); // a limitation for now

                    // make indices start from 0
                    for (int c = 0; c < 3; ++c) {
                        vIndex[c] -= 1;
                        nIndex[c] -= 1;
                        tIndex[c] -= 1;
                    }

                    gFaces.push_back(Face(vIndex, tIndex, nIndex));
                } else {
                    cout << "Ignoring unidentified line in obj file: " << curLine << endl;
                }
            }

            //data += curLine;
            if (!myfile.eof()) {
                //data += "\n";
            }
        }

        myfile.close();
    } else {
        return false;
    }

    /*
    for (int i = 0; i < gVertices.size(); ++i)
    {
        Vector3 n;

        for (int j = 0; j < gFaces.size(); ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                if (gFaces[j].vIndex[k] == i)
                {
                    // face j contains vertex i
                    Vector3 a(gVertices[gFaces[j].vIndex[0]].x,
                              gVertices[gFaces[j].vIndex[0]].y,
                              gVertices[gFaces[j].vIndex[0]].z);

                    Vector3 b(gVertices[gFaces[j].vIndex[1]].x,
                              gVertices[gFaces[j].vIndex[1]].y,
                              gVertices[gFaces[j].vIndex[1]].z);

                    Vector3 c(gVertices[gFaces[j].vIndex[2]].x,
                              gVertices[gFaces[j].vIndex[2]].y,
                              gVertices[gFaces[j].vIndex[2]].z);

                    Vector3 ab = b - a;
                    Vector3 ac = c - a;
                    Vector3 normalFromThisFace = (ab.cross(ac)).getNormalized();
                    n += normalFromThisFace;
                }

            }
        }

        n.normalize();

        gNormals.push_back(Normal(n.x, n.y, n.z));
    }
    */

    assert(gVertices.size() == gNormals.size());

    return true;
}

bool ReadDataFromFile(
        const string &fileName, ///< [in]  Name of the shader file
        string &data)     ///< [out] The contents of the file
{
    fstream myfile;

    // Open the input 
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open()) {
        string curLine;

        while (getline(myfile, curLine)) {
            data += curLine;
            if (!myfile.eof()) {
                data += "\n";
            }
        }

        myfile.close();
    } else {
        return false;
    }

    return true;
}

void createVS() {
    string shaderSource;

    string filename("vert.glsl");
    if (!ReadDataFromFile(filename, shaderSource)) {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar *shader = (const GLchar *) shaderSource.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shader, &length);
    glCompileShader(vs);

    char output[1024] = {0};
    glGetShaderInfoLog(vs, 1024, &length, output);
    printf("VS compile log: %s\n", output);

    glAttachShader(gProgram, vs);
}

void createFS() {
    string shaderSource;

    string filename("frag.glsl");
    if (!ReadDataFromFile(filename, shaderSource)) {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar *shader = (const GLchar *) shaderSource.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shader, &length);
    glCompileShader(fs);

    char output[1024] = {0};
    glGetShaderInfoLog(fs, 1024, &length, output);
    printf("FS compile log: %s\n", output);

    glAttachShader(gProgram, fs);
}

void initShaders() {
    gProgram = glCreateProgram();

    createVS();
    createFS();

    glLinkProgram(gProgram);
    glUseProgram(gProgram);
}

void initVBO() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    assert(vao > 0);
    glBindVertexArray(vao);
    cout << "vao = " << vao << endl;

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    assert(glGetError() == GL_NONE);

    glGenBuffers(1, &gVertexAttribBuffer);
    glGenBuffers(1, &gIndexBuffer);

    assert(gVertexAttribBuffer > 0 && gIndexBuffer > 0);

    glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    gVertexDataSizeInBytes = gVertices.size() * 3 * sizeof(GLfloat);
    gNormalDataSizeInBytes = gNormals.size() * 3 * sizeof(GLfloat);
    int indexDataSizeInBytes = gFaces.size() * 3 * sizeof(GLuint);
    GLfloat *vertexData = new GLfloat[gVertices.size() * 3];
    GLfloat *normalData = new GLfloat[gNormals.size() * 3];
    GLuint *indexData = new GLuint[gFaces.size() * 3];

    float minX = 1e6, maxX = -1e6;
    float minY = 1e6, maxY = -1e6;
    float minZ = 1e6, maxZ = -1e6;

    for (int i = 0; i < gVertices.size(); ++i) {
        vertexData[3 * i] = gVertices[i].x;
        vertexData[3 * i + 1] = gVertices[i].y;
        vertexData[3 * i + 2] = gVertices[i].z;

        minX = std::min(minX, gVertices[i].x);
        maxX = std::max(maxX, gVertices[i].x);
        minY = std::min(minY, gVertices[i].y);
        maxY = std::max(maxY, gVertices[i].y);
        minZ = std::min(minZ, gVertices[i].z);
        maxZ = std::max(maxZ, gVertices[i].z);
        objeGenisligi = maxX - minX;
        objeYuksekligi = maxY - minY;
    }

    std::cout << "minX = " << minX << std::endl;
    std::cout << "maxX = " << maxX << std::endl;
    std::cout << "minY = " << minY << std::endl;
    std::cout << "maxY = " << maxY << std::endl;
    std::cout << "minZ = " << minZ << std::endl;
    std::cout << "maxZ = " << maxZ << std::endl;

    for (int i = 0; i < gNormals.size(); ++i) {
        normalData[3 * i] = gNormals[i].x;
        normalData[3 * i + 1] = gNormals[i].y;
        normalData[3 * i + 2] = gNormals[i].z;
    }

    for (int i = 0; i < gFaces.size(); ++i) {
        indexData[3 * i] = gFaces[i].vIndex[0];
        indexData[3 * i + 1] = gFaces[i].vIndex[1];
        indexData[3 * i + 2] = gFaces[i].vIndex[2];
    }


    glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
    glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

    // done copying; can free now
    delete[] vertexData;
    delete[] normalData;
    delete[] indexData;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
}


void init(const char *objFileName) {
    ParseObj(objFileName);
    //ParseObj("bunny.obj");

    glEnable(GL_DEPTH_TEST);
    initShaders();
    initVBO();
}

void drawModel() {
    glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));

    glDrawElements(GL_TRIANGLES, gFaces.size() * 3, GL_UNSIGNED_INT, 0);
}

class Tavsan {
public:
    GLfloat x;
    GLfloat y;
    GLfloat olmasiGerekenY;
    GLfloat genislik;
    GLfloat yukseklik;
    std::vector<GLfloat> renk;
    bool animating;
    bool patliyor = false;
    bool patladi = false;
    bool kaydi = false;
    GLfloat patlamaScale;

    Tavsan(): animating(false), patlamaScale(1.0) {

    }

    bool kayiyor() {
        return olmasiGerekenY <= y;
    }

    void ciz(float angle) {
        // todo: color
        glLoadIdentity();
        glTranslatef(x, y, -10);

        GLfloat olmasiGerekenGenislik = 20.0 / sutunSayisi;
        GLfloat olmasiGerekenYukseklik = 20.0 / satirSayisi;
        GLfloat genislikScale = (olmasiGerekenGenislik / objeGenisligi) * 0.75;
        GLfloat yukseklikScale = (olmasiGerekenYukseklik / objeYuksekligi) * 0.75;
        glScalef(genislikScale,yukseklikScale,1);

        if (patliyor) {
            patlamaScale += 0.01;
            if (patlamaScale >= 1.5) {
                // patladi
                patladi = true;
                patliyor = false;
            }
        }

        if (kayiyor()) {
            animating = true;
            y -= 0.05;
            if (!kayiyor()) {
                kaydi = true;
                animating = false;
            }
        }

        glScalef(patlamaScale, patlamaScale, 1);
        glRotatef(angle, 0, 1, 0);
        if (!patladi) {
            drawModel();
        }

    }

    void patlamayiBaslat() {
        if (!animating) {
            animating = true;
            patliyor = true;
        }
    }
};

class Tavsanlar {
public:
    std::vector<std::vector<Tavsan>> tavsanlar;
    std::vector<std::vector<GLfloat>> colors = {
            {1.0, 0, 0}, // red
            {0, 1.0, 0}, // green
            {0, 0, 1.0}, // blue
            {1.0, 1.0, 0}, // yellow
            {1.0, 0, 1.0} // purple
    };

    Tavsanlar() {
        // todo: en basta yan yana gelenleri patlatmayi unutma
        tavsanlar = std::vector<std::vector<Tavsan>>(satirSayisi, std::vector<Tavsan>(sutunSayisi));
        for (int i = 0; i < tavsanlar.size(); ++i) {
            for (int j = 0; j < tavsanlar[0].size(); ++j) {

                float hucreX = getX(j);
                float hucreY = getY(i);
                tavsanlar[i][j].x = hucreX;
                tavsanlar[i][j].y = hucreY;
                tavsanlar[i][j].renk = colors[rand() % 5];
            }
        }
    }

    GLfloat getX(int j) {
        return -10 + (j + 0.5) * hucreGenisligi;
    }

    GLfloat getY(int i) {
        return 10 - (i + 0.5) * hucreYuksekligi;
    }

    void ciz(float angle) {

        for (int j = 0; j < tavsanlar[0].size(); ++j) {
            std::vector<Tavsan> patlamayanlar;
            for (int i = 0; i < tavsanlar.size(); ++i) {
                auto& tavsan = tavsanlar[i][j];
                if (!tavsan.patladi) {
                    patlamayanlar.push_back(tavsan);
                }
            }
            for (int i = patlamayanlar.size(), rastgeleTavsanI = 0; i < satirSayisi; ++i, rastgeleTavsanI++) {
                Tavsan rastgeleTavsan;
                rastgeleTavsan.renk = colors[rand() % 5];
                rastgeleTavsan.x = getX(j);
                rastgeleTavsan.y = getY(-(rastgeleTavsanI+1));
                rastgeleTavsan.olmasiGerekenY = getY(i);

                patlamayanlar.push_back(rastgeleTavsan);
            }
            for (int i = 0; i < tavsanlar.size(); ++i) {
                tavsanlar[i][j] = patlamayanlar[i];
                tavsanlar[i][j].olmasiGerekenY = getY(i);
            }
        }
        for (int i = 0; i < tavsanlar.size(); ++i) {
            for (int j = 0; j < tavsanlar[0].size(); ++j) {
                tavsanlar[i][j].ciz(angle);
            }
        }
    }

    void patlat(int i, int j) {
        tavsanlar[i][j].patlamayiBaslat();
    }
};
Tavsanlar* tavsanlar = nullptr;

void display() {
    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    static float angle = 0;


    // todo: her model icin isik kaynagi olustur
    // todo: renk
    // todo: boyut

    tavsanlar->ciz(angle);


    angle += 0.5;
}

void reshape(GLFWwindow *window, int w, int h) {
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    gWidth = w;
    gHeight = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10, 10, -10, 10, -20, 20);
    //gluPerspective(45, 1, 1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        //getting cursor position
        glfwGetCursorPos(window, &xpos, &ypos);
        float hucreGenisligiPx = gWidth / sutunSayisi;
        float hucreYuksekligiPx = gHeight / satirSayisi;
        int j = xpos / hucreGenisligiPx;
        int i = ypos / hucreYuksekligiPx;
        cout << "Cursor Position at (" << xpos << " : " << ypos << endl;
        cout << "Cursor Position at (" << gWidth << " : " << gHeight << endl;
        cout << "Cursor Position at (" << hucreGenisligiPx << " : " << hucreYuksekligiPx << endl;
        cout << "Cursor Position at (" << i << " : " << j << endl;
        tavsanlar->patlat(i, j);

        //glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void mainLoop(GLFWwindow *window) {
    tavsanlar = new Tavsanlar();
    while (!glfwWindowShouldClose(window)) {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main(int argc, char **argv)   // Create Main Function For Bringing It All Together
{
    GLFWwindow *window;
    if (!glfwInit()) {
        exit(-1);
    }
    srand((unsigned) time(NULL));

    satirSayisi = stoi(argv[2]);
    sutunSayisi = stoi(argv[1]);
    hucreGenisligi = 20.0 / sutunSayisi;
    hucreYuksekligi = 20.0 / satirSayisi;
    auto objectDosyasi = argv[3];
    printf("satir: %d, sutun: %d, objectDosyasi: %s\n", satirSayisi, sutunSayisi, objectDosyasi);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int width = 640, height = 600;
    window = glfwCreateWindow(width, height, "Simple Example", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(-1);
    }


    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit()) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    char rendererInfo[512] = {0};
    strcpy(rendererInfo, (const char *) glGetString(GL_RENDERER));
    strcat(rendererInfo, " - ");
    strcat(rendererInfo, (const char *) glGetString(GL_VERSION));
    glfwSetWindowTitle(window, rendererInfo);

    init(objectDosyasi);

    glfwSetKeyCallback(window, keyboard);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetWindowSizeCallback(window, reshape);

    reshape(window, width, height); // need to call this once ourselves
    mainLoop(window); // this does not return unless the window is closed

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

