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


// matrix structure for projection
struct mat4x4
{
    // a 2d deimesional array
    float m[4][4] = { 0 };
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
    // lets create a projection matrix
    mat4x4 matProj;

    float fTheta = 0.0f;


    // function to do matrix vector multiplication
    // input 1 vector and get a differnet output vector and pass in the matrix
    void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m) 
    {
        o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
        o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
        o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
        float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3]; // I am impling that the 4th element  of the input vector is 1

        //now because we have 4d and we need to get back to 3d space we divide it by w
        if (w != 0.0f) 
        {
            o.x /= w;
            o.y /= w;
            o.z /= w;
        }
    }

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

        // Projection Matrix
        // Populating the projection Matrix (we only do this once as the field of view an ascept ratio of ourscreen arent going to change in this project)
        float fNear = 0.1f;
        float fFar = 1000.0f;
        float fFov = 90.0f;
        // grabed directly form the console
        float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
        float fFovRad = 1.0f / tanf(fFov *  0.5f / 180.0f * 3.14159f);//converted degress to radian

        matProj.m[0][0] = fAspectRatio * fFovRad;
        matProj.m[1][1] = fFovRad;
        matProj.m[2][2] = fFar / (fFar - fNear);
        matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        matProj.m[2][3] = 1.0f;
        matProj.m[3][3] = 0.0f;

        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override
    {

        // to clear the screen
        Fill(0,0,ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);


        mat4x4 matRotZ, matRotX;
        // to give the impresion that something is rotating we need an angle value that changes over time
        fTheta += 1.0f * fElapsedTime;
        
        // hard code 2 rotation matrix
        //rotation matrixs can be looked up on wikepedia
        
        // Rotation Z
        matRotZ.m[0][0] = cosf(fTheta);
        matRotZ.m[0][1] = sinf(fTheta);
        matRotZ.m[1][0] = -sinf(fTheta);
        matRotZ.m[1][1] = cosf(fTheta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        // Rotation X
        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(fTheta * 0.5f);
        matRotX.m[1][2] = sinf(fTheta * 0.5f);
        matRotX.m[2][1] = -sinf(fTheta * 0.5f);
        matRotX.m[2][2] = cosf(fTheta * 0.5f);
        matRotX.m[3][3] = 1;


        // Draw Triangles
        // because out tringles are neatly contained inside a vector inside a mesh I can use an auto for loop to iterate through them all 
        // but ofcourse its not this simple the objects exist in 3d space but the screen is 2d space
        // so we need to come up with a way of condencing that 3d space to a 2d space and this is called projection
        // Define our screen
        // beacause screens come in all spaces and sizes its useful to reduce the 3d objects into a normilized screen space
        // we want to scale movements with in the screen space apprioprately using aspect ratio
        // normilizing the screen also has the advantage that aanything above +1 ot below -1 wont be drawn onto the screen
        // Field of view
        // we need a scalling factor that relates to the field of view (tangent function involved )
        // choosing scaling coeffiencts/factor 
        // Matrics multiplication
        // Projection matrix (black box highly customisable and useable)
        // we normilze x, y ,z ()
    
        for (auto tri : meshcube.tris) 
        {
            
            triangle triprojected, triTranslated, triRotatedZ, triRotatedZX; // were we store the result of out matrix multiplication so as to not upset the original triangle

            //Rotation
            // rotate the original triangle in the z axis
            MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
            MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
            MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

            // rotate the original triangle in the x axis
            MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
            MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
            MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);


            // Offset into the screen
            triTranslated = triRotatedZX;
            triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
            triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
            triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

            // Calculate the tringles normal
            // after translate the triangle into world space but before projrction so we are still in 3d space
            vec3d normal, line1, line2;
            line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
            line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
            line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

            line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
            line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
            line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

            // normal cross product of the 2 lines
            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;
            
            //normilize the normal (make the normal a unit vector)
            float l = sqrtf(normal.x*normal.x + normal.y * normal.y + normal.z * normal.z);
            normal.x /= l; normal.y /= l; normal.z /= l;

            // only draw and scale if we can see the triangle
            if (normal.z < 0) {            // Proejct triangles from 3D ---> 2D
                MultiplyMatrixVector(triTranslated.p[0], triprojected.p[0], matProj);// we can use the triangle directly and need to refrence the vertex inside
                MultiplyMatrixVector(triTranslated.p[1], triprojected.p[1], matProj);
                MultiplyMatrixVector(triTranslated.p[2], triprojected.p[2], matProj);

                //scale into view
                triprojected.p[0].x += 1.0f;
                triprojected.p[0].y += 1.0f;

                triprojected.p[1].x += 1.0f;
                triprojected.p[1].y += 1.0f;

                triprojected.p[2].x += 1.0f;
                triprojected.p[2].y += 1.0f;

                // changedd 0.5f to 0.25f discovered through trail and error 
                triprojected.p[0].x *= 0.25f * (float)ScreenWidth();
                triprojected.p[0].y *= 0.25f * (float)ScreenHeight();

                triprojected.p[1].x *= 0.25f * (float)ScreenWidth();
                triprojected.p[1].y *= 0.25f * (float)ScreenHeight();

                triprojected.p[2].x *= 0.25f * (float)ScreenWidth();
                triprojected.p[2].y *= 0.25f * (float)ScreenHeight();


                DrawTriangle(triprojected.p[0].x, triprojected.p[0].y,
                    triprojected.p[1].x, triprojected.p[1].y,
                    triprojected.p[2].x, triprojected.p[2].y,
                    PIXEL_SOLID, FG_WHITE);
            }
        }

        return true;
    }
};

int main()
{
    // instance of the class
    olcEngine3D demo;
    // instance of the console
    //if (demo.ConstructConsole(256, 240, 4, 4))
    if (demo.ConstructConsole(128, 120, 4, 4))
    //if (demo.ConstructConsole(64, 60, 4, 4))
        //if we cn successful construct the console start it 
        demo.Start();

    return 0;
}

