// ============================================================
//  Catch the Eggs  –  v2.0.0
//  CSE 426 – Computer Graphics Lab  |  Spring 2025
//  UAP Term Project
//
//  Built with OpenGL (GLUT / freeglut)
//
//  Compile (Linux / macOS):
//    g++ catch_the_eggs.cpp -o catch_the_eggs -lGL -lGLU -lglut -lm
//  Compile (Windows with freeglut):
//    g++ catch_the_eggs.cpp -o catch_the_eggs -lfreeglut -lopengl32 -lglu32
//
//  ─── Controls ──────────────────────────────────────────────
//  Mouse Move / Click     – move basket / interact menus
//  LEFT / RIGHT arrows    – move basket
//  A / D                  – move basket (alternate)
//  P                      – pause / resume
//  ESC                    – back / exit
// ============================================================

#ifdef _WIN32
#  include <windows.h>
#endif

#include <GL/glut.h>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

// ─────────────────────────── Window ──────────────────────────────
static const int   WIN_W = 900;
static const int   WIN_H = 650;
static const float PI    = 3.14159265f;

// ─────────────────────────── Game Timing ─────────────────────────
static const float GAME_DURATION = 120.f;   // 2 minutes

// ─────────────────────────── Bamboo stick ────────────────────────
static const float STICK_Y  = WIN_H - 95.f;
static const float STICK_H  = 14.f;

// ─────────────────────────── Chicken ─────────────────────────────
static const int   HEN_COUNT     = 2;
static const float HEN_W         = 56.f;
static const float HEN_H         = 48.f;
static const float HEN_SPEED     = 80.f;
static const float HEN_SPAWN_MIN = 1.0f;
static const float HEN_SPAWN_MAX = 2.8f;

// ─────────────────────────── Basket ──────────────────────────────
static const float BASKET_W_BASE = 110.f;
static const float BASKET_H      =  28.f;
static const float BASKET_Y      =  50.f;
static const float BASKET_SPEED  = 360.f;

// ─────────────────────────── Falling objects ─────────────────────
static const int   MAX_OBJECTS   = 20;
static const float EGG_RX        = 13.f;
static const float EGG_RY        = 17.f;
static const float BASE_EGG_SPD  = 130.f;

// ─────────────────────────── Perk blocks ─────────────────────────
static const float BLOCK_W = 28.f;
static const float BLOCK_H = 28.f;

// ─────────────────────────── Airflow (bonus) ─────────────────────
static const float AIRFLOW_FORCE = 60.f;

// ─────────────────────────── Colours ─────────────────────────────
struct Col3 { float r, g, b; };
static const Col3 C_SKY_TOP    = {0.53f, 0.81f, 0.95f};
static const Col3 C_SKY_BOT    = {0.85f, 0.95f, 1.00f};
static const Col3 C_GROUND     = {0.30f, 0.65f, 0.22f};
static const Col3 C_GROUND2    = {0.22f, 0.50f, 0.14f};
static const Col3 C_BASKET     = {0.70f, 0.42f, 0.10f};
static const Col3 C_BASKET_RIM = {0.90f, 0.62f, 0.18f};
static const Col3 C_WHITE      = {1.00f, 1.00f, 1.00f};
static const Col3 C_GOLDEN     = {1.00f, 0.82f, 0.00f};
static const Col3 C_BLUE       = {0.30f, 0.55f, 1.00f};
static const Col3 C_POOP       = {0.40f, 0.26f, 0.08f};
static const Col3 C_PERK_BIG   = {0.20f, 0.85f, 0.30f};
static const Col3 C_PERK_SLOW  = {0.25f, 0.70f, 1.00f};
static const Col3 C_PERK_TIME  = {1.00f, 0.80f, 0.10f};
static const Col3 C_HUD_BG     = {0.08f, 0.10f, 0.18f};

// ─────────────────────────── Enums ───────────────────────────────
enum ObjType  { OBJ_NORMAL=0, OBJ_GOLDEN, OBJ_BLUE, OBJ_POOP,
                OBJ_PERK_BIG, OBJ_PERK_SLOW, OBJ_PERK_TIME };
enum GameState{ STATE_MENU=0, STATE_PLAY, STATE_PAUSED, STATE_GAMEOVER, STATE_HELP };

// ─────────────────────────── Structs ─────────────────────────────
struct FallingObj {
    float   x, y, vy, vx, rot, wobble;
    ObjType type;
    bool    active;
};

struct Hen {
    float x, y, dir;
    float spawnTimer, spawnInterval;
    float wingPhase, bobPhase;
};

struct Particle {
    float x, y, vx, vy, life, maxLife, size;
    Col3  color;
};

struct Cloud { float x, y, w, speed; };

struct PerkState {
    float bigBasketTimer;
    float slowEggTimer;
    float airflowTimer;
    float airflowDir;
};

struct PopupLabel {
    float x, y, life, maxLife;
    std::string text;
    Col3 color;
};

// ─────────────────────────── Globals ─────────────────────────────
static GameState  gState      = STATE_MENU;
static int        gScore      = 0;
static int        gHighScore  = 0;
static float      gTimeLeft   = GAME_DURATION;
static float      gTime       = 0.f;
static float      gLastTime   = 0.f;
static bool       gKeyLeft    = false;
static bool       gKeyRight   = false;
static float      gMouseX     = WIN_W * 0.5f;
static int        gMouseMX    = 0;
static int        gMouseMY    = 0;
static float      gBasketX    = WIN_W * 0.5f;
static float      gBasketW    = BASKET_W_BASE;
static float      gFlashTimer = 0.f;
static Col3       gFlashColor = {1.f, 0.f, 0.f};

static FallingObj gObjs[MAX_OBJECTS];
static Hen        gHens[HEN_COUNT];
static PerkState  gPerk       = {0.f, 0.f, 0.f, 1.f};

static std::vector<Particle>  gParticles;
static std::vector<Cloud>     gClouds;
static std::vector<PopupLabel>gPopups;

// ─────────────────────────── Utility ─────────────────────────────
static void setColor(const Col3& c, float a=1.f){ glColor4f(c.r,c.g,c.b,a); }

static float randf(float lo, float hi){
    return lo + (hi-lo)*(rand()/float(RAND_MAX));
}

