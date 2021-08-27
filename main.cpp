#include "main.h"

unsigned int ID;
#define GL_CLAMP_TO_EDGE 0x812F

const int window_width = 900;
const int window_height = 550;

GLfloat eyeX = 0;
GLfloat eyeY = 40;
GLfloat eyeZ = 100;

GLfloat lookX = 0;
GLfloat lookY = 40;
GLfloat lookZ = -1000;

float wall_w = 40, wall_h = 1, wall_l = 120;
int doorl_open = 1, doorr_open = 1, dooru_open=1;
float doorlx = -wall_w/4, doorrx = wall_w/4, dooru_y=wall_w/4+10;

int up=0,down=0,r=0,l=0;
float ballx=0, bally=7, ballz=70;

bool ambient = true, diffuse = true, specular = true;
int light_1 = 1, light_2= 0, light_3 = 0, light_4 = 0;
///
float spot_x1=0, spot_y1=145, spot_z1=70;
float spot_x2=52, spot_y2=40, spot_z2=-126;
///

float obsx=wall_l-wall_w, obsy=0.0, obsz=-(wall_l+wall_w);

bool anglex_rot = true, ball_rotate=true;
float rotate_b=0;

int ball_pos[10] = {0};
//float v =-52.0,b =33.0,c =-126.0;
float rot = 0;

bool ball_y_inc = true;
bool ball_y_dec = false;
bool jump_for = false;
bool jump_back = false;
bool jump_left = false;
bool jump_right = false;

int cnt, process=0;
clock_t start,en_d,now;
string t;

static GLfloat v_cube[8][3] =
{
    {0,0,0},
    {0,0,1},
    {0,1,0},
    {0,1,1},

    {1,0,0},
    {1,0,1},
    {1,1,0},
    {1,1,1}
};

static GLubyte c_ind[6][4] =
{
    {3,1,5,7}, //front
    {2,0,1,3}, //left
    {7,5,4,6}, //r
    {2,3,7,6}, //top
    {1,0,4,5}, //bottom
    {6,4,0,2} //back
};

static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1,
                        GLfloat x2, GLfloat y2, GLfloat z2,
                        GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}

static void resize(int width, int height)
{
    const float ar = (float) window_width / (float) window_height;

    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

void LoadTexture(const char*filename, int rep = 1)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(rep)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}

void material_property(float colR=0.5, float colG=0.5, float colB=0.5, float emission=false, float shine=60)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { colR*0.5, colG*0.5, colB*0.5, 1.0 };
    GLfloat mat_diffuse[] = { colR, colG, colB, 1.0 };
    GLfloat mat_emission[] = {colR*0.5, colG*0.5, colB*0.5, 1.0};
    GLfloat mat_specular[] = { 1, 1, 1, 1.0 };
    GLfloat mat_shininess[] = {shine};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv( GL_FRONT, GL_EMISSION, no_mat);

    if(emission)
        glMaterialfv( GL_FRONT, GL_EMISSION, mat_emission);

}

void cube(float colR=0.5, float colG=0.5, float colB=0.5, bool e = false, float alpha = 1)
{

    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { colR, colG, colB, 1.0 };
    GLfloat mat_diffuse[] = { colR, colG, colB, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_emission[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = {30};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    glMaterialfv( GL_FRONT, GL_EMISSION, no_mat);

    if(e == true) glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {
        getNormal3p(v_cube[c_ind[i][0]][0], v_cube[c_ind[i][0]][1], v_cube[c_ind[i][0]][2],
                    v_cube[c_ind[i][1]][0], v_cube[c_ind[i][1]][1], v_cube[c_ind[i][1]][2],
                    v_cube[c_ind[i][2]][0], v_cube[c_ind[i][2]][1], v_cube[c_ind[i][2]][2]);

        glTexCoord2f(5,5);
        glVertex3fv(&v_cube[c_ind[i][0]][0]);
        glTexCoord2f(0,5);
        glVertex3fv(&v_cube[c_ind[i][1]][0]);
        glTexCoord2f(0,0);
        glVertex3fv(&v_cube[c_ind[i][2]][0]);
        glTexCoord2f(5,0);
        glVertex3fv(&v_cube[c_ind[i][3]][0]);

//        for (GLint j=0; j<4; j++)
//        {
//            glVertex3fv(&v_cube[c_ind[i][j]][0]);
//        }
    }
    glEnd();
}

void light(float x=0, float y=50, float z=0)
{
//    ambient = true, diffuse = true, specular = true;

    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[]  = {0.1, 0.1, 0.1, 1.0};
    GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_pos[] = { x,y,z,0};
    glEnable(GL_LIGHT0);

    if(light_1 && ambient) glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient);
    else glLightfv( GL_LIGHT0, GL_AMBIENT, no_light);
    if(light_1 && diffuse) glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    else glLightfv( GL_LIGHT0, GL_DIFFUSE, no_light);
    if(light_1 && specular) glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular);
    else glLightfv( GL_LIGHT0, GL_SPECULAR, no_light);

    glLightfv( GL_LIGHT0, GL_POSITION, light_pos);

}

