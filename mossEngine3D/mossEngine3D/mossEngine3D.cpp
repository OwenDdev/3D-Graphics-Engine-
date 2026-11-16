//mossEngine 3d
//simple 3d graphics engine in c++
//https://github.com/trentstauff/ShootPlusPlus/blob/main/olcConsoleGameEngine.h

#include "olcConsoleGameEngine.h" // all this really does is accepting input from a user and display things on a 2d screen
// file handling functions
#include <fstream>
#include <strstream>
#include <algorithm>
using namespace std;

//structure vec 3d holding 3 float values representing a 3d vector (cordinated in 3d space)
struct vec3d
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;
};

// structure that groups together 3 vec3ds
struct triangle
{
    vec3d p[3];

    // store the symbols that represent the color for the triangle
    wchar_t sym;
    short col;
};

// represents the object groups togther triangles
struct mesh
{
    vector<triangle> tris;

    bool LoadFromObjectFile(string sFilename)
    {
        ifstream f(sFilename);
        // if file files to open return false
        if (!f.is_open()) {
            return false;
        }

        // Locaal cache of verts
        vector<vec3d> verts;

        while (!f.eof()) 
        {
            char line[128];
            f.getline(line, 128);

            strstream s;
            s << line;

            char junk;

            if (line[0] == 'v')
            {
                vec3d v;
                s >> junk >> v.x >> v.y >> v.z;
                verts.push_back(v);
            }

            if(line[0] == 'f')
            {
                int f[3];
                s >> junk >> f[0] >> f[1] >> f[2];
                tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
            }
        }
        return true;
    }
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

    // vector respresenting camera postion in 3d space
    vec3d vCamera;
    //vec3d vCamera = { 0.0f, 0.0f, 0.0f };
    vec3d vLookDir;

    float fYaw;

    float fTheta = 0.0f;


    // function to do matrix vector multiplication
    // input 1 vector and get a differnet output vector and pass in the matrix
    //void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m) 
    //{
    //    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    //    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    //    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    //    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3]; // I am impling that the 4th element  of the input vector is 1

    //    //now because we have 4d and we need to get back to 3d space we divide it by w
    //    if (w != 0.0f) 
    //    {
    //        o.x /= w;
    //        o.y /= w;
    //        o.z /= w;
    //    }
    //}

    // utitlity function to replace MultiplyMatricVector function
    vec3d Matrix_MultiplyVector(mat4x4& m, vec3d& i)
    {
        vec3d v;
        v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
        v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
        v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
        v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
        return v;
    }
     