static void drawOval(float cx,float cy,float rx,float ry,int n=30){
    glBegin(GL_TRIANGLE_FAN); glVertex2f(cx,cy);
    for(int i=0;i<=n;i++){float a=2*PI*i/n; glVertex2f(cx+rx*cosf(a),cy+ry*sinf(a));}
    glEnd();
}
static void drawOvalLine(float cx,float cy,float rx,float ry,int n=30){
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<n;i++){float a=2*PI*i/n; glVertex2f(cx+rx*cosf(a),cy+ry*sinf(a));}
    glEnd();
}
static void drawCircle(float cx,float cy,float r,int n=22){drawOval(cx,cy,r,r,n);}

static void drawRect(float x,float y,float w,float h){
    glBegin(GL_QUADS);
    glVertex2f(x,y);glVertex2f(x+w,y);glVertex2f(x+w,y+h);glVertex2f(x,y+h);
    glEnd();
}

static void drawRoundRect(float x,float y,float w,float h,float r){
    glBegin(GL_QUADS);
    glVertex2f(x+r,y);glVertex2f(x+w-r,y);glVertex2f(x+w-r,y+h);glVertex2f(x+r,y+h);
    glVertex2f(x,y+r);glVertex2f(x+r,y+r);glVertex2f(x+r,y+h-r);glVertex2f(x,y+h-r);
    glVertex2f(x+w-r,y+r);glVertex2f(x+w,y+r);glVertex2f(x+w,y+h-r);glVertex2f(x+w-r,y+h-r);
    glEnd();
    float ang[4]={90,0,270,180};
    float cx[4]={x+r,x+w-r,x+w-r,x+r};
    float cy[4]={y+h-r,y+h-r,y+r,y+r};
    for(int c=0;c<4;c++){
        glBegin(GL_TRIANGLE_FAN); glVertex2f(cx[c],cy[c]);
        for(int i=0;i<=10;i++){float a=(ang[c]+90.f*i/10.f)*PI/180.f;
            glVertex2f(cx[c]+r*cosf(a),cy[c]+r*sinf(a));}
        glEnd();
    }
}

static void drawText(float x,float y,const std::string& s,void* font=GLUT_BITMAP_HELVETICA_18){
    glRasterPos2f(x,y);
    for(char c:s) glutBitmapCharacter(font,c);
}
static void drawTextL(float x,float y,const std::string& s){
    glRasterPos2f(x,y);
    for(char c:s) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,c);
}
static void drawTextSm(float x,float y,const std::string& s){
    glRasterPos2f(x,y);
    for(char c:s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,c);
}

static std::string itos(int v){std::ostringstream ss;ss<<v;return ss.str();}
static std::string fmtTime(float t){
    int m=int(t)/60, s=int(t)%60;
    char buf[16]; sprintf(buf,"%d:%02d",m,s); return buf;
}

// ─────────────────────────── Clouds ──────────────────────────────
static void initClouds(){
    gClouds.clear();
    for(int i=0;i<5;i++){
        Cloud c;
        c.x=randf(0,WIN_W); c.y=randf(WIN_H*0.55f,WIN_H*0.88f);
        c.w=randf(80,160);  c.speed=randf(12,28);
        gClouds.push_back(c);
    }
}
static void drawCloud(float cx,float cy,float w){
    float h=w*0.38f;
    glColor3f(1,1,1);
    drawOval(cx,cy,w*0.5f,h*0.55f);
    drawOval(cx-w*0.22f,cy+h*0.10f,w*0.32f,h*0.48f);
    drawOval(cx+w*0.22f,cy+h*0.10f,w*0.30f,h*0.45f);
    drawOval(cx-w*0.10f,cy+h*0.30f,w*0.22f,h*0.35f);
    drawOval(cx+w*0.08f,cy+h*0.28f,w*0.20f,h*0.32f);
}
static void updateClouds(float dt){
    for(auto& c:gClouds){
        c.x+=c.speed*dt;
        if(c.x-c.w>WIN_W){c.x=-c.w; c.y=randf(WIN_H*0.55f,WIN_H*0.88f);}
    }
}

// ─────────────────────────── Particles ───────────────────────────
static void spawnParticles(float x,float y,const Col3& col,int n=12){
    for(int i=0;i<n;i++){
        Particle p;
        p.x=x; p.y=y;
        p.vx=randf(-130,130); p.vy=randf(30,160);
        p.life=p.maxLife=randf(0.35f,0.75f);
        p.color=col; p.size=randf(3,7);
        gParticles.push_back(p);
    }
}
static void updateParticles(float dt){
    for(auto& p:gParticles){p.x+=p.vx*dt;p.y+=p.vy*dt;p.vy-=220*dt;p.life-=dt;}
    gParticles.erase(std::remove_if(gParticles.begin(),gParticles.end(),
        [](const Particle& p){return p.life<=0;}),gParticles.end());
}
static void drawParticles(){
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for(const auto& p:gParticles){
        float a=p.life/p.maxLife;
        glColor4f(p.color.r,p.color.g,p.color.b,a);
        drawCircle(p.x,p.y,p.size*a);
    }
    glDisable(GL_BLEND);
}

// ─────────────────────────── Popup labels ────────────────────────
static void spawnPopup(float x,float y,const std::string& t,const Col3& c){
    PopupLabel p; p.x=x;p.y=y;p.life=p.maxLife=1.2f;p.text=t;p.color=c;
    gPopups.push_back(p);
}
static void updatePopups(float dt){
    for(auto& p:gPopups){p.life-=dt;p.y+=30*dt;}
    gPopups.erase(std::remove_if(gPopups.begin(),gPopups.end(),
        [](const PopupLabel& p){return p.life<=0;}),gPopups.end());
}
static void drawPopups(){
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for(const auto& p:gPopups){
        float a=p.life/p.maxLife;
        glColor4f(p.color.r,p.color.g,p.color.b,a);
        drawText(p.x-12,p.y,p.text);
    }
    glDisable(GL_BLEND);
}

// ─────────────────────────── Background ──────────────────────────
static void drawBackground(){
    glBegin(GL_QUADS);
    setColor(C_SKY_TOP); glVertex2f(0,WIN_H); glVertex2f(WIN_W,WIN_H);
    setColor(C_SKY_BOT); glVertex2f(WIN_W,WIN_H*0.32f); glVertex2f(0,WIN_H*0.32f);
    glEnd();
    setColor(C_GROUND);  drawRect(0,0,WIN_W,WIN_H*0.32f);
    setColor(C_GROUND2); drawRect(0,WIN_H*0.30f,WIN_W,WIN_H*0.03f);
    // Grass tufts
    glColor3f(0.28f,0.72f,0.18f);
    for(int i=0;i<30;i++){
        float gx=i*(WIN_W/30.f)+8; float gy=WIN_H*0.32f;
        glBegin(GL_TRIANGLES);
        glVertex2f(gx-6,gy);glVertex2f(gx+6,gy);glVertex2f(gx,gy+10);
        glVertex2f(gx-3,gy);glVertex2f(gx+9,gy);glVertex2f(gx+3,gy+8);
        glEnd();
    }
}