void spot_light(float x, float y, float z)
{
//    ambient = true, diffuse = true, specular = true;

    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[]  = {0.5, 0, 0, 1.0};
    GLfloat light_diffuse[]  = { 1, 0, 0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_pos[] = { x, y, z,1};
    glEnable(GL_LIGHT2);

    if(light_3 && ambient) glLightfv( GL_LIGHT2, GL_AMBIENT, light_ambient);
    else glLightfv( GL_LIGHT2, GL_AMBIENT, no_light);
    if(light_3 && diffuse) glLightfv( GL_LIGHT2, GL_DIFFUSE, light_diffuse);
    else glLightfv( GL_LIGHT2, GL_DIFFUSE, no_light);
    if(light_3 && specular) glLightfv( GL_LIGHT2, GL_SPECULAR, light_specular);
    else glLightfv( GL_LIGHT2, GL_SPECULAR, no_light);

    glLightfv( GL_LIGHT2, GL_POSITION, light_pos);

    GLfloat direction[] = {0,-1,0,1};
    GLfloat cut_off = 20.0;
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direction );
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, cut_off );

}

void spot_light1(float x, float y, float z)
{
//    ambient = true, diffuse = true, specular = true;

    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[]  = {0.5, 0.5, 0, 1.0};
    GLfloat light_diffuse[]  = { 1, 0, 0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_pos[] = { x, y, z, 1};
    glEnable(GL_LIGHT3);

    if(light_4 && ambient) glLightfv( GL_LIGHT3, GL_AMBIENT, light_ambient);
    else glLightfv( GL_LIGHT3, GL_AMBIENT, no_light);
    if(light_4 && diffuse) glLightfv( GL_LIGHT3, GL_DIFFUSE, light_diffuse);
    else glLightfv( GL_LIGHT3, GL_DIFFUSE, no_light);
    if(light_4 && specular) glLightfv( GL_LIGHT3, GL_SPECULAR, light_specular);
    else glLightfv( GL_LIGHT3, GL_SPECULAR, no_light);

    glLightfv( GL_LIGHT3, GL_POSITION, light_pos);

    GLfloat direction[] = {0,-1,0,1};
    GLfloat cut_off = 50.0;
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, direction );
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, cut_off );
}

void spot_light2(float x, float y, float z)
{
//    ambient = true, diffuse = true, specular = true;

    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[]  = {0.5, 0, 0, 1.0};
    GLfloat light_diffuse[]  = { 1, 1, 0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_pos[] = { x, y, z, 1};
    glEnable(GL_LIGHT1);

    if(light_2 && ambient) glLightfv( GL_LIGHT1, GL_AMBIENT, light_ambient);
    else glLightfv( GL_LIGHT1, GL_AMBIENT, no_light);
    if(light_2 && diffuse) glLightfv( GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    else glLightfv( GL_LIGHT1, GL_DIFFUSE, no_light);
    if(light_2 && specular) glLightfv( GL_LIGHT1, GL_SPECULAR, light_specular);
    else glLightfv( GL_LIGHT1, GL_SPECULAR, no_light);

    glLightfv( GL_LIGHT1, GL_POSITION, light_pos);

    GLfloat direction[] = {0,-1,0,1};
    GLfloat cut_off = 30.0;
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction );
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, cut_off );
}

void door_touch()
{
    if((((doorlx-10 <= ballx-7 && ballx-7 <= doorlx+10) ||(doorlx-10 <= ballx+7 && ballx+7 <= doorlx+10)) ||
            ((doorrx-10 <= ballx-7 && ballx-7 <= doorrx+10) || (doorrx-10 <= ballx+7 && ballx+7 <= doorrx+10))) && ballz > -6 && ballz < 6)
    {
        ballx=0;
        bally=7;
        ballz=70;
        eyeX = 0;
        eyeY = 40;
        eyeZ = 100;
        lookX = 0;
        lookY = 40;
        lookZ = -1000;
    }
    if(dooru_y<33 && ballz>-65 && ballz<-50)
    {
        ballx=0;
        bally=7;
        ballz=70;
        eyeX = 0;
        eyeY = 40;
        eyeZ = 100;
        lookX = 0;
        lookY = 40;
        lookZ = -1000;
    }
    if(bally<9 && ballz>-167 && ballz<-165)
    {
        ballx=0;
        bally=7;
        ballz=70;
        eyeX = 0;
        eyeY = 40;
        eyeZ = 100;
        lookX = 0;
        lookY = 40;
        lookZ = -1000;
    }
//    if(ballz <= -153 && ballz >= -157 && bally < 9)
//    {
//        ballx=0; bally=7; ballz=70;
//        eyeX = 0; eyeY = 40; eyeZ = 100;
//        lookX = 0; lookY = 40; lookZ = -1000;
//    }
}

