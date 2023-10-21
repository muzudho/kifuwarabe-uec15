﻿// KifuwarabeUec15.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

//#include <iostream>
//
//int main()
//{
//    std::cout << "Hello World!\n";
//}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します

// CgfGoban.exe用の思考ルーチンのサンプル
// 2005/06/04 - 2005/07/15 山下 宏
// 乱数で手を返すだけです。
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <windows.h>

#include "kifuwarabe_uec15.h"


// 思考中断フラグ。0で初期化されています。
// GUIの「思考中断ボタン」を押された場合に1になります。
int* pThinkStop = NULL;


#define BOARD_MAX ((19+2)*256)			// 19路盤を最大サイズとする

int board[BOARD_MAX];
int check_board[BOARD_MAX];		// 既にこの石を検索した場合は1

int board_size;	// 盤面のサイズ。19路盤では19、9路盤では9

// 左右、上下に移動する場合の動く量
int dir4[4] = { +0x001,-0x001,+0x100,-0x100 };

int ishi = 0;	// 取った石の数(再帰関数で使う)
int dame = 0;	// 連のダメの数(再帰関数で使う)
int kou_z = 0;	// 次にコウになる位置
int hama[2];	// [0]... 黒が取った石の数, [1]...白が取った石の数
int sg_time[2];	// 累計思考時間

#define UNCOL(x) (3-(x))	// 石の色を反転させる

// move()関数で手を進めた時の結果
enum MoveResult {
	MOVE_SUCCESS,	// 成功
	MOVE_SUICIDE,	// 自殺手
	MOVE_KOU,		// コウ
	MOVE_EXIST,		// 既に石が存在
	MOVE_FATAL		// それ以外
};



// 関数のプロトタイプ宣言
void count_dame(int tz);				// ダメと石の数を調べる
void count_dame_sub(int tz, int col);	// ダメと石の数を調べる再帰関数
int move_one(int z, int col);			// 1手進める。z ... 座標、col ... 石の色
void print_board(void);					// 現在の盤面を表示
int think_sample(int col);
int get_z(int x, int y);					// (x,y)を1つの座標に変換
int endgame_status(int endgame_board[]);		// 終局処理
int endgame_draw_figure(int endgame_board[]);	// 図形を描く
int endgame_draw_number(int endgame_board[]);	// 数値を書く(0は表示されない)


// 一時的にWindowsに制御を渡します。
// 思考中にこの関数を呼ぶと思考中断ボタンが有効になります。
// 毎秒30回以上呼ばれるようにするとスムーズに中断できます。
void PassWindowsSystem(void)
{
	MSG msg;

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);						// keyboard input.
		DispatchMessage(&msg);
	}
}

#define PRT_LEN_MAX 256			// 最大256文字まで出力可
static HANDLE hOutput = INVALID_HANDLE_VALUE;	// コンソールに出力するためのハンドル

// printf()の代用関数。
void PRT(const char* fmt, ...)
{
	va_list ap;
	int len;
	static char text[PRT_LEN_MAX];
	DWORD nw;

	if (hOutput == INVALID_HANDLE_VALUE) return;
	va_start(ap, fmt);
	len = _vsnprintf_s(text, PRT_LEN_MAX - 1, _TRUNCATE, fmt, ap);
	va_end(ap);

	if (len < 0 || len >= PRT_LEN_MAX) return;
	WriteConsole(hOutput, text, (DWORD)strlen(text), &nw, NULL);
}


// 対局開始時に一度だけ呼ばれます。
DLL_EXPORT void cgfgui_thinking_init(int* ptr_stop_thinking)
{
	// 中断フラグへのポインタ変数。
	// この値が1になった場合は思考を終了してください。
	pThinkStop = ptr_stop_thinking;

	// PRT()情報を表示するためのコンソールを起動する。
	AllocConsole();		// この行をコメントアウトすればコンソールは表示されません。
	SetConsoleTitleA("Kifuwarabe UEC15 - CgfgobanDLL Infomation Window");
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	PRT("デバッグ用の窓です。PRT()関数で出力できます。\n");

	// この下に、メモリの確保など必要な場合のコードを記述してください。
}

// 対局終了時に一度だけ呼ばれます。
// メモリの解放などが必要な場合にここに記述してください。
DLL_EXPORT void cgfgui_thinking_close(void)
{
	FreeConsole();
	// この下に、メモリの解放など必要な場合のコードを記述してください。
}

