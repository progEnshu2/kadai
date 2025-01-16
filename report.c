#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "my-turtle.h"
#define SCR_WIDTH (100)
#define SCR_HEIGHT (60)
unsigned char screen[SCR_WIDTH][SCR_HEIGHT];

// スクリーンのサイズ
#define EDGE (600)
#define MAX_WALLS (10)  // 最大壁数

double translate_x(int x);
double translate_y(int y);
double cloud_x = SCR_WIDTH - 15;   // 右端からスタート
double cloud_y = SCR_HEIGHT - 35;  // 初期Y位置
int maxof(int x, int y);
int charachter_x = SCR_WIDTH / 2;
int charachter_y = SCR_HEIGHT - 2;
int score;          // スコア
int game_over_flg;  // ゲームオーバーか？
int count = 0;
char score_str[100];  // 画面表示用の作業領域
int highscore = 0;
char highscore_str[100];
int speedup = 0;

typedef struct {
  int x, y;           // 壁の位置
  int vx, vy;         // 壁の進行方向
  int active;         // 壁が動いているかどうか
  int width, height;  // 壁の幅
} Wall;

Wall walls[MAX_WALLS];

// 仮想画面の横幅（WIDTH）と高さ（HEIGHT），
// および仮想画面を表す二次元配列 screen．

// ラケット1および2の位置，ボールの位置，ボールの進行方向，
// スコア（0点），ゲームオーバーではない，を変数に設定．
void init() {
  score = 0;
  speedup = 0;
  game_over_flg = 0;

  srand(time(NULL));
  for (int i = 0; i < MAX_WALLS; i++) {
    walls[i].active = 0;
    walls[i].width = 2;
    walls[i].height = 3;
  }
}

void jump() {
  if (count == 0) {
    if (inkey(' ') == 1 && charachter_y > 0) {
      charachter_y -= 5;
      if (charachter_y < 0) charachter_y = 0;  // 範囲を超えないよう制約
      count += 5;
    }
  } else if (count > 0) {
    charachter_y += 1;
    if (charachter_y > SCR_HEIGHT - 2)
      charachter_y = SCR_HEIGHT - 2;  // 下端を超えない
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
    if (walls[i].active) {                          // 壁がアクティブならば描画
      for (int j = 0; j < walls[i].height; j++) {   // 壁の高さ分繰り返し
        for (int k = 0; k < walls[i].width; k++) {  // 壁の幅分繰り返し
          if (walls[i].y + j < SCR_HEIGHT) {
            screen[walls[i].x + k][walls[i].y + j] = 'w';  // 壁を描画
          }
        }
      }
    }
  }

  for (int j = 0; j < 2; j++) {
    for (int k = 0; k < 2; k++) {
      if (charachter_x + k < SCR_WIDTH && charachter_y + j < SCR_HEIGHT) {
        screen[charachter_x + k][charachter_y + j] = 'o';
      }
    }
  }
  screen[(int)cloud_x][(int)cloud_y] = '*';  // 雲の位置に'*'を配置
}

void move_cloud() {
  cloud_x -= 0.5;  // 雲を左に動かす（少しずつ移動）

  // 雲が画面の左端を超えた場合、右端に戻す
  if (cloud_x < 0) {
    cloud_x = SCR_WIDTH - 2;  // 右端に戻す
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

        // キャラクター（2x2）と壁の衝突判定
        for (int j = 0; j < 2; j++) {
          for (int k = 0; k < 2; k++) {
            if ((charachter_x + k >= walls[i].x &&
                 charachter_x + k < walls[i].x + walls[i].width) &&
                (charachter_y + j >= walls[i].y &&
                 charachter_y + j < walls[i].y + walls[i].height)) {
              game_over_flg = 1;  // ゲームオーバー
            }
          }
        }

        // 壁が画面左端を超えたら消去
        if (walls[i].x + walls[i].width <= 0) {
          walls[i].active = 0;  // 壁を非アクティブにする
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

void rowof_circles(int x, int y, double r) {
  setpos(x, y);   // 描画開始位置
  circle(r * 1);  // 半径 r の円を描画
}

void draw_cloud(int x, int y, int r, int g, int b) {
  pencolor((double)r, (double)g, (double)b);
  fillcolor((double)r, (double)g, (double)b);
  penup();
  set_dir(0);
  pendown();
  begin_fill();
  int rad = 10;
  int w;
  // 雲の形状を作るために円を並べて描画
  for (w = x; w <= x + rad * 5; w += 2 * rad) {
    rowof_circles(w, y, rad);                  // 通常の円を描画
    rowof_circles(w + rad, y + rad / 2, rad);  // 少しずらして円を描画
    rowof_circles(w + rad, y - rad / 2, rad);  // 少しずらして円を描画
  }
  rowof_circles(x + rad * 6, y, rad);
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
        case '#':
          draw_square(scr_x, scr_y, edge_size, 1, 1, 1);
          break;
        case 'w':
          draw_square(scr_x, scr_y, edge_size, 1, 1, 0);
          break;
        case 'o':
          draw_square(scr_x, scr_y, edge_size, 0, 1, 1);
          break;
        case '*':
          draw_cloud(scr_x, scr_y, 1, 1, 1);
          break;  // 雲を描画
      }
    }
  }
  // 画面上部に点数を表示する．sprintf はほとんど printf と同じように
  // 使えるが，「sprintf(score_str, "SCORE: %10d", score);」とすると，
  // 出力文字列が（画面ではなく）文字列のための配列変数 score_str に
  // 格納される．文字列 score_str は，my_turtle.o の
  // put_text 関数で出力する．
  pencolor(1.0, 1.0, 1.0);
  setpos(translate_x(60), translate_y(2));
  put_text(highscore_str, 'h', 18);
  sprintf(highscore_str, "HIGHSCORE: %10d", highscore);
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
        walls[i].width = 2;
        walls[i].height = 3;  // 幅を6に設定
        break;                // 最初に空いている壁位置に追加
      }
    }
  } else {
    wall_generation_counter++;
  }

  if (game_over_flg == 0) {
    // ゲームオーバーでなかったら．．．
    jump();
    move_cloud();
    score += 1;  // スコアを10点増やす．
    if (score % 10 == 0) {
      speedup += 100;
    }
    highscore = maxof(highscore, score);
    usleep(0.02 * 800 * 1000 -
           speedup);  // 時間調整（待ち時間を減らすと，より高速になる）
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
  runGL(EDGE, 2.0, "Jumping GAME", 1, &argc, argv);
}