void position_detect(float width, float len, int bridge_no, float cen_x=0, float cen_z=0)
{
    bool left = (ballx)>=(-width/2+cen_x);
    bool right = (ballx)<=(width/2+cen_x);
    bool fr = (ballz)>=(-len/2+cen_z);
    bool ba = (ballz)<=(len/2+cen_z);
    if(left && right && fr && ba)
    {
        //cout<<"ok"<<endl;
        //ok
        ball_pos[bridge_no] = 1;
    }

    else
    {
        //cout<<"no"<<endl;
        //game-over
        ball_pos[bridge_no] = 0;
    }
}

void update(int value)
{
    if(doorl_open)
    {
        doorlx -= 0.2;
        if(doorlx<-30)
        {
            doorl_open=0;
        }
    }
    if (!doorl_open)
    {
        doorlx += 0.2;
        if(doorlx>-10)
        {
            doorl_open=1;
        }
    }
    if(doorr_open)
    {
        doorrx += 0.2;
        if(doorrx>30)
        {
            doorr_open=0;
        }
    }
    if (!doorr_open)
    {
        doorrx -= 0.2;
        if(doorrx<10)
        {
            doorr_open=1;
        }
    }

    if(dooru_open)
    {
        dooru_y += 0.2;
//        cout << "dooru_y: "<<dooru_y<<endl;
        if(dooru_y>60)
        {
            dooru_open=0;
        }
    }
    if (!dooru_open)
    {
        dooru_y -= 0.2;
//        cout << "dooru_y: "<<dooru_y<<endl;
        if(dooru_y<20)
        {
            dooru_open=1;
        }
    }

    if(anglex_rot)
    {
        anglex = ( anglex + 3 ) % 360;
    }

    glutPostRedisplay(); //Tell GLUT that the display has changed

    //Tell GLUT to call update again in 25 milliseconds
    glutTimerFunc(25, update, 0);
}

void axes()
{
    float length = 10;
    float width = 0.3;

    // X-axis
    glPushMatrix();
    glTranslatef(length/2,0,0);
    glScalef(length,width,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,0,0); //RED Line
    glPopMatrix();

    // Y-axis
    glPushMatrix();
    glTranslatef(0,length/2,0);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0,1,0); //GREEN Line
    glPopMatrix();

    // Z-axis
    glPushMatrix();
    glTranslatef(0,0,length/2);
    glScalef(width,width,length);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0,0,1); //BLUE Line
    glPopMatrix();
}

