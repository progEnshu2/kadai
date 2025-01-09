// (Q) 二人で遊べる「スカッシュゲーム」を作れ．
// プログラムコードのところどころが，穴埋めになっている
// （/* ... */ のところなど）

#include "my-turtle.h"
// 文字列生成のための「sprintf」関数を使うため必要
#include <stdio.h>
// 「xマイクロ秒待つ」ための命令「usleep(x);」に必要
#include <unistd.h>

// スクリーンのサイズ
#define EDGE (600)

int racket1_x, racket2_x;  // ラケット1とラケット2の座標
int ball_x, ball_y;        // ボールの（仮想画面上での）X座標とY座標
int ball_vx, ball_vy;      // ボールの各軸方向の進行向き（-1 または 1）
int score;                 // スコア
int game_over_flg;         // ゲームオーバーか？
char score_str[100];       // 画面表示用の作業領域

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
  racket1_x = SCR_WIDTH / 2 - 3;
  racket2_x = SCR_WIDTH / 2 + 3;
  ball_x = SCR_WIDTH / 2;
  ball_y = SCR_HEIGHT / 2;
  ball_vx = -1;
  ball_vy = -1;
  score = 0;
  game_over_flg = 0;
}

// 仮想画面の描画．
void draw_virtual_screen() {
  int x, y;

  // まず，screen[x][y] をすべて空白（' '）にする
  for (y = 0; y < SCR_HEIGHT; y++) {
    for (x = 0; x < SCR_WIDTH; x++) {
      screen[x][y] = ' ';
    }
  }

  // 左右と上部の壁（'#'）を作る
  for (x = 0; x < SCR_WIDTH; x++) screen[x][3] = '#';
  for (y = 3; y < SCR_HEIGHT; y++) {
    screen[0][y] = '#';
    screen[SCR_WIDTH - 1][y] = '#';
  }

  // ラケット1（'='）とラケット2（'-'）を配置
  for (x = 0; x < RACKET_SIZE; x++) {
    screen[racket1_x + x][SCR_HEIGHT - 2] = '=';
    screen[racket2_x + x][SCR_HEIGHT - 2] = '-';
  }

  // ボール（'O'）を配置
  screen[ball_x][ball_y] = 'O';
}

// 大きい方を返す関数
int maxof(int x, int y) {
  if (x > y)
    return x;
  else
    return y;
}

// 仮想画面のX座標から，スクリーン上のX座標に変換する関数
double translate_x(int x) {
  double d_edge, tmp, ret;

  d_edge = maxof(SCR_WIDTH, SCR_HEIGHT);
  tmp = (double)(x - (d_edge / 2));
  if (SCR_WIDTH < SCR_HEIGHT) tmp += (SCR_HEIGHT - SCR_WIDTH) / 2;
  ret = tmp * ((double)EDGE / d_edge);
  return ret;
}

// 仮想画面のY座標から，スクリーン上のY座標に変換する関数
double translate_y(int y) {
  double d_edge, tmp, ret;

  d_edge = maxof(SCR_WIDTH, SCR_HEIGHT);
  tmp = (double)(y - (d_edge / 2));
  if (SCR_HEIGHT < SCR_WIDTH) tmp += (SCR_WIDTH - SCR_HEIGHT) / 2;
  ret = tmp * ((double)EDGE / d_edge);

  return ret;
}

// ラケットを動かす
void move_racket() {
  // ラケット1：'a'が押されたら左に，'s'が押されたら右に動く．
  // 壁にめり込まないように，座標チェックした上で移動させる．
  if (inkey('a') == 1 && racket1_x > 1) racket1_x -= 1;
  /* 's' キーが押されたときの処理 */
  if (inkey('s') == 1 && racket1_x + RACKET_SIZE < SCR_WIDTH - 1)
    racket1_x += 1;
  // ラケット2：';'が押されたら左に，'\''が押されたら右に動く（「'\''」と書く必要あり）．
  // 壁にめり込まないように，座標チェックした上で移動させる．
  /* ';' キーが押されたときの処理 */
  if (inkey('\'') == 1 && racket2_x + RACKET_SIZE < SCR_WIDTH - 1)
    racket2_x += 1;
  if (inkey(';') == 1 && racket2_x > 1) racket2_x -= 1;
}

