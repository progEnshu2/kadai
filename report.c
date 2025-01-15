#include "my-turtle.h"
#include <stdio.h>
#include <unistd.h>

// スクリーンのサイズ
#define EDGE (600)

int racket1_x, racket2_x;        // ラケット1とラケット2の座標
int ball_x, ball_y;              // ボールの（仮想画面上での）X座標とY座標
int ball_vx, ball_vy;            // ボールの各軸方向の進行向き（-1 または 1）
int score;                       // スコア
int game_over_flg;               // ゲームオーバーか？
char score_str[100];             // 画面表示用の作業領域

// ラケットのサイズ
#define RACKET_SIZE (6)

// 仮想画面の横幅（WIDTH）と高さ（HEIGHT），
// および仮想画面を表す二次元配列 screen．
#define SCR_WIDTH (40)
#define SCR_HEIGHT (60)
unsigned char screen[SCR_WIDTH][SCR_HEIGHT];

// ラケット1および2の位置，ボールの位置，ボールの進行方向，
// スコア（0点），ゲームオーバーではない，を変数に設定．
void init() {
  score = 0;
  game_over_flg = 0;
}



// 大きい方を返す関数


void draw_screen() {
  double scr_x, scr_y, edge_size;
  int x, y;


  // 画面をクリア（↓関数 clearscreen を使って黒く塗りつぶす）
  clearscreen(0,0,0);

  
  // 画面上部に点数を表示する．sprintf はほとんど printf と同じように
  // 使えるが，「sprintf(score_str, "SCORE: %10d", score);」とすると，
  // 出力文字列が（画面ではなく）文字列のための配列変数 score_str に
  // 格納される．文字列 score_str は，my_turtle.o の
  // put_text 関数で出力する．
  pencolor(1.0, 1.0, 1.0);
  /* 文字列「score_str」の生成処理 */
  put_text(score_str, 'h', 18);
  sprintf(score_str, "SCORE: %10d", score);
}

// 事実上のメイン処理
void disp_body(void) {
  // 画面描画を行う
  draw_screen();

  if(game_over_flg == 0) {
    // ゲームオーバーでなかったら．．．
    score += 1;               // スコアを10点増やす．
    usleep(0.02*1000*1000);    // 時間調整（待ち時間を減らすと，より高速になる）
  } else {
    // ゲームオーバーの処理．
    pencolor(1.0, 1.0, 1.0);
    put_text("GAME OVER", 'h', 18);
    put_text("Press [r] to play again", 'h', 18);
    if(inkey('r') == 1) init();
  }
}

// main関数
int main(int argc, char *argv[]) {
  init();
  runGL(EDGE, 2.0, "Squash GAME", 1, &argc, argv);
}
