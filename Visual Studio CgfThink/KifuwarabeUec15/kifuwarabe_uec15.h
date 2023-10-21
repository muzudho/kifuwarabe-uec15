// cgfthink.h
// �A�v���P�[�V��������Ă΂��֐��̐錾

#define DLL_EXPORT	__declspec( dllexport )

// �v�l���[�`���B�{�̂��猻�݂̎萔�Ƃ���܂ł̊�������������ԂŌĂ΂��B
// ��̍��W��Ԃ��BPASS�̏ꍇ0���B
// �܂��A�I�Ǐ����̏ꍇ�́A�I�ǔ��f�̌��ʂ�Ԃ��B
DLL_EXPORT int cgfgui_thinking(
	int init_board[],	// �����Ֆʁi�u��̏ꍇ�́A�����ɒu�΂�����j
	int kifu[][3],		// ����  [][0]...���W�A[][1]...�΂̐F�A[][2]...����ԁi�b)
	int tesuu,			// �萔
	int black_turn,		// ���(����...1�A����...0)
	int board_size,		// �Ֆʂ̃T�C�Y
	double komi,		// �R�~
	int endgame_type,	// 0...�ʏ�̎v�l�A1...�I�Ǐ����A2...�}�`��\���A3...���l��\���B
	int endgame_board[]	// �I�Ǐ����̌��ʂ�������B
);

// �΋ǊJ�n���Ɉ�x�����Ă΂�܂��B
DLL_EXPORT void cgfgui_thinking_init(int* ptr_stop_thinking);

// �΋ǏI�����Ɉ�x�����Ă΂�܂��B
DLL_EXPORT void cgfgui_thinking_close(void);


#define BLACK 1
#define WHITE 2
#define WAKU  3		// �ՊO

// ���݋ǖʂŉ������邩�A���w��
enum GameType {
	GAME_MOVE,			// �ʏ�̎�
	GAME_END_STATUS,	// �I�Ǐ���
	GAME_DRAW_FIGURE,	// �}�`��`��
	GAME_DRAW_NUMBER 	// ���l������
};

// �ՖʁA�΂̏�ɕ`���L��
// (�` | �F) �Ŏw�肷��B���Ŏl�p��`���ꍇ�� (FIGURE_SQUARE | FIGURE_BLACK)
enum FigureType {
	FIGURE_NONE,			// �����`���Ȃ�
	FIGURE_TRIANGLE,		// �O�p�`
	FIGURE_SQUARE,			// �l�p
	FIGURE_CIRCLE,			// �~
	FIGURE_CROSS,			// �~
	FIGURE_QUESTION,		// "�H"�̋L��	
	FIGURE_HORIZON,			// ����
	FIGURE_VERTICAL,		// �c��
	FIGURE_LINE_LEFTUP,		// �΂߁A���ォ��E��
	FIGURE_LINE_RIGHTUP,	// �΂߁A��������E��
	FIGURE_BLACK = 0x1000,	// ���ŕ`���i�F�w��)
	FIGURE_WHITE = 0x2000,	// ���ŕ`��	(�F�w��j
};

// �������ŃZ�b�g����l
// ���̈ʒu�̐΂��u���v���u���v���B�s���ȏꍇ�́u���v�ŁB
// ���̈ʒu�̓_���u���n�v�u���n�v�u�_���v���B
enum GtpStatusType {
	GTP_ALIVE,				// ��
	GTP_DEAD,				// ��
	GTP_ALIVE_IN_SEKI,		// �Z�L�Ŋ��i���g�p�A�u���v�ő�p���ĉ������j
	GTP_WHITE_TERRITORY,	// ���n
	GTP_BLACK_TERRITORY,	// ���n
	GTP_DAME				// �_��
};


// �T���v���Ŏg�p����֐�
void PRT(const char* fmt, ...);	// printf()�̑�p�֐��B�R���\�[���ɏo�́B
void PassWindowsSystem(void);	// �ꎞ�I��Windows�ɐ����n���܂��B