void curve_object(float curveX=-40.0, float curveY=25.0, float curveZ=110.0, float colR=1.0, float colG=1.0, float colB=0.0)
{

    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { colR, colG, colB, 1.0 };
    GLfloat mat_diffuse[] = { colR, colG, colB, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_emission[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = {30};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    glMaterialfv( GL_FRONT, GL_EMISSION, no_mat);

    glPushMatrix();
    material_property(1,1,0);
    glRotatef( 90, 0.0, 0.0, 1.0);
//    glRotatef( anglex, 0.0, 0.0, 1.0);
//    glRotatef( anglex, 0.0, 0.0, 1.0);
//    glRotatef( angley, 0.0, 1.0, 0.0);
//    glRotatef( anglez, 1.0, 0.0, 0.0);
    glTranslatef(curveX, curveY, curveZ);
    glScalef(30, 50, 50);
    glTranslatef(-0.5, -0.5, -0.5);
    bottleBezier(0,20,40,20);
    glPopMatrix();
    /*glPushMatrix();
    material_property(1,0,0);
    glTranslatef(-10,0,0);
    glRotatef(90,0,1,0);
    bottleBezier(29,34,40,20);
    glPopMatrix();*/
}

void curve_gate()
{
    for(int i=1; i<=12; i+=2)
    {
        glPushMatrix();
        material_property(1,0,0);
        glTranslatef(0,0,9+i);
        glRotatef(90,0,1,0);
        bottleBezier(21,28,40,20);
        glPopMatrix();
    }
    for(int i=1; i<=12; i+=2)
    {
        glPushMatrix();
        material_property(1,0,0);
        glTranslatef(0,0,-276+i);
        glRotatef(90,0,1,0);
        bottleBezier(21,28,40,20);
        glPopMatrix();
    }
}

void sphere(double r,double g,double b)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { 0.5*r, 0.5*g, 0.5*b, 1.0 };
    GLfloat mat_diffuse[] = { r, g, b, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = {60};
    GLfloat mat_emission[] = {0.3*r, 0.2*g, 0.2*b, 1.0};

    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glEnable(GL_TEXTURE_GEN_T);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 7);

    glPushMatrix();
    glTranslatef(ballx,bally,ballz);
    glutSolidSphere (7, 50, 50);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glDisable(GL_TEXTURE_GEN_T);
}

void ball()
{
    sphere(0.5,1,0.7);
}

void ball_jump()
{
    if(jump_for)
    {
        if(ball_y_inc)
        {
            bally +=0.5;
            ballz -=0.125;
            eyeZ -=0.125;
            lookZ -=0.125;
//            cout << "bally: "<<bally<<endl;
            if(bally>=30)
            {
                ball_y_inc = false;
                ball_y_dec = true;
            }
        }
        else if(ball_y_dec)
        {
            bally -=0.5;
            ballz -=0.125;
            eyeZ -=0.125;
            lookZ -=0.125;
//            cout << "bally: "<<bally<<endl;
            if(bally==0)
            {
                ball_y_inc = true;
                ball_y_dec = false;
                jump_for = false;
            }
        }

    }
    if(jump_back)
    {
        if(ball_y_inc)
        {
            bally +=0.5;
            ballz +=0.125;
            eyeZ +=0.125;
            lookZ +=0.125;
            if(bally>=30)
            {
                ball_y_inc = false;
                ball_y_dec = true;
            }
        }
        else if(ball_y_dec)
        {
            bally -=0.5;
            ballz +=0.125;
            eyeZ +=0.125;
            lookZ +=0.125;
            if(bally==0)
            {
                ball_y_inc = true;
                ball_y_dec = false;
                jump_back = false;
            }
        }
    }
    if(jump_left)
    {
        if(ball_y_inc)
        {
            bally +=0.5;
            ballx -=0.125;
            eyeX -=0.125;
            lookX -=0.125;
            if(bally>=30)
            {
                ball_y_inc = false;
                ball_y_dec = true;
            }
        }
        else if(ball_y_dec)
        {
            bally -=0.5;
            ballx -=0.125;
            eyeX -=0.125;
            lookX -=0.125;
            if(bally==0)
            {
                ball_y_inc = true;
                ball_y_dec = false;
                jump_left = false;
            }
        }
    }
    if(jump_right)
    {
        if(ball_y_inc)
        {
            bally +=0.5;
            ballx +=0.125;
            eyeX +=0.125;
            lookX +=0.125;
            if(bally>=30)
            {
                ball_y_inc = false;
                ball_y_dec = true;
            }
        }
        else if(ball_y_dec)
        {
            bally -=0.5;
            ballx +=0.125;
            eyeX +=0.125;
            lookX +=0.125;
            if(bally==0)
            {
                ball_y_inc = true;
                ball_y_dec = false;
                jump_right = false;
            }
        }
    }
    glutPostRedisplay();
}

void lamp()
{
    float wall_w = 40, wall_h = 1, wall_l = 120;
    float lamp_w = 4, lamp_h = 50, lamp_l = 1;

    glPushMatrix();
    glTranslatef(0, lamp_h/2, -(wall_l/2+wall_w-lamp_l/2));
    glScalef(lamp_w, lamp_h, lamp_l);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, lamp_h/2, -(wall_l/2+wall_w-lamp_l/2));
    glScalef(lamp_w, lamp_h, lamp_l);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1);
    glPopMatrix();
}

