#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "my-turtle.h"

double translate_x(int x);
double translate_y(int y);
int maxof(int x, int y);
#define SCR_WIDTH (100)
#define SCR_HEIGHT (60)
unsigned char screen[SCR_WIDTH][SCR_HEIGHT];

// スクリーンのサイズ
#define EDGE (600)
#define MAX_WALLS (10)  // 最大壁数
int charachter_x = SCR_WIDTH / 2;
int charachter_y = SCR_HEIGHT - 1;
int score;          // スコア
int game_over_flg;  // ゲームオーバーか？
int count = 0;
char score_str[100];  // 画面表示用の作業領域

typedef struct {
  int x, y;    // 壁の位置
  int vx, vy;  // 壁の進行方向
  int active;  // 壁が動いているかどうか
} Wall;

Wall walls[MAX_WALLS];

// 仮想画面の横幅（WIDTH）と高さ（HEIGHT），
// および仮想画面を表す二次元配列 screen．

// ラケット1および2の位置，ボールの位置，ボールの進行方向，
// スコア（0点），ゲームオーバーではない，を変数に設定．
void init() {
  score = 0;
  game_over_flg = 0;
  srand(time(NULL));
  for (int i = 0; i < MAX_WALLS; i++) {
    walls[i].active = 0;
  }
}

void jump() {
  if (count == 0) {
    if (inkey('s') == 1 && charachter_y > 0) {
      charachter_y -= 5;
      count += 5;
    }
  } else if (count > 0) {
    charachter_y += 1;
    count -= 1;
  }
}

void draw_virtual_screen() {
  int x, y;

  // 画面を空白でクリア
  for (y = 0; y < SCR_HEIGHT; y++) {
    for (x = 0; x < SCR_WIDTH; x++) {
      screen[x][y] = ' ';
    }
  }

  // 左右と上部の壁（'#'）を作る
  for (x = 0; x < SCR_WIDTH; x++) {
    screen[x][3] = '#';
  }
  for (y = 3; y < SCR_HEIGHT; y++) {
    screen[0][y] = '#';
    screen[SCR_WIDTH - 1][y] = '#';
  }

  // 壁（'w'）を縦長の長方形として配置
  for (int i = 0; i < MAX_WALLS; i++) {
    if (walls[i].active) {  // 壁がアクティブならば描画
      // 壁を描画
      if (walls[i].y < SCR_HEIGHT - 1) {
        screen[walls[i].x][walls[i].y] = 'w';      // 上部の部分
        screen[walls[i].x][walls[i].y + 1] = 'w';  // 下部の部分
      }
    }
  }

  for (int i = 0; i < 1; i++) {
    screen[charachter_x][charachter_y] = 'o';
  }
}

void move_walls() {
  if (game_over_flg == 1) {
    pencolor(1.0, 1.0, 1.0);
    setpos(translate_x(SCR_WIDTH / 2 - 5), translate_y(SCR_HEIGHT / 2));
    put_text("GAME OVER", 'h', 18);
    setpos(translate_x(SCR_WIDTH / 2 - 8), translate_y(SCR_HEIGHT / 2 + 2));
    put_text("Press [r] to play again", 'h', 18);
    if (inkey('r') == 1) init();
  }
  if (game_over_flg == 0) {
    for (int i = 0; i < MAX_WALLS; i++) {
      if (walls[i].active) {  // 壁がアクティブなら移動
        walls[i].x += walls[i].vx;
        walls[i].y += walls[i].vy;
        if ((walls[i].x == charachter_x) &&
            (walls[i].y == charachter_y || walls[i].y + 1 == charachter_y)) {
          game_over_flg = 1;  // ゲームオーバー
        }
        // 壁が画面左壁に衝突した場合、壁を消去
        if (walls[i].x <= 0) {
          walls[i].active = 0;  // 壁を非アクティブにする（消去）
        }
      }
    }
  }
}

void draw_square(int x, int y, int edge_size, int r, int g, int b) {
  pencolor((double)r, (double)g, (double)b);
  fillcolor((double)r, (double)g, (double)b);
  penup();
  setpos((double)x, (double)y);
  set_dir(0);
  pendown();
  begin_fill();
  forward(edge_size);
  right(90.0);
  forward(edge_size);
  right(90.0);
  forward(edge_size);
  right(90.0);
  forward(edge_size);
  right(90.0);
  end_fill();
}