// ─────────────────────────── Bamboo stick ────────────────────────
static void drawBambooStick(){
    float y=STICK_Y;
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.15f); drawRect(0,y-STICK_H*0.5f-5,WIN_W,6);
    glDisable(GL_BLEND);

    glBegin(GL_QUADS);
    glColor3f(0.55f,0.40f,0.10f);
    glVertex2f(0,y-STICK_H*0.5f); glVertex2f(WIN_W,y-STICK_H*0.5f);
    glColor3f(0.80f,0.65f,0.25f);
    glVertex2f(WIN_W,y+STICK_H*0.5f); glVertex2f(0,y+STICK_H*0.5f);
    glEnd();

    glColor3f(0.50f,0.36f,0.08f); glLineWidth(2.5f);
    for(int i=1;i<12;i++){
        float nx=i*(WIN_W/12.f);
        glBegin(GL_LINES); glVertex2f(nx,y-STICK_H*0.5f); glVertex2f(nx,y+STICK_H*0.5f); glEnd();
    }
    glLineWidth(1.f);

    // Support poles
    glColor3f(0.45f,0.30f,0.05f);
    drawRect(0,WIN_H*0.32f,14,y-WIN_H*0.32f+STICK_H*0.5f);
    drawRect(WIN_W-14,WIN_H*0.32f,14,y-WIN_H*0.32f+STICK_H*0.5f);
}

// ─────────────────────────── Egg ─────────────────────────────────
static void drawEgg(float x,float y,ObjType type,float rot){
    glPushMatrix();
    glTranslatef(x,y,0); glRotatef(rot,0,0,1);

    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.14f); drawOval(3,-EGG_RY+2,EGG_RX*0.85f,4.f);
    glDisable(GL_BLEND);

    Col3 shell, outline;
    if     (type==OBJ_GOLDEN){shell=C_GOLDEN; outline={0.75f,0.50f,0.f};}
    else if(type==OBJ_BLUE  ){shell=C_BLUE;   outline={0.10f,0.30f,0.80f};}
    else                      {shell=C_WHITE;  outline={0.70f,0.70f,0.70f};}

    setColor(shell);
    drawOval(0,0,EGG_RX,EGG_RY); drawOval(0,-3,EGG_RX*0.85f,EGG_RY*0.62f);

    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1,1,1,0.50f); drawOval(-EGG_RX*0.28f,EGG_RY*0.28f,EGG_RX*0.20f,EGG_RY*0.16f);
    glDisable(GL_BLEND);

    glLineWidth(1.4f); setColor(outline); drawOvalLine(0,0,EGG_RX,EGG_RY); glLineWidth(1.f);

    if(type==OBJ_GOLDEN){
        glColor3f(1.f,0.50f,0.f);
        glBegin(GL_TRIANGLE_FAN); glVertex2f(0,0);
        for(int i=0;i<=10;i++){float a=PI/2+i*2*PI/10; float r=(i%2==0)?6.f:2.5f;
            glVertex2f(r*cosf(a),r*sinf(a));}
        glEnd();
    }
    glPopMatrix();
}

// ─────────────────────────── Poop ────────────────────────────────
static void drawPoop(float x,float y,float rot){
    glPushMatrix(); glTranslatef(x,y,0); glRotatef(rot,0,0,1);
    setColor(C_POOP);
    drawCircle(0,0,10.f); drawCircle(2,10,7.5f); drawCircle(0,18,5.f);
    glColor3f(0.58f,0.40f,0.16f); drawCircle(0,22,3.5f);
    // Buzz flies
    glColor3f(0.1f,0.1f,0.1f);
    drawCircle(-14+sinf(gTime*5)*4, 6, 2.2f);
    drawCircle( 14+sinf(gTime*5+PI)*3, 9, 2.2f);
    glPopMatrix();
}

// ─────────────────────────── Perk block ──────────────────────────
static void drawPerkBlock(float x,float y,ObjType type){
    Col3 col; const char* label="";
    if     (type==OBJ_PERK_BIG ){col=C_PERK_BIG; label="BIG";}
    else if(type==OBJ_PERK_SLOW){col=C_PERK_SLOW;label="SLW";}
    else                         {col=C_PERK_TIME;label="+T";}

    float pulse=0.7f+0.3f*sinf(gTime*4);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(col.r,col.g,col.b,0.28f*pulse);
    drawRoundRect(x-BLOCK_W*0.5f-6,y-BLOCK_H*0.5f-6,BLOCK_W+12,BLOCK_H+12,8);
    glDisable(GL_BLEND);

    setColor(col); drawRoundRect(x-BLOCK_W*0.5f,y-BLOCK_H*0.5f,BLOCK_W,BLOCK_H,6);
    glColor3f(1,1,1); glLineWidth(2.f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x-BLOCK_W*0.5f,y-BLOCK_H*0.5f); glVertex2f(x+BLOCK_W*0.5f,y-BLOCK_H*0.5f);
    glVertex2f(x+BLOCK_W*0.5f,y+BLOCK_H*0.5f); glVertex2f(x-BLOCK_W*0.5f,y+BLOCK_H*0.5f);
    glEnd(); glLineWidth(1.f);
    glColor3f(1,1,1); drawTextSm(x-10,y-4,label);
}

