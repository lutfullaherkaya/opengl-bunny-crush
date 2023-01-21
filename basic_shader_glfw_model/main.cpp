#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h>   // The GL Header File
#include <GL/gl.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

/*
 * 2 Specifications
1. YAPILDI The executable file for this assignment will be named “hw3” with a window size of 640 × 600.
This is the file that you will run to start the game. The program gets grid shape and the
object file name as command line argument.
2. YAPILDI The game board will have a grid size given as command line argument. For example, “10 5”
means that the game board will be a 10x5 grid of squares, each containing an object.
3. YAPILDI Your program will parse the object file and use this information to create the objects for the
game. The object file contains information about the shape and appearance of the object,
such as the vertices and faces that make up the object’s 3D model. Your program will use
this information to create a 3D model of the object that can be displayed in the game.
4. YAPILDI You will resize the objects considering the grid size to fit into the window properly.
5. YAPILDI The scene will be rendered using an orthographic projection, with the specified left, right,
bottom, top, near, and far parameters of (-10, 10, -10, 10, -20, 20). Orthographic projection
is a way of representing 3D objects on a 2D surface, where the size and shape of the objects
are not affected by their distance from the viewer. The left, right, bottom, top, near, and far
parameters specify the dimensions of the viewing volume, or the region of 3D space that is
visible in the game.
6. YAPILDI At each frame of the game, the objects will rotate around the vector (0, 1, 0) at an angle of
0.5 degrees. A frame is a single image displayed by the game. The vector (0, 1, 0) specifies an
axis of rotation, and the angle of 0.5 degrees specifies the amount of rotation that will occur
around this axis at each frame. This will cause the objects to rotate slightly at each frame of
the game.
7. DAHA YAPILMADI If there are more than three objects of the same color in a row or column, they will explode
and the objects above them will slide down to fill the empty space. For example, if there are
four blue objects in a row, they will all explode and the objects above them will slide down
to fill the empty spaces.
8. YAPILDI New random color objects will be added at the top of the grid and will slide down to their
correct positions using an animation in which the y position of the bunny object is decreased
by 0.05 at each frame. When a bunny explodes and leaves an empty space, a new bunny will
be added at the top of the grid. The new bunny will start at the top of the grid and slide
down to its correct position using an animation in which the y position of the bunny object
is decreased by 0.05 at each frame.
9. YAPILDI If a bunny explodes, it will be scaled up by 0.01 in all directions at each frame until it reaches
1.5 times its original size, at which point it will be deleted. This will create an animation in
which the exploding bunny grows in size until it reaches 1.5 times its original size, at which
point it will be removed from the game.
10. YAPILDI The player can interact with the game using the mouse to manually explode bunnies and
change the arrangement of the bunnies on the grid. For example, the player can click on a
bunny to manually cause it to explode and match with other bunnies of the same color.
11. YAPILDI MATCHING HARIC Mouse interaction and color matching will be disabled until all animations have completed.
This means that the player will not be able to interact with the game using the mouse or
the game won’t match new objects until all animations, such as the slide animation for new
bunnies and the explosion animation for matching bunnies, have finished.
12. DAHA YAPILMADI Each object in the game will have its own point light source, located at the same x and y
coordinates as the object and at a z coordinate of 1. The position of the light will move
together with the object during the animation. A point light is a light source that emits light
in all directions from a single point. By giving each object its own point light source, you can
create a more realistic lighting effect in the game. In order to get same view for each of the
objects, you will accept as eye position is at the same point with the light source.
13. DAHA YAPILMADI The game will keep track of the number of moves made by the player, as well as the number
of matched objects and the score earned. The number of moves is the number of times the
player has manually exploded a bunny. The number of matched objects is the total number
of objects that have exploded as part of a match. The score is the total number of points
earned by the player.
14. These values will be displayed at the bottom of the window. The number of moves, the
number of matched objects, and the score will all be displayed at the bottom of the game
window, so that the player can see their progress as they play.
15. YAPILDI If the user presses the ’R’ key, the game will restart with a new random arrangement of
objects. If the user presses the ’ESC’ key, the game will close. This will allow the player to
easily restart the game or exit the game if they wish.
 */



using namespace std;

GLuint gProgram;
GLuint gProgram2ForTxt;
int gWidth, gHeight;
int satirSayisi, sutunSayisi;
GLfloat objeGenisligi, objeYuksekligi;
float hucreGenisligi;
float hucreYuksekligi;
GLFWwindow *window;


struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};
std::map<GLchar, Character> Characters;

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

GLuint gVertexAttribBuffer, gTextVBO, gIndexBuffer;
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

void createVSForTxt(GLuint &program, const string &filename) {
    string shaderSource;

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

    glAttachShader(program, vs);
}

void createFSForTxt(GLuint &program, const string &filename) {
    string shaderSource;

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

    glAttachShader(program, fs);
}

void initShaders() {
    gProgram = glCreateProgram();
    gProgram2ForTxt = glCreateProgram();


    createVSForTxt(gProgram, "vert.glsl");
    createFSForTxt(gProgram, "frag.glsl");
    createVSForTxt(gProgram2ForTxt, "vert_text.glsl");
    createFSForTxt(gProgram2ForTxt, "frag_text.glsl");

    glBindAttribLocation(gProgram2ForTxt, 2, "vertex");


    glLinkProgram(gProgram);
    glLinkProgram(gProgram2ForTxt);
    glUseProgram(gProgram);
}