    // Matix utility functions 
    mat4x4 Matrix_MakeIdentity()
    {
        mat4x4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    mat4x4 Matrix_MakeRotationX(float fAngleRad)
    {
        mat4x4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = cosf(fAngleRad);
        matrix.m[1][2] = sinf(fAngleRad);
        matrix.m[2][1] = -sinf(fAngleRad);
        matrix.m[2][2] = cosf(fAngleRad);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    mat4x4 Matrix_MakeRotationY(float fAngleRad)
    {
        mat4x4 matrix;
        matrix.m[0][0] = cosf(fAngleRad);
        matrix.m[0][2] = sinf(fAngleRad);
        matrix.m[2][0] = -sinf(fAngleRad);
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = cosf(fAngleRad);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    mat4x4 Matrix_MakeRotationZ(float fAngleRad)
    {
        mat4x4 matrix;
        matrix.m[0][0] = cosf(fAngleRad);
        matrix.m[0][1] = sinf(fAngleRad);
        matrix.m[1][0] = -sinf(fAngleRad);
        matrix.m[1][1] = cosf(fAngleRad);
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    mat4x4 Matrix_MakeTranslation(float x, float y, float z)
    {
        mat4x4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        matrix.m[3][0] = x;
        matrix.m[3][1] = y;
        matrix.m[3][2] = z;
        return matrix;
    }

    mat4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
    {
        float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
        mat4x4 matrix;
        matrix.m[0][0] = fAspectRatio * fFovRad;
        matrix.m[1][1] = fFovRad;
        matrix.m[2][2] = fFar / (fFar - fNear);
        matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        matrix.m[2][3] = 1.0f;
        matrix.m[3][3] = 0.0f;
        return matrix;
    }

    // matrix matrix multipliction 
    mat4x4 Matrix_MultiplyMatrix(mat4x4& m1, mat4x4& m2)
    {
        mat4x4 matrix;
        for (int c = 0; c < 4; c++)
            for (int r = 0; r < 4; r++)
                matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
        return matrix;
    }

    // point at marix for the camera 
    // paramaters 
    // position vector were the object should be 
    // target vector the forword vector for an object 
    mat4x4 Matrix_PiontAt (vec3d &pos, vec3d &target, vec3d &up)
    {
        // Calculate new forward direction 
        vec3d newForward = Vector_Sub(target, pos);
        newForward = Vector_Normalise(newForward);

        // Calculate new up direction
        vec3d a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
        vec3d newUp = Vector_Sub(up, a);
        newUp = Vector_Normalise(newUp);

        // New Right direction is easy, its just cross product
        vec3d newRight = Vector_CrossProduct(newUp, newForward);

        // Construct Dimensioning and Translation Matrix
        mat4x4 matrix;
        matrix.m[0][0] = newRight.x;    matrix.m[0][1] = newRight.y;    matrix.m[0][2] = newRight.z;
        matrix.m[1][0] = newUp.x;       matrix.m[1][1] = newUp.y;       matrix.m[1][2] = newUp.z;
        matrix.m[2][0] = newForward.x;  matrix.m[2][1] = newForward.y;  matrix.m[2][2] = newForward.z;
        matrix.m[3][0] = pos.x;         matrix.m[3][1] = pos.y;         matrix.m[3][2] = pos.z;
        return matrix;
    }

    mat4x4 Matrix_QuickInverse(mat4x4& m) // Only for Rotation/Translation Matrices
    {
        mat4x4 matrix;
        matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
        matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
        matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
        matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
        matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
        matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }


    // utility functions part 3
    // adds two vectors 
    vec3d Vector_Add(vec3d &v1, vec3d &v2) 
    {
        return {v1.x + v2.x,v1.y + v2.y, v1.z + v2.z};
    }

    // utility functions part 3
    // subtracts two vectors 
    vec3d Vector_Sub(vec3d& v1, vec3d& v2)
    {
        return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
    }

    // scaling function multiplies two vectors 
    vec3d Vector_Mul(vec3d& v1, float k)
    {
        return { v1.x * k, v1.y * k, v1.z * k };
    }

    // scling function that divides two vectors 
    vec3d Vector_Div(vec3d& v1, float k)
    {
        return { v1.x / k, v1.y / k, v1.z / k };
    }
    
    float Vector_DotProduct(vec3d &v1, vec3d &v2) 
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    float Vector_Length(vec3d& v)
    {
        return sqrtf(Vector_DotProduct(v, v));
    }

    vec3d Vector_Normalise(vec3d& v)
    {
        float l = Vector_Length(v);
        return { v.x / l, v.y / l , v.z / l};
    }
    
    vec3d Vector_CrossProduct(vec3d& v1, vec3d& v2) 
    {
        vec3d v;
        v.x = v1.y * v2.z - v1.z * v2.y;
        v.y = v1.z * v2.x - v1.x * v2.z;
        v.z = v1.x * v2.y - v1.y * v2.x;
        return v;
    }



    // Taken From Command Line Webcam Video
    CHAR_INFO GetColour(float lum)
    {
        short bg_col, fg_col;
        wchar_t sym;
        int pixel_bw = (int)(13.0f * lum);
        switch (pixel_bw)
        {
        case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

        case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
        case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
        case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
        case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

        case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
        case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
        case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
        case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

        case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
        case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
        case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
        case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
        default:
            bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
        }

        CHAR_INFO c;
        c.Attributes = bg_col | fg_col;
        c.Char.UnicodeChar = sym;
        return c;
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
        //meshcube.tris = {

        //    //south
        //    {0.0f, 0.0f, 0.0f,            0.0f, 1.0f, 0.0f,            1.0f, 1.0f, 0.0f,},
        //    {0.0f, 0.0f, 0.0f,            1.0f, 1.0f, 0.0f,            1.0f, 0.0f, 0.0f,},

        //    // East
        //    {1.0f, 0.0f, 0.0f,            1.0f, 1.0f, 0.0f,            1.0f, 1.0f, 1.0f,},
        //    {1.0f, 0.0f, 0.0f,            1.0f, 1.0f, 1.0f,            1.0f, 0.0f, 1.0f,},

        //    // North
        //    {1.0f, 0.0f, 1.0f,            1.0f, 1.0f, 1.0f,            0.0f, 1.0f, 1.0f,},
        //    {1.0f, 0.0f, 1.0f,            0.0f, 1.0f, 1.0f,            0.0f, 0.0f, 1.0f,},

        //    // West
        //    {0.0f, 0.0f, 1.0f,            0.0f, 1.0f, 1.0f,            0.0f, 1.0f, 0.0f,},
        //    {0.0f, 0.0f, 1.0f,            0.0f, 1.0f, 0.0f,            0.0f, 0.0f, 0.0f,},

        //    // Top
        //    {0.0f, 1.0f, 0.0f,            0.0f, 1.0f, 1.0f,            1.0f, 1.0f, 1.0f,},
        //    {0.0f, 1.0f, 0.0f,            1.0f, 1.0f, 1.0f,            1.0f, 1.0f, 0.0f,},

        //    // Bottom
        //    {1.0f, 0.0f, 1.0f,            0.0f, 0.0f, 1.0f,            0.0f, 0.0f, 0.0f,},
        //    {1.0f, 0.0f, 1.0f,            0.0f, 0.0f, 0.0f,            1.0f, 0.0f, 0.0f,},

        //   

        //};

      /*  if (!meshcube.LoadFromObjectFile("teapot.obj"))
        {
            wcout << L"Failed to load VideoShip.obj!" << endl;
            return false;
        }

        wcout << L"VideoShip.obj loaded successfully!" << endl;*/

        //meshcube.LoadFromObjectFile("teapot.obj");
        // meshcube.LoadFromObjectFile("Ship.obj");

        // Projection Matrix manually
        // Populating the projection Matrix (we only do this once as the field of view an ascept ratio of ourscreen arent going to change in this project)
        //float fNear = 0.1f;
        //float fFar = 1000.0f;
        //float fFov = 90.0f;
        //// grabed directly form the console
        //float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
        //float fFovRad = 1.0f / tanf(fFov *  0.5f / 180.0f * 3.14159f);//converted degress to radian

        //matProj.m[0][0] = fAspectRatio * fFovRad;
        //matProj.m[1][1] = fFovRad;
        //matProj.m[2][2] = fFar / (fFar - fNear);
        //matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        //matProj.m[2][3] = 1.0f;
        //matProj.m[3][3] = 0.0f;

        // object that represents the axis to help with camera orientation 
        meshcube.LoadFromObjectFile("axis.obj");

         // Projection Matrix 
        matProj = Matrix_MakeProjection(90.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.1f, 1000.0f);
        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override
    {

        if(GetKey(VK_UP).bHeld)
            vCamera.y += 8.0f * fElapsedTime;	// Travel Upwards

        if (GetKey(VK_DOWN).bHeld)
            vCamera.y -= 8.0f * fElapsedTime;	// Travel Downwards

        // Dont use these two in FPS mode, it is confusing :P
        if (GetKey(VK_LEFT).bHeld)
            vCamera.x -= 8.0f * fElapsedTime;	// Travel Along X-Axis

        if (GetKey(VK_RIGHT).bHeld)
            vCamera.x += 8.0f * fElapsedTime;	// Travel Along X-Axis

        vec3d vForward = Vector_Mul(vLookDir, 8.0f * fElapsedTime);

        // Standard FPS Control scheme, but turn instead of strafe
        if (GetKey(L'W').bHeld)
            vCamera = Vector_Add(vCamera, vForward);

        if (GetKey(L'S').bHeld)
            vCamera = Vector_Sub(vCamera, vForward);

        if (GetKey(L'A').bHeld)
            fYaw -= 2.0f * fElapsedTime;

        if (GetKey(L'D').bHeld)
            fYaw += 2.0f * fElapsedTime;

        // to clear the screen
        Fill(0,0,ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);


        mat4x4 matRotZ, matRotX;
        // to give the impresion that something is rotating we need an angle value that changes over time
        // temperoraly removed so the world doesnt spin round while i work on the camera 
        // fTheta += 1.0f * fElapsedTime;
        
        // hard code 2 rotation matrix
        //rotation matrixs can be looked up on wikepedia
        
        //// Rotation Z manually
        //matRotZ.m[0][0] = cosf(fTheta);
        //matRotZ.m[0][1] = sinf(fTheta);
        //matRotZ.m[1][0] = -sinf(fTheta);
        //matRotZ.m[1][1] = cosf(fTheta);
        //matRotZ.m[2][2] = 1;
        //matRotZ.m[3][3] = 1;

        matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);

        //// Rotation X manually
        //matRotX.m[0][0] = 1;
        //matRotX.m[1][1] = cosf(fTheta * 0.5f);
        //matRotX.m[1][2] = sinf(fTheta * 0.5f);
        //matRotX.m[2][1] = -sinf(fTheta * 0.5f);
        //matRotX.m[2][2] = cosf(fTheta * 0.5f);
        //matRotX.m[3][3] = 1;

        matRotX = Matrix_MakeRotationX(fTheta);

        mat4x4 matTrans;
        matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 16.0f);

        mat4x4 matWorld;
        matWorld = Matrix_MakeIdentity();	// Form World Matrix
        matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX); // Transform by rotation
        matWorld = Matrix_MultiplyMatrix(matWorld, matTrans); // Transform by translation

        // vLookDir = { 0, 0, 1 };
        vec3d vUp = { 0, 1, 0 };
        // vec3d vTarget = Vector_Add(vCamera, vLookDir);
        vec3d vTarget = { 0, 0, 1 };
        mat4x4 matCameraRot = Matrix_MakeRotationY(fYaw);
        vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
        vTarget = Vector_Add(vCamera, vLookDir);

        mat4x4 matCamera = Matrix_PiontAt(vCamera, vTarget, vUp);

        // marke view matrix from camera 
        mat4x4  matView = Matrix_QuickInverse(matCamera);


        vector<triangle> vecTrianglesToRaster;


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
            
            // triangle triprojected, triTranslated, triRotatedZ, triRotatedZX; // were we store the result of out matrix multiplication so as to not upset the original triangle
            triangle triprojected, triTransformed, triViewed; // were we store the result of out matrix multiplication so as to not upset the original triangle

            ////Rotation
            //// rotate the original triangle in the z axis
            //MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
            //MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
            //MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

            //// rotate the original triangle in the x axis
            //MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
            //MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
            //MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

            //// Offset into the screen
            //triTranslated = triRotatedZX;
            //triTranslated.p[0].z = triRotatedZX.p[0].z + 8.0f;
            //triTranslated.p[1].z = triRotatedZX.p[1].z + 8.0f;
            //triTranslated.p[2].z = triRotatedZX.p[2].z + 8.0f;
            
            // World Matrix Transform
            triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
            triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
            triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);

            //// Calculate the tringles normal
            //// after translate the triangle into world space but before projrction so we are still in 3d space
            //vec3d normal, line1, line2;
            //line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
            //line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
            //line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

            //line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
            //line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
            //line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

            //// normal cross product of the 2 lines
            //normal.x = line1.y * line2.z - line1.z * line2.y;
            //normal.y = line1.z * line2.x - line1.x * line2.z;
            //normal.z = line1.x * line2.y - line1.y * line2.x;
            
            // Calculate triangle Normal
            vec3d normal, line1, line2;

            // Get lines either side of triangle
            line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
            line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);

            // Take cross product of lines to get normal to triangle surface
            normal = Vector_CrossProduct(line1, line2);

            // You normally need to normalise a normal!
            normal = Vector_Normalise(normal);

            // normilize the normal (make the normal a unit vector)
            float l = sqrtf(normal.x*normal.x + normal.y * normal.y + normal.z * normal.z);
            normal.x /= l; normal.y /= l; normal.z /= l;

            // only draw and scale if we can see the triangle
            //if (normal.z < 0) {      
            //if (normal.x * (triTranslated.p[0].x - vCamera.x) +
            //    normal.y * (triTranslated.p[0].y - vCamera.y) +
            //    normal.z * (triTranslated.p[0].z - vCamera.z) < 0.0f)

            // Get ray from triangle to camera
            vec3d vCameraRay = Vector_Sub(triTransformed.p[0], vCamera);

            // If ray is aligned with normal, then triangle is visible
            if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
            {
                //// Illumintion (Basic lighting)
                //vec3d light_direction = { 0.0f, 0.0f, -1.0f };
                //// a triangle is more light the normal that its normal is alligned with the light direction
                ////normilize the light direction (make the light direction a unit vector)
                //float l = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
                //light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

                //// calculate the dot product of the light direction and normal
                //float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;

                // Illumination
                vec3d light_direction = { 0.0f, 1.0f, -1.0f };
                light_direction = Vector_Normalise(light_direction);

                // How "aligned" are light direction and triangle surface normal?
                float dp = max(0.1f, Vector_DotProduct(light_direction, normal));

                // console specific 
                CHAR_INFO c = GetColour(dp);
                triTransformed.col = c.Attributes;
                triTransformed.sym = c.Char.UnicodeChar;


                // Convert World Space --> View Space
                triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
                triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
                triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);


                // Proejct triangles from 3D ---> 2D
                triprojected.p[0] = Matrix_MultiplyVector(matProj, triViewed.p[0]);// we can use the triangle directly and need to refrence the vertex inside
                triprojected.p[1] = Matrix_MultiplyVector(matProj, triViewed.p[1]);
                triprojected.p[2] = Matrix_MultiplyVector(matProj, triViewed.p[2]);
                triprojected.col = triTransformed.col;
                triprojected.sym = triTransformed.sym;

                // normalize manually
                triprojected.p[0] = Vector_Div(triprojected.p[0], triprojected.p[0].w);
                triprojected.p[1] = Vector_Div(triprojected.p[1], triprojected.p[1].w);
                triprojected.p[2] = Vector_Div(triprojected.p[2], triprojected.p[2].w);
          
                // X/Y are inverted so put them back
                triprojected.p[0].x *= -1.0f;
                triprojected.p[1].x *= -1.0f;
                triprojected.p[2].x *= -1.0f;
                triprojected.p[0].y *= -1.0f;
                triprojected.p[1].y *= -1.0f;
                triprojected.p[2].y *= -1.0f;


                // Offset verts into visible normalised space
                vec3d vOffsetView = { 1,1,0 };

                //scale into view
                triprojected.p[0] = Vector_Add(triprojected.p[0], vOffsetView);
                triprojected.p[1] = Vector_Add(triprojected.p[1], vOffsetView);
                triprojected.p[2] = Vector_Add(triprojected.p[2], vOffsetView);

                //scale into view
                /*triprojected.p[0].x += 1.0f;
                triprojected.p[0].y += 1.0f;

                triprojected.p[1].x += 1.0f;
                triprojected.p[1].y += 1.0f;

                triprojected.p[2].x += 1.0f;
                triprojected.p[2].y += 1.0f;*/

                // changed 0.5f to 0.25f discovered through trail and error 

                triprojected.p[0].x *= 0.5f * (float)ScreenWidth();
                triprojected.p[0].y *= 0.5f * (float)ScreenHeight();

                triprojected.p[1].x *= 0.5f * (float)ScreenWidth();
                triprojected.p[1].y *= 0.5f * (float)ScreenHeight();

                triprojected.p[2].x *= 0.5f * (float)ScreenWidth();
                triprojected.p[2].y *= 0.5f * (float)ScreenHeight();


                // Store triangle for sorting
                vecTrianglesToRaster.push_back(triprojected);

                
            }
        }

        // sort triangles from back to front
        sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2) 
        {
                float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
                return z1 > z2;
         });

        for(auto &triprojected : vecTrianglesToRaster)
        {
            // Rasterize triangle
            FillTriangle(triprojected.p[0].x, triprojected.p[0].y,
                    triprojected.p[1].x, triprojected.p[1].y,
                    triprojected.p[2].x, triprojected.p[2].y,
                    triprojected.sym, triprojected.col);


                //DrawTriangle(triprojected.p[0].x, triprojected.p[0].y,
                //    triprojected.p[1].x, triprojected.p[1].y,
                //    triprojected.p[2].x, triprojected.p[2].y,
                //    PIXEL_SOLID, FG_BLACK);
        }
        return true;
    }
};

int main()
{
    // instance of the class
    olcEngine3D demo;
    // instance of the console
    // if (demo.ConstructConsole(512, 480, 4, 4))
    // if (demo.ConstructConsole(256, 240, 4, 4))
    // if (demo.ConstructConsole(128, 120, 4, 4))
    if (demo.ConstructConsole(64, 60, 4, 4))
        //if we cn successful construct the console start it 
        demo.Start();

    return 0;
}