// ─────────────────────────── Hen ─────────────────────────────────
static void drawHen(const Hen& h){
    float x=h.x, y=h.y+sinf(h.bobPhase)*3;
    float wf=0.5f+0.5f*sinf(h.wingPhase);

    glColor3f(0.78f,0.35f,0.12f); drawOval(x,y,HEN_W*0.44f,HEN_H*0.40f);

    glColor3f(0.62f,0.26f,0.08f);
    glPushMatrix(); glTranslatef(x-HEN_W*0.22f,y-6*wf,0); glRotatef(-25-30*wf,0,0,1);
    drawOval(0,0,HEN_W*0.26f,HEN_H*0.20f); glPopMatrix();

    // Tail
    glColor3f(0.58f,0.22f,0.06f);
    float tx=x-HEN_W*0.42f*h.dir;
    for(int f=-1;f<=1;f++){
        glBegin(GL_TRIANGLES);
        glVertex2f(tx,y); glVertex2f(tx-18*h.dir,y+8+f*5); glVertex2f(tx-10*h.dir,y-5+f*4);
        glEnd();
    }

    float hx=x+HEN_W*0.38f*h.dir, hy=y+HEN_H*0.44f;
    glColor3f(0.78f,0.35f,0.12f); drawCircle(hx,hy,HEN_H*0.26f);

    // Comb
    glColor3f(0.88f,0.10f,0.10f);
    for(int i=0;i<3;i++) drawOval(hx+(i-1)*5.f,hy+HEN_H*0.26f+3+i*2,4.f,6+i*2.f);
    drawOval(hx+4*h.dir,hy-6,4.f,6.f);

    // Beak
    glColor3f(1.f,0.65f,0.10f);
    float bx=hx+HEN_H*0.24f*h.dir;
    glBegin(GL_TRIANGLES);
    glVertex2f(bx,hy+2); glVertex2f(bx+10*h.dir,hy+5); glVertex2f(bx+10*h.dir,hy-1);
    glEnd();

    glColor3f(0.05f,0.05f,0.05f); drawCircle(hx+HEN_H*0.10f*h.dir,hy+4,3.f);
    glColor3f(1,1,1);             drawCircle(hx+HEN_H*0.10f*h.dir+1,hy+5,1.2f);

    // Feet
    glColor3f(1.f,0.65f,0.10f); glLineWidth(2.5f);
    for(int f=-1;f<=1;f+=2){
        float fx=x+f*HEN_W*0.15f, fy=h.y-HEN_H*0.40f;
        glBegin(GL_LINES);
        glVertex2f(fx,fy); glVertex2f(fx,fy-12);
        glVertex2f(fx,fy-12); glVertex2f(fx-10*h.dir,fy-12);
        glVertex2f(fx,fy-12); glVertex2f(fx,fy-20);
        glEnd();
    }
    glLineWidth(1.f);
}

// ─────────────────────────── Basket ──────────────────────────────
static void drawBasket(){
    float bx=gBasketX, by=BASKET_Y, hw=gBasketW*0.5f;
    float topW=hw, botW=hw*0.75f;

    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.18f); drawOval(bx,by-BASKET_H*0.5f-2,hw*0.88f,5.f);
    glDisable(GL_BLEND);

    setColor(C_BASKET);
    glBegin(GL_QUADS);
    glVertex2f(bx-topW,by+BASKET_H*0.5f); glVertex2f(bx+topW,by+BASKET_H*0.5f);
    glVertex2f(bx+botW,by-BASKET_H*0.5f); glVertex2f(bx-botW,by-BASKET_H*0.5f);
    glEnd();

    glColor3f(0.45f,0.28f,0.05f); glLineWidth(1.f);
    for(int r=1;r<4;r++){
        float t=r/4.f, lw=topW*(1-t)+botW*t, ly=by+BASKET_H*0.5f-BASKET_H*t;
        glBegin(GL_LINES); glVertex2f(bx-lw,ly); glVertex2f(bx+lw,ly); glEnd();
    }
    for(int c=-4;c<=4;c++){
        float tx=bx+c*(hw*0.22f);
        glBegin(GL_LINES); glVertex2f(tx,by+BASKET_H*0.5f);
        glVertex2f(tx*0.96f+bx*0.04f,by-BASKET_H*0.5f); glEnd();
    }

    setColor(C_BASKET_RIM); drawRect(bx-topW-4,by+BASKET_H*0.5f,(topW+4)*2,9.f);

    glColor3f(0.55f,0.34f,0.06f); glLineWidth(3.f);
    for(int s=-1;s<=1;s+=2){
        float hbx=bx+s*(topW+4), hby=by+BASKET_H*0.5f+9;
        glBegin(GL_LINE_STRIP);
        glVertex2f(hbx,hby); glVertex2f(hbx+s*8,hby+14); glVertex2f(hbx+s*4,hby+22);
        glEnd();
    }
    glLineWidth(1.f);

    // Big-basket glow
    if(gPerk.bigBasketTimer>0){
        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        float pulse=0.22f+0.12f*sinf(gTime*6);
        glColor4f(0.2f,1.f,0.4f,pulse);
        drawRect(bx-topW-8,by-BASKET_H*0.5f-4,(topW+8)*2,BASKET_H+22);
        glDisable(GL_BLEND);
    }
}

// ─────────────────────────── Airflow arrows ──────────────────────
static void drawAirflow(){
    if(gPerk.airflowTimer<=0) return;
    float alpha=0.20f+0.12f*sinf(gTime*5);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.6f,0.9f,1.f,alpha);
    float d=gPerk.airflowDir;
    for(int row=0;row<4;row++){
        float ay=WIN_H*0.35f+row*(WIN_H*0.55f/4)+30;
        for(int col=0;col<7;col++){
            float ax=col*(WIN_W/7.f)+40;
            glBegin(GL_TRIANGLES);
            glVertex2f(ax,ay); glVertex2f(ax+22*d,ay+6); glVertex2f(ax+22*d,ay-6);
            glEnd();
        }
    }
    glDisable(GL_BLEND);
}