// 思考ルーチン。次の1手を返す。
// 本体から初期盤面、棋譜、手数、手番、盤のサイズ、コミ、が入った状態で呼ばれる。
DLL_EXPORT int cgfgui_thinking(
	int dll_init_board[],	// 初期盤面
	int dll_kifu[][3],		// 棋譜  [][0]...座標、[][1]...石の色、[][2]...消費時間（秒)
	int dll_tesuu,			// 手数
	int dll_black_turn,		// 手番(黒番...1、白番...0)
	int dll_board_size,		// 盤面のサイズ
	double dll_komi,		// コミ
	int dll_endgame_type,	// 0...通常の思考、1...終局処理、2...図形を表示、3...数値を表示。
	int dll_endgame_board[]	// 終局処理の結果を代入する。
)
{
	int z, col, t, i, ret_z;

	// 現在局面を棋譜と初期盤面から作る
	for (i = 0; i < BOARD_MAX; i++) board[i] = dll_init_board[i];	// 初期盤面をコピー
	board_size = dll_board_size;
	hama[0] = hama[1] = 0;
	sg_time[0] = sg_time[1] = 0;	// 累計思考時間を初期化
	kou_z = 0;

	for (i = 0; i < dll_tesuu; i++) {
		z = dll_kifu[i][0];	// 座標、y*256 + x の形で入っている
		col = dll_kifu[i][1];	// 石の色
		t = dll_kifu[i][2];	// 消費時間
		sg_time[i & 1] += t;
		if (move_one(z, col) != MOVE_SUCCESS) break;
	}

#if 0	// 中断処理を入れる場合のサンプル。0を1にすればコンパイルされます。
	for (i = 0; i < 300; i++) {				// 300*10ms = 3000ms = 3秒待ちます。
		PassWindowsSystem();			// 一時的にWindowsに制御を渡します。
		if (*pThinkStop != 0) break;	// 中断ボタンが押された場合。
		Sleep(10);						// 10ms(0.01秒)停止。
	}
#endif

	// 終局処理、図形、数値を表示する場合
	if (dll_endgame_type == GAME_END_STATUS) return endgame_status(dll_endgame_board);
	if (dll_endgame_type == GAME_DRAW_FIGURE) return endgame_draw_figure(dll_endgame_board);
	if (dll_endgame_type == GAME_DRAW_NUMBER) return endgame_draw_number(dll_endgame_board);

	// サンプルの思考ルーチンを呼ぶ
	if (dll_black_turn) col = BLACK;
	else                  col = WHITE;
	ret_z = think_sample(col);

	PRT("思考時間：先手=%d秒、後手=%d秒\n", sg_time[0], sg_time[1]);
	PRT("着手=(%2d,%2d)(%04x), 手数=%d,手番=%d,盤size=%d,komi=%.1f\n", (ret_z & 0xff), (ret_z >> 8), ret_z, dll_tesuu, dll_black_turn, dll_board_size, dll_komi);
	//	print_board();
	return ret_z;
}


// 乱数に近い評価関数。少し石を取りに行くように。
int think_sample(int col)
{
	int max, ret_z;
	int x, y, z, i, value, capture, z1, flag, safe, k;
	int un_col = UNCOL(col);

	// 実行するたびに違う値が得られるように現在の時刻で乱数を初期化
	srand((unsigned)clock());

	max = -1;
	ret_z = 0;
	for (y = 0; y < board_size; y++) for (x = 0; x < board_size; x++) {
		z = get_z(x, y);
		if (board[z]) continue;
		if (z == kou_z) continue;	// コウ

		value = rand() % 100;
		capture = safe = 0;
		for (i = 0; i < 4; i++) {
			z1 = z + dir4[i];
			k = board[z1];
			if (k == WAKU) safe++;
			if (k == 0 || k == WAKU) continue;
			count_dame(z1);
			if (k == un_col && dame == 1) capture = 1;	// 敵石が取れる
			if (k == col && dame >= 2) safe++;			// 安全な味方に繋がる
			value += (k == un_col) * ishi * (10 / (dame + 1));
		}
		if (safe == 4) continue;	// 眼には打たない。
		if (capture == 0) {		// 石が取れない場合は実際に置いてみて自殺手かどうか判定
			int kz = kou_z;			// コウの位置を退避
			flag = move_one(z, col);
			board[z] = 0;
			kou_z = kz;
			if (flag == MOVE_SUICIDE) continue;	// 自殺手
		}
		//		PRT("x,y=(%d,%d)=%d\n",x,y,value);
		if (value > max) { max = value; ret_z = z; }
	}
	return ret_z;
}

// 現在の盤面を表示
void print_board(void)
{
	int x, y, z;
	const char* str[4] = { "・","●","○","＋" };

	for (y = 0; y < board_size + 2; y++) for (x = 0; x < board_size + 2; x++) {
		z = (y + 0) * 256 + (x + 0);
		PRT("%s", str[board[z]]);
		if (x == board_size + 1) PRT("\n");
	}
}

