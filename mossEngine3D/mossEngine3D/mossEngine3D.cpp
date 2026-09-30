//mossEngine 3d
//simple 3d graphics engine in c++
//https://github.com/trentstauff/ShootPlusPlus/blob/main/olcConsoleGameEngine.h

#include "olcConsoleGameEngine.h" // all this really does is accepting input from a user and display things on a 2d screen
using namespace std;

//structure vec 3d holding 3 float values representing a 3d vector (cordinated in 3d space)
struct vec3d
{
    float x, y, z;
};

// structure that groups together 3 vec3ds
struct triangle
{
    vec3d p[3];
};

// represents the object groups togther triangles
struct mesh
{
    vector<triangle> tris;
};

//we need to create a sub class of the olcConsoleGameEngine
//which inherits from the console game engine
class olcEngine3D : public olcConsoleGameEngine
{
//a qiuck constructer
public:
    olcEngine3D() {
        //name the application
        m_sAppName = L"3D Demo";
    }

private:
    mesh meshcube;

// overriding 2 function
public:
    bool OnUserCreate() override 
    {
        //populate mesh with the vertex data to define a cube made of triangle
        // using initializer lists I define the vertices manually and group them  appripriatly
        // initializer list in an initilazer list (a sub initializer list)
        // keep the cube simple and define it as a unit cube (each side of the cube is 1 unit long and the origin of the cube is at 0,0s)
        // the order of the points we define the triangle in are important (I use a clockwise order)
        meshcube.tris = {

            //south
            {0.0f, 0.0f, 0.0f,            0.0f, 1.0f, 0.0f,            1.0f, 1.0f, 0.0f,},
            {0.0f, 0.0f, 0.0f,            1.0f, 1.0f, 0.0f,            1.0f, 0.0f, 0.0f,},

            //East
            {1.0f, 0.0f, 0.0f,            1.0f, 1.0f, 0.0f,            1.0f, 1.0f, 1.0f,},
            {1.0f, 0.0f, 0.0f,            1.0f, 1.0f, 1.0f,            1.0f, 0.0f, 1.0f,},

            // North
            {1.0f, 0.0f, 1.0f,            1.0f, 1.0f, 1.0f,            0.0f, 1.0f, 1.0f,},
            {1.0f, 0.0f, 1.0f,            0.0f, 1.0f, 1.0f,            0.0f, 0.0f, 1.0f,},

            // West
            {0.0f, 0.0f, 1.0f,            0.0f, 1.0f, 1.0f,            0.0f, 1.0f, 0.0f,},
            {0.0f, 0.0f, 1.0f,            0.0f, 1.0f, 0.0f,            0.0f, 0.0f, 0.0f,},

            // Top
            {0.0f, 1.0f, 0.0f,            0.0f, 1.0f, 1.0f,            1.0f, 1.0f, 1.0f,},
            {0.0f, 1.0f, 0.0f,            1.0f, 1.0f, 1.0f,            1.0f, 1.0f, 0.0f,},

            // Bottom
            {1.0f, 0.0f, 1.0f,            0.0f, 0.0f, 1.0f,            0.0f, 0.0f, 0.0f,},
            {1.0f, 0.0f, 1.0f,            0.0f, 0.0f, 0.0f,            1.0f, 0.0f, 0.0f,},

        };


        return true;
    }
    bool OnUserUpdate(float fElaspedTime) override
    {

        // to clear the screen
        Fill(0,0,ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

        // Draw Triangles
        // because out tringles are neatly contained inside a vector inside a mesh I can use an auto for loop to iterate through them all 
        // but ofcourse its not this simple the objects exist in 3d space but the scereen is 2d space
        for (auto tri : meshcube.tris) 
        {

        };

        return true;
    }
};

int main()
{
    // instance of the class
    olcEngine3D demo;
    // instance of the console
    if (demo.ConstructConsole(256, 240, 4, 4))
        //if we cn successful construct the console start it 
        demo.Start();

    return 0;
}