// ─────────────────────────── HUD ─────────────────────────────────
static void drawHUD(){
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(C_HUD_BG.r,C_HUD_BG.g,C_HUD_BG.b,0.85f);
    drawRect(0,WIN_H-50,WIN_W,50);
    glDisable(GL_BLEND);

    // Dividers
    glColor3f(0.35f,0.35f,0.55f); glLineWidth(1.f);
    for(float dx:{WIN_W*0.33f,WIN_W*0.66f}){
        glBegin(GL_LINES); glVertex2f(dx,WIN_H-50); glVertex2f(dx,WIN_H); glEnd();
    }

    // Score
    glColor3f(1.f,0.92f,0.40f);
    drawText(14,WIN_H-30,"SCORE: " + itos(gScore));
    glColor3f(0.65f,0.65f,0.85f);
    drawTextSm(14,WIN_H-10,"Best: " + itos(gHighScore));

    // Timer (color changes near end)
    float ratio=gTimeLeft/GAME_DURATION;
    Col3 tc=(ratio<0.25f)?Col3{1.f,0.25f,0.25f}:Col3{0.30f,1.f,0.55f};
    setColor(tc);
    drawTextL(WIN_W*0.5f-46,WIN_H-28,"TIME: " + fmtTime(gTimeLeft));

    // Perk indicators
    float px=WIN_W*0.66f+14;
    glColor3f(0.75f,0.75f,0.90f); drawTextSm(px,WIN_H-10,"PERKS:");
    if(gPerk.bigBasketTimer>0){setColor(C_PERK_BIG); drawTextSm(px,WIN_H-28,"BIG "+itos((int)gPerk.bigBasketTimer)+"s"); px+=84;}
    if(gPerk.slowEggTimer>0) {setColor(C_PERK_SLOW);drawTextSm(px,WIN_H-28,"SLOW "+itos((int)gPerk.slowEggTimer)+"s"); px+=84;}
    if(gPerk.airflowTimer>0){glColor3f(0.6f,0.9f,1.f);drawTextSm(px,WIN_H-28,"WIND "+itos((int)gPerk.airflowTimer)+"s");}

    // Timer bar at very top of HUD
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    setColor(tc,0.7f);
    drawRect(0,WIN_H-50,WIN_W*ratio,4.f);
    glDisable(GL_BLEND);
}

// ─────────────────────────── Flash ───────────────────────────────
static void drawFlash(){
    if(gFlashTimer<=0) return;
    float a=std::min(gFlashTimer*2.f,0.48f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(gFlashColor.r,gFlashColor.g,gFlashColor.b,a);
    drawRect(0,0,WIN_W,WIN_H);
    glDisable(GL_BLEND);
}

// ─────────────────────────── Button helper ───────────────────────
static bool inButton(int mx,int my,float cx,float cy,float w,float h){
    return mx>cx-w*0.5f && mx<cx+w*0.5f &&
           (WIN_H-my)>cy-h*0.5f && (WIN_H-my)<cy+h*0.5f;
}

static void drawButton(float cx,float cy,float w,float h,
                       const std::string& label,bool hover){
    float x=cx-w*0.5f, y=cy-h*0.5f;
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(hover?0.28f:0.12f, hover?0.52f:0.20f, hover?0.76f:0.38f, 0.94f);
    drawRoundRect(x,y,w,h,8);
    glDisable(GL_BLEND);
    glColor3f(1,1,1); glLineWidth(hover?2.5f:1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,y);glVertex2f(x+w,y);glVertex2f(x+w,y+h);glVertex2f(x,y+h);
    glEnd(); glLineWidth(1.f);
    int tw=glutBitmapLength(GLUT_BITMAP_HELVETICA_18,(const unsigned char*)label.c_str());
    glColor3f(1,1,1); drawText(cx-tw*0.5f,cy-6,label);
}

// ─────────────────────────── Menu pages ──────────────────────────
static void drawMenuPage(){
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.04f,0.06f,0.16f,0.90f);
    drawRoundRect(WIN_W*0.5f-250,WIN_H*0.5f-210,500,430,20);
    glDisable(GL_BLEND);
    glColor3f(C_GOLDEN.r,C_GOLDEN.g,C_GOLDEN.b); glLineWidth(3.f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(WIN_W*0.5f-250,WIN_H*0.5f-210);glVertex2f(WIN_W*0.5f+250,WIN_H*0.5f-210);
    glVertex2f(WIN_W*0.5f+250,WIN_H*0.5f+220);glVertex2f(WIN_W*0.5f-250,WIN_H*0.5f+220);
    glEnd(); glLineWidth(1.f);

    glColor3f(1.f,0.88f,0.10f);
    drawTextL(WIN_W*0.5f-130,WIN_H*0.5f+178,"CATCH  THE  EGGS");
    glColor3f(0.60f,0.60f,0.82f);
    drawTextSm(WIN_W*0.5f-68,WIN_H*0.5f+152,"v2.0.0   |   CSE 426 – Spring 2025");

    if(gHighScore>0){
        glColor3f(1.f,0.80f,0.20f);
        drawText(WIN_W*0.5f-92,WIN_H*0.5f+122,"High Score:  " + itos(gHighScore));
    }

    float lx=WIN_W*0.5f-205, ly=WIN_H*0.5f+86;
    glColor3f(1.f,0.88f,0.50f); drawText(lx,ly,"EGG TYPES:");
    glColor3f(0.88f,0.88f,1.f);
    drawTextSm(lx,ly-20,"Golden egg  =  +10 pts");
    drawTextSm(lx,ly-38,"Blue egg    =   +5 pts");
    drawTextSm(lx,ly-56,"White egg   =   +1 pt");
    drawTextSm(lx,ly-74,"Poop        =  -10 pts  (avoid!)");

    glColor3f(1.f,0.88f,0.50f); drawText(lx+220,ly,"PERK BLOCKS:");
    glColor3f(0.88f,0.88f,1.f);
    drawTextSm(lx+220,ly-20,"GREEN  = Bigger basket (15 s)");
    drawTextSm(lx+220,ly-38,"BLUE   = Slow eggs (12 s)");
    drawTextSm(lx+220,ly-56,"GOLD   = +15 extra seconds");

    float btnY=WIN_H*0.5f-108;
    drawButton(WIN_W*0.5f,btnY+70, 220,42,"  START GAME",
               inButton(gMouseMX,gMouseMY,WIN_W*0.5f,btnY+70,220,42));
    drawButton(WIN_W*0.5f,btnY+14, 220,42,"  HOW TO PLAY",
               inButton(gMouseMX,gMouseMY,WIN_W*0.5f,btnY+14,220,42));
    drawButton(WIN_W*0.5f,btnY-42, 220,42,"  EXIT",
               inButton(gMouseMX,gMouseMY,WIN_W*0.5f,btnY-42,220,42));
}

static void drawHelpPage(){
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.04f,0.06f,0.18f,0.96f);
    drawRoundRect(WIN_W*0.5f-280,WIN_H*0.5f-240,560,490,18);
    glDisable(GL_BLEND);
    glColor3f(0.40f,0.80f,1.f); glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(WIN_W*0.5f-280,WIN_H*0.5f-240);glVertex2f(WIN_W*0.5f+280,WIN_H*0.5f-240);
    glVertex2f(WIN_W*0.5f+280,WIN_H*0.5f+250);glVertex2f(WIN_W*0.5f-280,WIN_H*0.5f+250);
    glEnd(); glLineWidth(1.f);

    glColor3f(0.40f,0.90f,1.f);
    drawTextL(WIN_W*0.5f-78,WIN_H*0.5f+210,"HOW TO PLAY");

    float lx=WIN_W*0.5f-240, ly=WIN_H*0.5f+165;
    glColor3f(1.f,0.92f,0.50f); drawText(lx,ly,"CONTROLS:");
    glColor3f(0.88f,0.88f,1.f);
    drawText(lx,ly-28, "Mouse Move         –  move basket left / right");
    drawText(lx,ly-52, "LEFT / RIGHT keys  –  keyboard basket movement");
    drawText(lx,ly-76, "A / D              –  alternate keyboard control");
    drawText(lx,ly-100,"P  or  ESC         –  pause the game");

    glColor3f(1.f,0.92f,0.50f); drawText(lx,ly-138,"SCORING:");
    glColor3f(0.88f,0.88f,1.f);
    drawText(lx,ly-162,"Golden egg  +10     Blue egg  +5     White egg  +1");
    drawText(lx,ly-186,"Catching poop  –10 pts  —  dodge it!");

    glColor3f(1.f,0.92f,0.50f); drawText(lx,ly-224,"PERKS (falling coloured blocks):");
    glColor3f(0.88f,0.88f,1.f);
    drawText(lx,ly-248,"Green block  –  Basket grows wider for 15 seconds");
    drawText(lx,ly-272,"Blue  block  –  Eggs fall at half speed for 12 s");
    drawText(lx,ly-296,"Gold  block  –  +15 bonus seconds added to timer");

    glColor3f(1.f,0.92f,0.50f); drawText(lx,ly-334,"BONUS FEATURE:");
    glColor3f(0.88f,0.88f,1.f);
    drawText(lx,ly-358,"Random airflow can push eggs sideways—watch out!");

    drawButton(WIN_W*0.5f,WIN_H*0.5f-218,180,38,"  BACK",
               inButton(gMouseMX,gMouseMY,WIN_W*0.5f,WIN_H*0.5f-218,180,38));
}