// ボールを動かす
void move_ball() {
  // ball_x，ball_y に ball_vx，ball_vy を加算して，新しい座標を求める．
  ball_x += ball_vx;
  /* ball_y に関する処理 */
  ball_y += ball_vy;
  // ボールがさらにこのまま動いたとき，
  // ・左右方向にさらに移動したとき「'#'」にぶつかる
  //    → ビープ音を鳴らして，ball_vx の値（1，-1）を反転
  // ・上下方向にさらに移動したとき「'#'」「'='」「'-'」のどれかにぶつかる
  //    → ビープ音を鳴らして，ball_vy の値（1，-1）を反転
  if (screen[ball_x + ball_vx][ball_y] == '#') {
    beep(800, 0.1);
    ball_vx = -ball_vx;
  }
  if (screen[ball_x][ball_y + ball_vy] == '#') {
    beep(1000, 0.1);
    ball_vy = -ball_vy;
  }
  /* '=' との
     衝突時の処理 */
  if (screen[ball_x][ball_y + ball_vy] == '=') {
    beep(800, 0.1);
    ball_vy = -ball_vy;
  }
  /* '-' との
     衝突時の処理 */
  if (screen[ball_x][ball_y + ball_vy] == '-') {
    beep(1000, 0.1);
    ball_vy = -ball_vy;
  }

  // ボールが最下行にある（ball_y == SCR_HEIGHT-1）→ game_over_flg をたてる
  if (ball_y == SCR_HEIGHT - 1) game_over_flg = 1;
}

// サイズ edge_size の塗りつぶした四角（■）を，座標 (x, y) に表示．
// 色は「r，g，b」で指定する．
void draw_square(int x, int y, int edge_size, int r, int g, int b) {
  pencolor((double)r, (double)g, (double)b);
  fillcolor((double)r, (double)g, (double)b);
  penup();
  setpos((double)x, (double)y);
  set_dir(0);
  pendown();
  begin_fill();
  /*
    1辺が「edge_size」の
    正方形を描く処理
   */
  forward(edge_size);
  right(90.0);
  forward(edge_size);
  right(90.0);
  forward(edge_size);
  right(90.0);
  forward(edge_size);
  end_fill();
}

// 画面描画を行う
void draw_screen() {
  double scr_x, scr_y, edge_size;
  int x, y;

  // 仮想画面を描く（↓ユーザ定義関数 draw_virtual_screen の呼び出し）
  draw_virtual_screen();

  // 画面をクリア（↓関数 clearscreen を使って黒く塗りつぶす）
  clearscreen(0, 0, 0);

  // 仮想画面の各座標の内容 screen[x][y] を，描画画面に反映する．
  // 関数translate_x，translate_y を使って，仮想画面の座標から
  // 描画画面の座標に変換し，draw_square 関数で描く．
  // draw_square 関数の引数 edge_size の値は，
  // 「edge_size = EDGE/maxof(SCR_WIDTH, SCR_HEIGHT);」
  // で求めれば良い．

  for (y = 0; y < SCR_HEIGHT; y++) {
    for (x = 0; x < SCR_WIDTH; x++) {
      scr_x = translate_x(x);
      scr_y = translate_y(y);
      edge_size = EDGE / maxof(SCR_WIDTH, SCR_HEIGHT);

      switch (screen[x][y]) {  // 仮想画面 (x, y) にあるキャラクターが．．．
        case ' ':
          draw_square(scr_x, scr_y, edge_size, 0, 0, 0);
          break;  // ' ' のとき
          /* '#' のとき */
        case '#':
          draw_square(scr_x, scr_y, edge_size, 1, 1, 1);
          break;
          /* '=' のとき */
        case '=':
          draw_square(scr_x, scr_y, edge_size, 1, 1, 0);
          break;
          /* '-' のとき */
        case '-':
          draw_square(scr_x, scr_y, edge_size, 0, 1, 1);
          break;
          /* 'O' のとき */
        case 'O':
          draw_square(scr_x, scr_y, edge_size, 0, 0, 1);
          break;
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
  sprintf(score_str, "SCORE: %10d", score);
  put_text(score_str, 'h', 18);
}

// 事実上のメイン処理
void disp_body(void) {
  // 画面描画を行う
  draw_screen();

  if (game_over_flg == 0) {
    // ゲームオーバーでなかったら．．．
    move_racket();  // ラケットを動かし，
    move_ball();    // ボールを動かし，
    score += 10;    // スコアを10点増やす．
    usleep(0.02 * 1000 *
           1000);  // 時間調整（待ち時間を減らすと，より高速になる）
  } else {
    // ゲームオーバーの処理．
    pencolor(1.0, 1.0, 1.0);
    setpos(translate_x(SCR_WIDTH / 2 - 5), translate_y(SCR_HEIGHT / 2));
    put_text("GAME OVER", 'h', 18);
    setpos(translate_x(SCR_WIDTH / 2 - 8), translate_y(SCR_HEIGHT / 2 + 2));
    put_text("Press [r] to play again", 'h', 18);
    if (inkey('r') == 1) init();
  }
}

// main関数
int main(int argc, char *argv[]) {
  init();
  runGL(EDGE, 2.0, "Squash GAME", 1, &argc, argv);
}