void obstacle()
{
    material_property(1,1,1);
    glTranslatef(obsx, obsy, obsz);
    glScalef(1,2.5,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 11);
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.5,1);
    glVertex3f(-16,8,0);
    glTexCoord2f(0,0);
    glVertex3f(-20,0,0);
    glTexCoord2f(1,0);
    glVertex3f(-12,0,0);

    glTexCoord2f(0.5,1);
    glVertex3f(-8,8,0);
    glTexCoord2f(0,0);
    glVertex3f(-12,0,0);
    glTexCoord2f(1,0);
    glVertex3f(-4,0,0);

    glTexCoord2f(0.5,1);
    glVertex3f(0,8,0);
    glTexCoord2f(0,0);
    glVertex3f(-4,0,0);
    glTexCoord2f(1,0);
    glVertex3f(4,0,0);

    glTexCoord2f(0.5,1);
    glVertex3f(8,8,0);
    glTexCoord2f(0,0);
    glVertex3f(4,0,0);
    glTexCoord2f(1,0);
    glVertex3f(12,0,0);

    glTexCoord2f(0.5,1);
    glVertex3f(16,8,0);
    glTexCoord2f(0,0);
    glVertex3f(12,0,0);
    glTexCoord2f(1,0);
    glVertex3f(20,0,0);

    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void bridge()
{
    axes();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 4);
    // square
    glPushMatrix();
    position_detect(wall_w, wall_w, 7, 0, -(wall_l*2+wall_w*2+wall_l/2+wall_w/2));
    glTranslatef(0, 0, -(wall_l*2+wall_w*2+wall_l/2+wall_w/2));
    glScalef(wall_w, wall_h, wall_w);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,1);

    // straight
    glPushMatrix();
    position_detect(wall_w, wall_l, 6, 0, -(wall_l*2+wall_w*2));
    glTranslatef(0, 0, -(wall_l*2+wall_w*2));
    glScalef(wall_w, wall_h, wall_l);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1);
    glPopMatrix();

    // left
    glPushMatrix();
    position_detect(wall_l, wall_w, 5, wall_l/2-wall_w/2, -(wall_l+wall_w+wall_l/2+wall_w/2));
    glTranslatef(wall_l/2-wall_w/2, 0, -(wall_l+wall_w+wall_l/2+wall_w/2));
    glScalef(wall_l, wall_h, wall_w);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 0);
    glPopMatrix();

    // straight
    glPushMatrix();
    position_detect(wall_w, wall_l, 4, wall_l-wall_w, -(wall_l+wall_w));
    glTranslatef(wall_l-wall_w, 0, -(wall_l+wall_w));
    glScalef(wall_w, wall_h, wall_l);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1);
    glPopMatrix();

    // right
    glPushMatrix();
    position_detect(wall_l, wall_w, 3, wall_l/2-wall_w/2, -(wall_l/2+wall_w/2));
    glTranslatef(wall_l/2-wall_w/2, 0, -(wall_l/2+wall_w/2));
    glScalef(wall_l, wall_h, wall_w);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 0);
    glPopMatrix();

    // straight
    glPushMatrix();
    position_detect(wall_w, wall_l, 2);
    glScalef(wall_w, wall_h, wall_l);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,4);
    // square
    glPushMatrix();
    position_detect(wall_w, wall_w, 1, 0, wall_l/2+wall_w/2);
    glTranslatef(0, 0, wall_l/2+wall_w/2);
    glScalef(wall_w, wall_h, wall_w);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1.000, 1.000, 0.941);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void door()
{
    //right
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,11);
    glPushMatrix();
    glPushMatrix();
    glTranslatef(doorrx, wall_w/4+10, 0);
    glScalef(wall_w/2, wall_w/2+20, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.753, 0.753, 0.753);
    glPopMatrix();

    //left
    glPushMatrix();
    glTranslatef(doorlx, wall_w/4+10, 0);
    glScalef(wall_w/2, wall_w/2+20, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.753, 0.753, 0.753);
    glPopMatrix();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //updown
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,1);
    glPushMatrix();
    glTranslatef(0, dooru_y, -(wall_w/4+50));
    glScalef(wall_w, wall_w/2+20, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.545, 0.000, 0.000);
    glPopMatrix();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void scene()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 5);
    //front
    glPushMatrix();
    glTranslatef(0, 90, -450);
    glScalef(400, 200, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.690, 0.769, 0.871);
    glPopMatrix();

    //back
    glPushMatrix();
    glTranslatef(0, 90, 150);
    glScalef(400, 200, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.690, 0.769, 0.871);
    glPopMatrix();

    //right
    glPushMatrix();
    glTranslatef(200, 90, -350);
    glScalef(1, 200, 1000);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.690, 0.769, 0.871);
    glPopMatrix();

    //left
    glPushMatrix();
    glTranslatef(-200, 90, -350);
    glScalef(1, 200, 1000);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.690, 0.769, 0.871);
    glPopMatrix();

    //up
    glPushMatrix();
    glTranslatef(0, 190, -350);
    glScalef(400, 1, 1000);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.690, 0.769, 0.871);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //down