static void drawPausePage(){
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.62f); drawRect(0,0,WIN_W,WIN_H);
    glColor4f(0.06f,0.08f,0.20f,0.96f);
    drawRoundRect(WIN_W*0.5f-185,WIN_H*0.5f-130,370,278,16);
    glDisable(GL_BLEND);

    glColor3f(0.80f,0.90f,1.0f); drawTextL(WIN_W*0.5f-58,WIN_H*0.5f+110,"PAUSED");
    glColor3f(0.7f,0.7f,0.9f);
    drawText(WIN_W*0.5f-80,WIN_H*0.5f+72,"Score so far: " + itos(gScore));
    drawText(WIN_W*0.5f-80,WIN_H*0.5f+44,"Time left:   " + fmtTime(gTimeLeft));

    drawButton(WIN_W*0.5f,WIN_H*0.5f- 4,230,40,"  RESUME",
               inButton(gMouseMX,gMouseMY,WIN_W*0.5f,WIN_H*0.5f-4,230,40));
    drawButton(WIN_W*0.5f,WIN_H*0.5f-54,230,40,"  MAIN MENU",
               inButton(gMouseMX,gMouseMY,WIN_W*0.5f,WIN_H*0.5f-54,230,40));
    drawButton(WIN_W*0.5f,WIN_H*0.5f-104,230,40,"  EXIT",
               inButton(gMouseMX,gMouseMY,WIN_W*0.5f,WIN_H*0.5f-104,230,40));
}

static void drawGameOverPage(){
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,0.68f); drawRect(0,0,WIN_W,WIN_H);
    glColor4f(0.06f,0.04f,0.18f,0.97f);
    drawRoundRect(WIN_W*0.5f-220,WIN_H*0.5f-165,440,345,18);
    glDisable(GL_BLEND);
    glColor3f(C_GOLDEN.r,C_GOLDEN.g,C_GOLDEN.b); glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(WIN_W*0.5f-220,WIN_H*0.5f-165);glVertex2f(WIN_W*0.5f+220,WIN_H*0.5f-165);
    glVertex2f(WIN_W*0.5f+220,WIN_H*0.5f+180);glVertex2f(WIN_W*0.5f-220,WIN_H*0.5f+180);
    glEnd(); glLineWidth(1.f);

    glColor3f(1.f,0.22f,0.22f); drawTextL(WIN_W*0.5f-90,WIN_H*0.5f+138,"TIME'S UP!");
    glColor3f(1.f,0.88f,0.30f); drawText(WIN_W*0.5f-78,WIN_H*0.5f+98,"Final Score: "+itos(gScore));
    glColor3f(0.80f,0.80f,1.f); drawText(WIN_W*0.5f-82,WIN_H*0.5f+64,"High Score:  "+itos(gHighScore));

    if(gScore>0 && gScore>=gHighScore){
        float blink=0.6f+0.4f*sinf(gTime*5);
        glColor3f(1.f,0.82f*blink,0.f);
        drawText(WIN_W*0.5f-80,WIN_H*0.5f+30,"★  NEW HIGH SCORE!  ★");
    }

    drawButton(WIN_W*0.5f,WIN_H*0.5f-22, 230,42,"  PLAY AGAIN",
               inButton(gMouseMX,gMouseMY,WIN_W*0.5f,WIN_H*0.5f-22,230,42));
    drawButton(WIN_W*0.5f,WIN_H*0.5f-76, 230,42,"  MAIN MENU",
               inButton(gMouseMX,gMouseMY,WIN_W*0.5f,WIN_H*0.5f-76,230,42));
    drawButton(WIN_W*0.5f,WIN_H*0.5f-130,230,42,"  EXIT",
               inButton(gMouseMX,gMouseMY,WIN_W*0.5f,WIN_H*0.5f-130,230,42));
}

// ─────────────────────────── Spawn helpers ───────────────────────
static float currentEggSpeed(){
    float s=BASE_EGG_SPD;
    if(gPerk.slowEggTimer>0) s*=0.45f;
    return s;
}

static void spawnObject(int idx,float fromX){
    FallingObj& o=gObjs[idx];
    o.active=true; o.x=fromX;
    o.y=STICK_Y-STICK_H*0.5f-5;
    o.vy=currentEggSpeed()*randf(0.88f,1.14f);
    o.vx=0; o.rot=randf(-18,18); o.wobble=randf(0,2*PI);

    float r=randf(0,1);
    if     (r<0.08f) o.type=OBJ_GOLDEN;
    else if(r<0.20f) o.type=OBJ_BLUE;
    else if(r<0.30f) o.type=OBJ_POOP;
    else if(r<0.37f) o.type=OBJ_PERK_BIG;
    else if(r<0.44f) o.type=OBJ_PERK_SLOW;
    else if(r<0.48f) o.type=OBJ_PERK_TIME;
    else              o.type=OBJ_NORMAL;
}

