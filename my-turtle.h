#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#endif

#include <math.h>
#include <stdlib.h>

extern void runGL(int, double, char*, int, int*, char**);
                                          // たとえば「runGL(500, 2.0, "Sample Window", 0, &argc, argv);」で
                                          // ウィンドウサイズ＝500，ペンの太さ＝2の，「Sample Window」という
                                          // ウィンドウを作る．第4引数は，描画するのが静止画のときは0を，
                                          // アニメーションのときは1を指定する．

extern void right(double);                // たとえば「right(30);」で，30度右を向く
extern void rt(double);                   // 関数「right」と同じ
extern void left(double);                 // たとえば「left(30);」で，30度左を向く
extern void lt(double);                   // 関数「left」と同じ
extern double heading(void);              // 「heading()」は，向きを返す（右向き真横が「0度」）
extern void set_dir(double);              // たとえば「set_dir(50);」で，向きを「50度」に設定する

extern void setpos(double, double);       // たとえば「setpos(30, 100);」で，座標 (30, 100) に移動する．
extern void setposition(double, double);  // 関数「setpos」と同じ
extern void setx(double);                 // たとえば「setx(50);」で，X座標＝50 の場所に移動する（Y座標は変えない）
extern void sety(double);                 // たとえば「sety(70);」で，Y座標＝70 の場所に移動する（X座標は変えない）
extern double position_x(void);           // 「position_x()」は，X座標を返す
extern double pos_x(void);                // 関数「position_x」と同じ
extern double x_cor(void);                // 関数「position_x」と同じ
extern double position_y(void);           // 「position_y()」は，Y座標を返す
extern double pos_y(void);                // 関数「position_y」と同じ
extern double y_cor(void);                // 関数「position_y」と同じ

extern void pensize(double);              // たとえば「pensize(2);」で，ペンの太さを「2」にする
extern void width(double);                // 関数「pensize」と同じ

extern void pencolor(double, double, double);     // たとえば「pencolor(1.0, 1.0, 1.0);」で，線の色を白に設定（RGB指定）
extern void fillcolor(double, double, double);    // たとえば「fillcolor(1.0, 1.0, 1.0);」で，塗りつぶし色を白に設定

extern void clearscreen(double, double, double);  // たとえば「clearscreen(1.0, 1.0, 1.0);」で，画面を白で塗りつぶす

extern void penup(void);                  // 「penup();」で，ペンを上げる（移動しても，線は描かない）
extern void pu(void);                     // 関数「penup」と同じ
extern void up(void);                     // 関数「penup」と同じ
extern void pendown(void);                // 「pendown();」で，ペンを下げる（移動すると，軌跡（線）が残る）
extern void pd(void);                     // 関数「pendown」と同じ
extern void down(void);                   // 関数「pendown」と同じ
extern int isdown(void);                  // 「isdown()」は，ペンが下がっているとき1，上がっているとき0

extern void line_seg(double, double, double, double);  // たとえば「line_seg(10, 20, 30, 40);」で，(10,20) から (30,40) に線を引く
extern void forward(double);              // たとえば「forward(100);」で，100だけ前方に移動する
extern void fd(double);                   // 関数「forward」と同じ
extern void backward(double);             // たとえば「backward(100);」で，100だけ後方に移動する
extern void bk(double);                   // 関数「backward」と同じ
extern void back(double);                 // 関数「backward」と同じ

extern void begin_fill(void);             // 「begin_fill();」から「end_fill();」の間にある命令について，
extern void end_fill(void);               //  内部を塗りつぶしながら描画

extern void circle(double);               // たとえば「circle(100);」で，半径100の円を描く
extern void dot(double);                  // たとえば「dot(10);」で，半径10の点を描く

extern void put_text(char*, char, int);   // たとえば「put_text("abc", 'h', 18);」で，サイズ18のHELVETICA
                                          // フォントで，画面にテキスト「abc」を描く（日本語は使用不可．また
                                          // フォントとフォントサイズの組は「'h', 10」「'h', 12」「'h', 18」
                                          // 「'r', 10」「'r', 24」「'b', 8」「'b', 9」のいずれか）

extern int inkey(int);                    // リアルタイムキー入力．たとえば「inkey('a')」は，小文字の「a」が
                                          // 押されているとき1を，押されていないときは0を返す

extern void beep(double, double);         // たとえば「beep(800, 0.2);」で，800Hzのサイン波を0.2秒鳴らす
                                          // （OpenAL の都合上，Mac非対応．引数に何を与えても，つねに0.2秒
                                          // /System/Library/Sounds/Ping.aiff の音を鳴らす）