void initFonts(int windowWidth, int windowHeight) {
    // Set OpenGL options
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -20.0f, 20.0f);
    glUseProgram(gProgram2ForTxt);
    glUniformMatrix4fv(glGetUniformLocation(gProgram2ForTxt, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/liberation/LiberationSerif-Italic.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    //
    // Configure VBO for texture quads
    //
    glGenBuffers(1, &gTextVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    initFonts(gWidth, gHeight);
    initVBO();
}

void drawModel() {
    glUseProgram(gProgram);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));

    glDrawElements(GL_TRIANGLES, gFaces.size() * 3, GL_UNSIGNED_INT, 0);
}

void renderText(const std::string &text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
    // Activate corresponding render state
    glUseProgram(gProgram2ForTxt);
    glUniform3f(glGetUniformLocation(gProgram2ForTxt, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
                {xpos,     ypos + h, 0.0, 0.0},
                {xpos,     ypos,     0.0, 1.0},
                {xpos + w, ypos,     1.0, 1.0},

                {xpos,     ypos + h, 0.0, 0.0},
                {xpos + w, ypos,     1.0, 1.0},
                {xpos + w, ypos + h, 1.0, 0.0}
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices),
                        vertices); // Be sure to use glBufferSubData and not glBufferData

        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)

        x += (ch.Advance >> 6) *
             scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

class Tavsan {
public:
    GLfloat x;
    GLfloat y;
    GLfloat olmasiGerekenY;
    std::vector<GLfloat> renk;
    bool animasyonOynuyor;
    bool patliyor = false;
    bool patladi = false;
    bool kaydi = false;
    GLfloat patlamaScale;

    Tavsan() : animasyonOynuyor(false), patlamaScale(1.0) {

    }

    bool kayiyor() {
        return olmasiGerekenY < y;
    }

    void ciz(float angle) {
        // todo: color
        glLoadIdentity();
        glTranslatef(x, y, -10);

        GLfloat olmasiGerekenGenislik = 20.0 / sutunSayisi;
        GLfloat olmasiGerekenYukseklik = 20.0 / satirSayisi;
        GLfloat genislikScale = (olmasiGerekenGenislik / objeGenisligi) * 0.67;
        GLfloat yukseklikScale = (olmasiGerekenYukseklik / objeYuksekligi) * 0.67;
        glScalef(genislikScale, yukseklikScale, 1);

        if (patliyor) {
            patlamaScale += 0.01;
            if (patlamaScale >= 1.5) {
                // patladi
                patladi = true;
                patliyor = false;
            }
        }

        if (kayiyor()) {
            animasyonOynuyor = true;
            y -= 0.05;
            if (!kayiyor()) {
                kaydi = true;
                animasyonOynuyor = false;
            }
        }

        glScalef(patlamaScale, patlamaScale, 1);
        glRotatef(angle, 0, 1, 0);
        if (!patladi) {
            drawModel();
        }

    }

    void patlamayiBaslat() {
        if (!animasyonOynuyor) {
            animasyonOynuyor = true;
            patliyor = true;
        }
    }
};

class Tavsanlar {
public:
    std::vector<std::vector<Tavsan>> tavsanlar;
    std::vector<std::vector<GLfloat>> colors = {
            {1.0, 0,   0}, // red
            {0,   1.0, 0}, // green
            {0,   0,   1.0}, // blue
            {1.0, 1.0, 0}, // yellow
            {1.0, 0,   1.0} // purple
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
                tavsanlar[i][j].olmasiGerekenY = hucreY;
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

    bool herhangiAnimasyonOynuyor() {
        for (auto &satir: tavsanlar) {
            for (auto &tavsan: satir) {
                if (tavsan.animasyonOynuyor) {
                    return true;
                }
            }
        }
        return false;
    }

    void ciz(float angle) {

        for (int j = 0; j < tavsanlar[0].size(); ++j) {
            std::vector<Tavsan> patlamayanlar;
            for (int i = tavsanlar.size() - 1; i >= 0; --i) {
                auto &tavsan = tavsanlar[i][j];
                if (!tavsan.patladi) {
                    patlamayanlar.push_back(tavsan);
                }
            }
            for (int i = patlamayanlar.size(), rastgeleTavsanI = 0; i < satirSayisi; ++i, rastgeleTavsanI++) {
                Tavsan rastgeleTavsan;
                rastgeleTavsan.renk = colors[rand() % 5];
                rastgeleTavsan.x = getX(j);
                rastgeleTavsan.y = getY(-(rastgeleTavsanI + 1));
                rastgeleTavsan.olmasiGerekenY = getY(i);

                patlamayanlar.push_back(rastgeleTavsan);
            }
            for (int i = tavsanlar.size() - 1; i >= 0; --i) {
                tavsanlar[i][j] = patlamayanlar[patlamayanlar.size() - i - 1];
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
        if (!herhangiAnimasyonOynuyor()) {
            tavsanlar[i][j].patlamayiBaslat();
        }

    }
};

Tavsanlar *tavsanlar = nullptr;

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

    assert(glGetError() == GL_NO_ERROR);

    renderText("CENG 477 - 2022", 0, 0, 1, glm::vec3(0, 1, 1));

    assert(glGetError() == GL_NO_ERROR);

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
    } else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        delete tavsanlar;
        tavsanlar = new Tavsanlar();
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