static void resetObjects(){ for(auto& o:gObjs) o.active=false; }

static void initHens(){
    for(int i=0;i<HEN_COUNT;i++){
        Hen& h=gHens[i];
        h.x=(i==0)?WIN_W*0.25f:WIN_W*0.75f;
        h.y=STICK_Y+STICK_H*0.5f+HEN_H*0.42f;
        h.dir=(i==0)?1.f:-1.f;
        h.spawnInterval=randf(HEN_SPAWN_MIN,HEN_SPAWN_MAX);
        h.spawnTimer=randf(0.3f,h.spawnInterval);
        h.wingPhase=randf(0,2*PI); h.bobPhase=randf(0,2*PI);
    }
}

// ─────────────────────────── Reset ───────────────────────────────
static void resetGame(){
    gScore=0; gTimeLeft=GAME_DURATION;
    gBasketX=WIN_W*0.5f; gBasketW=BASKET_W_BASE;
    gPerk={0.f,0.f,0.f,1.f};
    gFlashTimer=0.f;
    gParticles.clear(); gPopups.clear();
    resetObjects(); initHens();
    gState=STATE_PLAY;
}

// ─────────────────────────── Update ──────────────────────────────
static void update(float dt){
    gTime+=dt;
    if(gFlashTimer>0) gFlashTimer-=dt;
    updateClouds(dt);

    if(gState==STATE_PLAY){
        gTimeLeft-=dt;
        if(gTimeLeft<=0){
            gTimeLeft=0;
            if(gScore>gHighScore) gHighScore=gScore;
            gState=STATE_GAMEOVER; return;
        }

        // Basket: mouse (smooth follow) or keyboard
        if(!gKeyLeft && !gKeyRight)
            gBasketX+=(gMouseX-gBasketX)*std::min(1.f,8.f*dt);
        if(gKeyLeft)  gBasketX-=BASKET_SPEED*dt;
        if(gKeyRight) gBasketX+=BASKET_SPEED*dt;
        float hw=gBasketW*0.5f;
        gBasketX=std::max(hw,std::min(float(WIN_W)-hw,gBasketX));

        // Perk timers
        if(gPerk.bigBasketTimer>0){
            gPerk.bigBasketTimer-=dt;
            gBasketW=(gPerk.bigBasketTimer>0)?BASKET_W_BASE*1.75f:BASKET_W_BASE;
        }
        if(gPerk.slowEggTimer>0) gPerk.slowEggTimer-=dt;
        if(gPerk.airflowTimer>0){
            gPerk.airflowTimer-=dt;
            if(gPerk.airflowTimer<=0) gPerk.airflowDir=1.f;
        }

        // Random airflow trigger
        if(gPerk.airflowTimer<=0 && randf(0,1)<0.0015f){
            gPerk.airflowTimer=randf(4,9);
            gPerk.airflowDir=(randf(0,1)<0.5f)?1.f:-1.f;
            spawnPopup(WIN_W*0.5f,WIN_H*0.5f,"AIRFLOW!",{0.6f,0.9f,1.f});
        }

        // Update hens
        for(auto& h:gHens){
            h.x+=h.dir*HEN_SPEED*dt;
            if(h.x>WIN_W-HEN_W*0.5f-18){h.x=WIN_W-HEN_W*0.5f-18; h.dir=-1.f;}
            if(h.x<HEN_W*0.5f+18)      {h.x=HEN_W*0.5f+18;        h.dir= 1.f;}
            h.wingPhase+=7*dt; h.bobPhase+=4*dt;
            h.spawnTimer-=dt;
            if(h.spawnTimer<=0){
                h.spawnTimer=h.spawnInterval*randf(0.82f,1.18f);
                for(int i=0;i<MAX_OBJECTS;i++)
                    if(!gObjs[i].active){spawnObject(i,h.x);break;}
            }
        }

        // Update falling objects
        for(auto& o:gObjs){
            if(!o.active) continue;
            float drift=(gPerk.airflowTimer>0)?AIRFLOW_FORCE*gPerk.airflowDir:0.f;
            o.x+=(o.vx+drift)*dt; o.y-=o.vy*dt;
            // Wall bounce
            if(o.x<EGG_RX)        {o.x=EGG_RX;         o.vx= fabsf(o.vx);}
            if(o.x>WIN_W-EGG_RX)  {o.x=WIN_W-EGG_RX;   o.vx=-fabsf(o.vx);}

            // Catch check
            float bTop=BASKET_Y+BASKET_H*0.5f+12;
            float bBot=BASKET_Y-BASKET_H*0.5f-2;
            float bL=gBasketX-gBasketW*0.5f-6;
            float bR=gBasketX+gBasketW*0.5f+6;

            if(o.y-EGG_RY<bTop && o.y+EGG_RY>bBot && o.x>bL && o.x<bR){
                o.active=false;
                switch(o.type){
                case OBJ_GOLDEN:
                    gScore+=10; spawnParticles(o.x,o.y,C_GOLDEN);
                    spawnPopup(o.x,o.y+35,"+10",C_GOLDEN); break;
                case OBJ_BLUE:
                    gScore+=5; spawnParticles(o.x,o.y,C_BLUE);
                    spawnPopup(o.x,o.y+35,"+5",C_BLUE); break;
                case OBJ_NORMAL:
                    gScore+=1; spawnParticles(o.x,o.y,C_WHITE,6);
                    spawnPopup(o.x,o.y+35,"+1",C_WHITE); break;
                case OBJ_POOP:
                    if(gScore>=10) gScore-=10; else gScore=0;
                    gFlashTimer=0.35f; gFlashColor={0.35f,0.18f,0.05f};
                    spawnParticles(o.x,o.y,C_POOP,8);
                    spawnPopup(o.x,o.y+35,"-10",C_POOP); break;
                case OBJ_PERK_BIG:
                    gPerk.bigBasketTimer=15; gBasketW=BASKET_W_BASE*1.75f;
                    spawnParticles(o.x,o.y,C_PERK_BIG);
                    spawnPopup(o.x,o.y+35,"BIG BASKET!",C_PERK_BIG); break;
                case OBJ_PERK_SLOW:
                    gPerk.slowEggTimer=12;
                    spawnParticles(o.x,o.y,C_PERK_SLOW);
                    spawnPopup(o.x,o.y+35,"SLOW EGGS!",C_PERK_SLOW); break;
                case OBJ_PERK_TIME:
                    gTimeLeft+=15;
                    spawnParticles(o.x,o.y,C_PERK_TIME);
                    spawnPopup(o.x,o.y+35,"+15 SECS!",C_PERK_TIME); break;
                }
            } else if(o.y+EGG_RY<0) o.active=false;
        }

        updateParticles(dt);
        updatePopups(dt);
    }
    glutPostRedisplay();
}