//    glTranslatef(0, 190, -350);
//    glScalef(400, 1, 1000);
    // middle
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 2);

    glPushMatrix();
    glTranslatef(40, -10, -150);
    glScalef(160, 1, 600);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1.000, 0.271, 0.000);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // left
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 10);

    glPushMatrix();
    glTranslatef(-120, -10, -150);
    glScalef(160, 1, 600);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1.000, 0.627, 0.478);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // right
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 10);

    glPushMatrix();
    glTranslatef(160, -10, -150);
    glScalef(80, 1, 600);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1.000, 0.627, 0.478);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, 2);
//
//    glPushMatrix();
//    glTranslatef(0, -10, 50);
//    glScalef(400, 1, 200);
//    glTranslatef(-0.5, -0.5, -0.5);
//    cube(1, 0, 1);
//    glPopMatrix();
//
//    glDisable(GL_TEXTURE_2D);

//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, 2);
//
//    glPushMatrix();
//    glTranslatef(0, -10, -150);
//    glScalef(400, 1, 200);
//    glTranslatef(-0.5, -0.5, -0.5);
//    cube(1, 0, 1);
//    glPopMatrix();
//
//    glDisable(GL_TEXTURE_2D);

//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, 2);
//
//    glPushMatrix();
//    glTranslatef(0, -10, -350);
//    glScalef(400, 1, 200);
//    glTranslatef(-0.5, -0.5, -0.5);
//    cube(1, 0, 1);
//    glPopMatrix();
//
//    glDisable(GL_TEXTURE_2D);

}

void idle()
{
    if(process==2)
    {
        en_d=clock();
        cnt=(en_d-start)/CLOCKS_PER_SEC;

        if(cnt==120)
        {
            process=3;
        }
        else if((cnt<100) && (ballz<=-380))
        {
            process=1;
        }
    }

    if (ball_rotate == true)
    {
        rotate_b+=0.0001;
    }
}

void game_over()
{
    int status = 0;

    for(int i=0; i<10; i++)
    {
        status +=ball_pos[i];
    }
    if(status<=0)
    {
//        cout<<"game_over"<<endl;
//        glutDestroyWindow(58);
        process = 3;
    }
    else
    {
//        cout<<"game_ok"<<endl;
    }
}

void Text(char *str,int x,int y,int z)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(0.04,0.04,0.04);
    material_property(1,1,1, true);
//    glRasterPos3f(x,y,z);
    for (int i = 0; i < strlen(str); i++)
    {
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,str[i]);
    }
    glPopMatrix();
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    //glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glLoadIdentity();
    int lim = 8;
    glFrustum(-lim, lim, -lim, lim, 2, 1000);

    glMatrixMode(GL_MODELVIEW);
    //glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glLoadIdentity();

    gluLookAt(eyeX,eyeY,eyeZ, lookX,lookY,lookZ, 0,1,0);
    glRotatef(rot, 0,1,0);
    //axes();

    glPushMatrix();
    light();
    glPopMatrix();

    glPushMatrix();
    spot_light(spot_x1, spot_y1, spot_z1);
    glPopMatrix();

    glPushMatrix();
    spot_light1(spot_x2, spot_y2, spot_z2);
    glPopMatrix();

    glPushMatrix();
    spot_light2(0, 145, -329);
    glPopMatrix();

    glPushMatrix();

    glPushMatrix();
    obstacle();
    glPopMatrix();

    glPushMatrix();
    curve_gate();
    glPopMatrix();

    glPushMatrix();
    curve_object(40, 23.5, -329);
    glPopMatrix();

    glPushMatrix();
    curve_object(40, -52, -126);
    glPopMatrix();

    glPushMatrix();
    curve_object(40.0, 25.0, 110.0);
    glPopMatrix();

    glPushMatrix();
    scene();
    glPopMatrix();

    glPushMatrix();
    lamp();
    glPopMatrix();

    glPushMatrix();
    door();
    glPopMatrix();

    glPushMatrix();
    bridge();
    glPopMatrix();

    glPushMatrix();