void draw_screen() {
  double scr_x, scr_y, edge_size;
  int x, y;

  // 画面をクリア（↓関数 clearscreen を使って黒く塗りつぶす）
  draw_virtual_screen();
  clearscreen(0, 0, 0);

  for (y = 0; y < SCR_HEIGHT; y++) {
    for (x = 0; x < SCR_WIDTH; x++) {
      scr_x = translate_x(x);
      scr_y = translate_y(y);
      edge_size = EDGE / maxof(SCR_WIDTH, SCR_HEIGHT);

      switch (screen[x][y]) {
        case ' ':
          draw_square(scr_x, scr_y, edge_size, 0, 0, 0);
          break;
        case '#':
          draw_square(scr_x, scr_y, edge_size, 1, 1, 1);
          break;
        case 'w':
          draw_square(scr_x, scr_y, edge_size, 0, 0, 1);
          break;
        case 'o':
          draw_square(scr_x, scr_y, edge_size, 1, 0, 0);
      }
    }
  }
  // 画面上部に点数を表示する．sprintf はほとんど printf と同じように
  // 使えるが，「sprintf(score_str, "SCORE: %10d", score);」とすると，
  // 出力文字列が（画面ではなく）文字列のための配列変数 score_str に
  // 格納される．文字列 score_str は，my_turtle.o の
  // put_text 関数で出力する．
  pencolor(1.0, 1.0, 1.0);
  setpos(translate_x(0), translate_y(2));
  /* 文字列「score_str」の生成処理 */
  put_text(score_str, 'h', 18);
  sprintf(score_str, "SCORE: %10d", score);
}

int maxof(int x, int y) {
  if (x > y) return x;
  return y;
}

double translate_x(int x) {
  double d_edge, tmp, ret;

  d_edge = maxof(SCR_WIDTH, SCR_HEIGHT);
  tmp = (double)(x - (d_edge / 2));
  if (SCR_WIDTH < SCR_HEIGHT) tmp += (SCR_HEIGHT - SCR_WIDTH) / 2;
  ret = tmp * ((double)EDGE / d_edge);
  return ret;
}

double translate_y(int y) {
  double d_edge, tmp, ret;

  d_edge = maxof(SCR_WIDTH, SCR_HEIGHT);
  tmp = (double)(y - (d_edge / 2));
  if (SCR_HEIGHT < SCR_WIDTH) tmp += (SCR_WIDTH - SCR_HEIGHT) / 2;
  ret = tmp * ((double)EDGE / d_edge);

  return ret;
}
// 事実上のメイン処理
void disp_body(void) {
  // 画面描画を行う
  static int wall_generation_counter = 0;
  draw_screen();

  if (wall_generation_counter >= (rand() % 500 + 1)) {
    wall_generation_counter = 0;

    // 新しい壁を画面右下から生成
    for (int i = 0; i < MAX_WALLS; i++) {
      if (!walls[i].active) {         // ボールが非アクティブならば生成
        walls[i].x = SCR_WIDTH - 2;   // 右端から生成
        walls[i].y = SCR_HEIGHT - 2;  // 下端から生成

        // 進行方向を左に設定
        walls[i].vx = -1;
        walls[i].vy = 0;      // 垂直方向には移動しない
        walls[i].active = 1;  // 壁をアクティブに設定
        break;                // 最初に空いている壁位置に追加
      }
    }
  } else {
    wall_generation_counter++;
  }

  if (game_over_flg == 0) {
    // ゲームオーバーでなかったら．．．
    jump();
    score += 1;  // スコアを10点増やす．
    usleep(0.02 * 1000 *
           1000);  // 時間調整（待ち時間を減らすと，より高速になる）
  } else {
    // ゲームオーバーの処理．
  }

  draw_screen();
  move_walls();
  usleep(0.02 * 1000 * 1000);  // 時間調整
}

// main関数
int main(int argc, char *argv[]) {
  init();
  runGL(EDGE, 2.0, "Squash GAME", 1, &argc, argv);
}