// 終局処理（サンプルでは簡単な判断で死石と地の判定をしています）
int endgame_status(int endgame_board[])
{
	int x, y, z, sum, i, k;
	int* p;

	for (y = 0; y < board_size; y++) for (x = 0; x < board_size; x++) {
		z = get_z(x, y);
		p = endgame_board + z;
		if (board[z] == 0) {
			*p = GTP_DAME;
			sum = 0;
			for (i = 0; i < 4; i++) {
				k = board[z + dir4[i]];
				if (k == WAKU) continue;
				sum |= k;
			}
			if (sum == BLACK) *p = GTP_BLACK_TERRITORY;
			if (sum == WHITE) *p = GTP_WHITE_TERRITORY;
		}
		else {
			*p = GTP_ALIVE;
			count_dame(z);
			//			PRT("(%2d,%2d),ishi=%2d,dame=%2d\n",z&0xff,z>>8,ishi,dame);
			if (dame <= 1) *p = GTP_DEAD;
		}
	}
	return 0;
}

// 図形を描く
int endgame_draw_figure(int endgame_board[])
{
	int x, y, z;
	int* p;

	for (y = 0; y < board_size; y++) for (x = 0; x < board_size; x++) {
		z = get_z(x, y);
		p = endgame_board + z;
		if ((rand() % 2) == 0) *p = FIGURE_NONE;
		else {
			if (rand() % 2) *p = FIGURE_BLACK;
			else              *p = FIGURE_WHITE;
			*p |= (rand() % 9) + 1;
		}
	}
	return 0;
}

// 数値を書く(0は表示されない)
int endgame_draw_number(int endgame_board[])
{
	int x, y, z;
	int* p;

	for (y = 0; y < board_size; y++) for (x = 0; x < board_size; x++) {
		z = get_z(x, y);
		p = endgame_board + z;
		*p = (rand() % 110) - 55;
	}
	return 0;
}


// (x,y)を1つの座標に変換
int get_z(int x, int y)
{
	return (y + 1) * 256 + (x + 1);
}

// 位置 tz におけるダメの数と石の数を計算。結果はグローバル変数に。
void count_dame(int tz)
{
	int i;

	dame = ishi = 0;
	for (i = 0; i < BOARD_MAX; i++) check_board[i] = 0;
	count_dame_sub(tz, board[tz]);
}

// ダメと石の数える再帰関数
// 4方向を調べて、空白だったら+1、自分の石なら再帰で。相手の石、壁ならそのまま。
void count_dame_sub(int tz, int col)
{
	int z, i;

	check_board[tz] = 1;			// この石は検索済み	
	ishi++;							// 石の数
	for (i = 0; i < 4; i++) {
		z = tz + dir4[i];
		if (check_board[z]) continue;
		if (board[z] == 0) {
			check_board[z] = 1;	// この空点は検索済み
			dame++;				// ダメの数
		}
		if (board[z] == col) count_dame_sub(z, col);	// 未探索の自分の石
	}
}

// 石を消す
void del_stone(int tz, int col)
{
	int z, i;

	board[tz] = 0;
	for (i = 0; i < 4; i++) {
		z = tz + dir4[i];
		if (board[z] == col) del_stone(z, col);
	}
}

// 手を進める。z ... 座標、col ... 石の色
int move_one(int z, int col)
{
	int i, z1, sum, del_z = 0;
	int all_ishi = 0;	// 取った石の合計
	int un_col = UNCOL(col);

	if (z == 0) {	// PASSの場合
		kou_z = 0;
		return MOVE_SUCCESS;
	}
	if (z == kou_z) {
		PRT("move() Err: コウ！z=%04x\n", z);
		return MOVE_KOU;
	}
	if (board[z] != 0) {
		PRT("move() Err: 空点ではない！z=%04x\n", z);
		return MOVE_EXIST;
	}
	board[z] = col;	// とりあえず置いてみる

	for (i = 0; i < 4; i++) {
		z1 = z + dir4[i];
		if (board[z1] != un_col) continue;
		// 敵の石が取れるか？
		count_dame(z1);
		if (dame == 0) {
			hama[col - 1] += ishi;
			all_ishi += ishi;
			del_z = z1;	// 取られた石の座標。コウの判定で使う。
			del_stone(z1, un_col);
		}
	}
	// 自殺手を判定
	count_dame(z);
	if (dame == 0) {
		PRT("move() Err: 自殺手! z=%04x\n", z);
		board[z] = 0;
		return MOVE_SUICIDE;
	}

	// 次にコウになる位置を判定。石を1つだけ取った場合。
	kou_z = 0;	// コウではない
	if (all_ishi == 1) {
		// 取られた石の4方向に自分のダメ1の連が1つだけある場合、その位置はコウ。
		kou_z = del_z;	// 取り合えず取られた石の場所をコウの位置とする
		sum = 0;
		for (i = 0; i < 4; i++) {
			z1 = del_z + dir4[i];
			if (board[z1] != col) continue;
			count_dame(z1);
			if (dame == 1 && ishi == 1) sum++;
		}
		if (sum >= 2) {
			PRT("１つ取られて、コウの位置へ打つと、１つの石を2つ以上取れる？z=%04x\n", z);
			return MOVE_FATAL;
		}
		if (sum == 0) kou_z = 0;	// コウにはならない。
	}
	return MOVE_SUCCESS;
}