//    glScalef(1, 1, 0.5);
    ball();
    glPopMatrix();

    glPushMatrix();
    ball_jump();
    glPopMatrix();

    glPushMatrix();
    door_touch();
    glPopMatrix();

    glPushMatrix();
    game_over();
    glPopMatrix();

    glPushMatrix();
    t = "Time: ";
    stringstream ss;
    ss<<cnt;
    string scnt;
    ss>>scnt;
    t+=scnt;
    glPushMatrix();
    glTranslatef(ballx+65, 7+122, ballz+0.5);
    glScalef(0.04,0.04,0.04);
    material_property(1,1,1, true);

    for (int i = 0; i < t.size(); i++)
    {
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,t[i]);
    }
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void output(char *str,int x,int y,int z)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int lim = 8;
    glFrustum(-lim, lim, -lim, lim, 2, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0,40,100, 0,40,-1000, 0,1,0);

    //material_property(1,1,0, true);
    glPushMatrix();
    //glColor3f(1,0,0);
    glRasterPos3f(x,y,z);
    for (int i = 0; i < strlen(str); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,str[i]);
    }
    glPopMatrix();

    glPushMatrix();
    material_property(1,1,1,true);
    GLUquadricObj *start_ball = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricTexture(start_ball, true);
    glBindTexture(GL_TEXTURE_2D,12);
    glRotatef(rotate_b,0,1,0);
    glTranslatef(0,120,0);
    glScalef(5,5,5);
    gluSphere(start_ball,5,50,50);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void Start(void)
{
//    light();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    material_property(1,1,0, true);
    output("3D Ball Balance", -60,250,0);
    glPopMatrix();
    glPushMatrix();
    material_property(1,1,0, true);
    output("Abu Noman Md Sakib", -100,50,0);
    glPopMatrix();
    glPushMatrix();
    material_property(1,1,0, true);
    output("Roll: 1607058", -55,10,0);
    glPopMatrix();
    glPushMatrix();
    material_property(1,1,0, true);
    output("Press ENTER to start the game", -130,-250,0);
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void Play()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    material_property(1,1,0, true);
    output("Congratulations! You have completed the game.", -160, 50, 0);
    glPopMatrix();

//    glPushMatrix();
//    t = "Time: ";
//    stringstream ss;
//    ss<<cnt;
//    string scnt;
//    ss>>scnt;
//    t+=scnt;
//    glPushMatrix();
//    glTranslatef(-160, 20, 0);
//    glScalef(0.04,0.04,0.04);
//    material_property(1,1,1, true);
//
//    for (int i = 0; i < t.size(); i++)
//    {
//        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,t[i]);
//    }
//    glPopMatrix();

    glPushMatrix();
    material_property(1,1,0, true);
    output("Press 'q' to exit & 'n' to start again.", -160, 0, 0);
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void End()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    material_property(1,1,0,true);
    output("Game Over!", -60, 50, 0);
    glPopMatrix();
    glPushMatrix();
    material_property(1,1,0,true);
    output("Press 'q' to exit & 'n' to start again.", -160, 0, 0);
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
}

void func()
{
    if(process==0) Start();
    else if(process==1) Play();
    else if (process==2) display();
    else if (process==3) End();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {

//    case 'v':
//        v+=0.5;
//        cout << "x: " << v<<endl;
//        break;
//    case 'V':
//        v-=0.5;
//        cout << "x: " << v<<endl;
//        break;
//    case 'b':
//        b+=0.5;
//        cout << "y: " << b<<endl;
//        break;
//    case 'B':
//        b-=0.5;
//        cout << "y: " << b<<endl;
//        break;
//    case 'c':
//        spot_y1+=0.5;
//        cout << "z: " << spot_y1<<endl;
//        break;
//    case 'C':
//        spot_y1-=0.5;
//        cout << "z: " << spot_y1<<endl;
//        break;

    case 13:
        process=2;
        start=clock();
        break;
    case 'n':
        ballx=0;
        bally=7;
        ballz=70;
        eyeX = 0;
        eyeY = 40;
        eyeZ = 100;
        lookX = 0;
        lookY = 40;
        lookZ = -1000;
        process=2;
        start=clock();
        break;
    case 'q':
        exit(0);
        break;
    case 'u':
        eyeY++;
        lookY++;
        break;
    case 'e':
        eyeY--;
        lookY--;
        break;
    case 'r':
        rot--;
        break;
    case 't':
        rot++;
        break;
    case 'l':
        eyeX++;
        lookX++;
        break;
    case 'j':
        eyeX--;
        lookX--;
        break;
    case 'i':
        eyeZ--;
        break;
    case 'k':
        eyeZ++;
        break;

    case 'w':
//        cout<<"ballz: "<<ballz<<endl;
        if(ballz >= -420)
        {
            ballz -=1;
            eyeZ--;
            lookZ--;
        }
        break;
    case 's':
        ballz +=1;
        eyeZ++;
        lookZ++;
        break;
    case 'a':
        ballx -=1;
        eyeX--;
        lookX--;
//        cout<<"ballx: "<<ballx<<endl;
        break;
    case 'd':
        ballx +=1;
        eyeX++;
        lookX++;
//        cout<<"ballx: "<<ballx<<endl;
        break;
    case '4':
        jump_left = true;
        break;
    case '6':
        jump_right = true;
        break;
    case '8':
        jump_for = true;
        break;
    case '5':
        jump_back = true;
        break;
    case 'x':
        anglex = ( anglex + 3 ) % 360;
        break;
    case 'X':
        anglex = ( anglex - 3 ) % 360;
        break;

    case 'y':
        angley = ( angley + 3 ) % 360;
        break;
    case 'Y':
        angley = ( angley - 3 ) % 360;
        break;

    case 'z':
        anglez = ( anglez + 3 ) % 360;
        break;
    case 'Z':
        anglez = ( anglez - 3 ) % 360;
        break;
    case 'f':
        if(anglex_rot) anglex_rot = false;
        else anglex_rot = true;
        break;
    case '1':
    {
        light_1 = 1-light_1;
        ambient = true, diffuse = true, specular = true;
    }
    break;
    case '2':
    {
        light_2 = 1-light_2;
        ambient = true, diffuse = true, specular = true;
    }
    break;
    case '3':
    {
        light_3 = 1-light_3;
        ambient = true, diffuse = true, specular = true;
    }
    break;
    case '7':
    {
        light_4 = 1-light_4;
        ambient = true, diffuse = true, specular = true;
    }
    break;

    case 'c':
        ambient = 1 - ambient;
        break;
    case 'v':
        diffuse = 1 - diffuse;
        break;
    case 'b':
        specular = 1 - specular;
        break;
    }

    glutPostRedisplay();
}