// ─────────────────────────── Display ─────────────────────────────
static void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0,WIN_W,0,WIN_H);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    drawBackground();
    for(const auto& c:gClouds) drawCloud(c.x,c.y,c.w);
    drawBambooStick();
    for(int i=0;i<HEN_COUNT;i++) drawHen(gHens[i]);

    if(gState==STATE_PLAY || gState==STATE_PAUSED || gState==STATE_GAMEOVER){
        for(const auto& o:gObjs){
            if(!o.active) continue;
            if(o.type==OBJ_POOP) drawPoop(o.x,o.y,o.rot);
            else if(o.type==OBJ_PERK_BIG||o.type==OBJ_PERK_SLOW||o.type==OBJ_PERK_TIME)
                drawPerkBlock(o.x,o.y,o.type);
            else drawEgg(o.x,o.y,o.type,o.rot);
        }
        drawAirflow();
        drawBasket();
        drawParticles();
        drawPopups();
        drawHUD();
        drawFlash();
    }

    if     (gState==STATE_MENU)     drawMenuPage();
    else if(gState==STATE_HELP)     drawHelpPage();
    else if(gState==STATE_PAUSED)   drawPausePage();
    else if(gState==STATE_GAMEOVER) drawGameOverPage();

    glutSwapBuffers();
}

// ─────────────────────────── Timer ───────────────────────────────
static void timerCB(int){
    float now=glutGet(GLUT_ELAPSED_TIME)/1000.f;
    float dt=now-gLastTime;
    if(dt>0.05f) dt=0.05f;
    gLastTime=now;
    update(dt);
    glutTimerFunc(16,timerCB,0);
}

// ─────────────────────────── Input ───────────────────────────────
static void keyDown(unsigned char k,int,int){
    switch(k){
    case 'a':case 'A': gKeyLeft=true;  break;
    case 'd':case 'D': gKeyRight=true; break;
    case 'p':case 'P':
        if(gState==STATE_PLAY)   gState=STATE_PAUSED;
        else if(gState==STATE_PAUSED) gState=STATE_PLAY;
        break;
    case 27:  // ESC
        if(gState==STATE_PLAY)     gState=STATE_PAUSED;
        else if(gState==STATE_PAUSED)   gState=STATE_MENU;
        else if(gState==STATE_HELP)     gState=STATE_MENU;
        else if(gState==STATE_GAMEOVER) gState=STATE_MENU;
        else exit(0);
        break;
    }
}
static void keyUp(unsigned char k,int,int){
    switch(k){
    case 'a':case 'A': gKeyLeft=false;  break;
    case 'd':case 'D': gKeyRight=false; break;
    }
}
static void specialDown(int k,int,int){
    if(k==GLUT_KEY_LEFT)  gKeyLeft=true;
    if(k==GLUT_KEY_RIGHT) gKeyRight=true;
}
static void specialUp(int k,int,int){
    if(k==GLUT_KEY_LEFT)  gKeyLeft=false;
    if(k==GLUT_KEY_RIGHT) gKeyRight=false;
}

static void mouseMove(int x,int){ gMouseX=float(x); gMouseMX=x; }
static void mousePassive(int x,int y){ gMouseX=float(x); gMouseMX=x; gMouseMY=y; }

static void mouseClick(int button,int state,int x,int y){
    if(button!=GLUT_LEFT_BUTTON||state!=GLUT_DOWN) return;
    gMouseMX=x; gMouseMY=y;

    if(gState==STATE_MENU){
        float btnY=WIN_H*0.5f-108;
        if(inButton(x,y,WIN_W*0.5f,btnY+70,220,42)) resetGame();
        if(inButton(x,y,WIN_W*0.5f,btnY+14,220,42)) gState=STATE_HELP;
        if(inButton(x,y,WIN_W*0.5f,btnY-42,220,42)) exit(0);
    } else if(gState==STATE_HELP){
        if(inButton(x,y,WIN_W*0.5f,WIN_H*0.5f-218,180,38)) gState=STATE_MENU;
    } else if(gState==STATE_PAUSED){
        if(inButton(x,y,WIN_W*0.5f,WIN_H*0.5f- 4,230,40)) gState=STATE_PLAY;
        if(inButton(x,y,WIN_W*0.5f,WIN_H*0.5f-54,230,40)) gState=STATE_MENU;
        if(inButton(x,y,WIN_W*0.5f,WIN_H*0.5f-104,230,40)) exit(0);
    } else if(gState==STATE_GAMEOVER){
        if(inButton(x,y,WIN_W*0.5f,WIN_H*0.5f-22, 230,42)) resetGame();
        if(inButton(x,y,WIN_W*0.5f,WIN_H*0.5f-76, 230,42)) gState=STATE_MENU;
        if(inButton(x,y,WIN_W*0.5f,WIN_H*0.5f-130,230,42)) exit(0);
    }
}

// ─────────────────────────── Main ────────────────────────────────
int main(int argc,char** argv){
    srand(static_cast<unsigned>(time(nullptr)));

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(WIN_W,WIN_H);
    glutCreateWindow("Catch the Eggs  v2.0.0  |  CSE 426");

    glClearColor(0.53f,0.81f,0.95f,1.f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

    initClouds();
    initHens();
    // Show a couple idle objects on menu
    spawnObject(0,WIN_W*0.35f);
    spawnObject(1,WIN_W*0.65f);

    gLastTime=glutGet(GLUT_ELAPSED_TIME)/1000.f;

    glutDisplayFunc(display);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutMotionFunc(mouseMove);
    glutPassiveMotionFunc(mousePassive);
    glutMouseFunc(mouseClick);
    glutTimerFunc(16,timerCB,0);

    glutMainLoop();
    return 0;
}