void instructions()
{
    cout << "##	INSTRUCTIONS	##\n\nPress\n\n - 'q' to quit\n - 'u' to up\n - 'e' to down\n - 'r' to rotate right\n - 't' to rotate left\n - 'l' to move right\n - 'j' to move left\n - 'i' to zoom in\n - 'k' to zoom out\n";
    cout << "\n - 'w' for ball front\n - 's' for ball back\n - 'a' for ball left\n - 'd' for ball right\n";
    cout << "\n - '8' for jump front\n - '5' for jump back\n - '4' for jump left\n - '6' for jump right\n";
    cout << "\n - '1' for 1st light\n - 'c' for ambient(1st light)\n - 'v' for diffuse(1st light)\n - 'b' for specular(1st light)\n";
//    cout << "\n - '2' for 2nd light\n - 'c' for ambient(2nd light)\n - 'v' for diffuse(2nd light)\n - 'b' for specular(2nd light)\n";
    cout << "\n - '3' for Spot light\n - 'c' for ambient(Spot light)\n - 'v' for diffuse(Spot light)\n - 'b' for specular(Spot light)\n";

}

int main(int argc, char *argv[])
{
    instructions();
    glutInit(&argc, argv);
    glutInitWindowSize(window_width,window_height);
    glutInitWindowPosition(300,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("3D Ball Balancing");
    LoadTexture("F:\\Study\\Graphics\\3D Ball Balancing\\img\\wood.bmp",1);
    LoadTexture("F:\\Study\\Graphics\\3D Ball Balancing\\img\\lava_n.bmp",2);
    LoadTexture("F:\\Study\\Graphics\\3D Ball Balancing\\img\\finish.bmp",3);
    LoadTexture("F:\\Study\\Graphics\\3D Ball Balancing\\img\\start.bmp",4);
    LoadTexture("F:\\Study\\Graphics\\3D Ball Balancing\\img\\star.bmp",5);
    LoadTexture("F:\\Study\\Graphics\\3D Ball Balancing\\img\\volcano.bmp",6);
    LoadTexture("F:\\Study\\Graphics\\3D Ball Balancing\\img\\ball.bmp",7);
    LoadTexture("F:\\Study\\Graphics\\3D Ball Balancing\\img\\water.bmp",8);
    LoadTexture("F:\\Study\\Graphics\\3D Ball Balancing\\img\\grass.bmp",9);
    LoadTexture("F:\\Study\\Graphics\\3D Ball Balancing\\img\\sand.bmp",10);
    LoadTexture("F:\\Study\\Graphics\\3D Ball Balancing\\img\\steel.bmp",11);
    LoadTexture("F:\\Study\\Graphics\\3D Ball Balancing\\img\\rainbow.bmp",12);

    light();
    glutDisplayFunc(func);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
//    glutMouseFunc(processMouse);

    glEnable(GL_DEPTH_TEST);
    glShadeModel( GL_SMOOTH );
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);

    glutTimerFunc(25, update, 0);
    glutMainLoop();
    return EXIT_SUCCESS;
}
