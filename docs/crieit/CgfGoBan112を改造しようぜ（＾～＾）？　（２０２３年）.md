# 他の記事

📖　[CGFGoban109を改造しようぜ☆（＾～＾）](https://qiita.com/muzudho1/items/35941c1e56d7a5b0d0e9)  

# cgfthink

## x86

多分、 cgfgoban.exe が恐らく x86 なので、 cgfthink.dll も x64 ではなく **x86** で作らなければいけないと思う 

## 構成の種類

アプリケーション（ `.exe` ）ではなく **ダイナミック ライブラリ** （ `.dll` ）に変更  

## ヘッダーファイルが読み込めない場合

`cgfthink.h` が上手く読み込めない場合は、 `#include "cgfthink.h"` の箇所に　そのファイルの内容をベタ書きする  

## const char*

Original:  

```cpp
	char* str[4] = { "・","●","○","＋" };
```

Refactoring:  

```cpp
	const char* str[4] = { "・","●","○","＋" };
```

## LPCWSTR

Original:  

```cpp
SetConsoleTitle("CgfgobanDLL Infomation Window");
````

Refactoring:  

```cpp
// 関数名の末尾にA が付いた
SetConsoleTitleA("CgfgobanDLL Infomation Window");
```

📖　[Set console title in C++ using a string](https://stackoverflow.com/questions/13219182/set-console-title-in-c-using-a-string)

## _vsnprintf

Original:  

```cpp
	len = _vsnprintf(text, PRT_LEN_MAX - 1, fmt, ap);
```

Refactoring:  

```cpp
// 関数名の末尾に _s が付いた
// _TRUNCATE という引数が増えた
len = _vsnprintf_s(text, PRT_LEN_MAX - 1, _TRUNCATE, fmt, ap);
```

## Fail GetProcAddress() .. 2

多分、もともと `.c` のファイルを `.cpp` にすると　おかしくなるのかもしれない。

# cgfgoban

## st

Original:  

```c++
	char *st[5] = { "＋","●","○","★","☆" };	// 埋まったのが黒。空洞が白
```

Refactoring:  

```c++
	const char *st[5] = { "＋","●","○","★","☆" };	// 埋まったのが黒。空洞が白
```

他同様

## sBC

Original:  

```c++
static char *sBC[SBC_NUM] = {
"●","◆","★",
"○","◇","☆","◯",	// 0x819b, 0x81fc,コードが違う。
"┏","┯","┓",
"┠","┼","┨",
"┗","┷","┛",
"・","－","＋",
"｜","－","＋",
"┌","┬","┐",
"├","╋","┤",
"└","┴","┘",
			
};
```

Refactoring:  

```c++
static const wchar_t *sBC[SBC_NUM] = {
L"●",L"◆",L"★",
L"○",L"◇",L"☆",L"◯",	// 0x819b, 0x81fc,コードが違う。
L"┏",L"┯",L"┓",
L"┠",L"┼",L"┨",
L"┗",L"┷",L"┛",
L"・",L"－",L"＋",
L"｜",L"－",L"＋",
L"┌",L"┬",L"┐",
L"├",L"╋",L"┤",
L"└",L"┴",L"┘",
			
};
```

他同様  

## PRT

Original:  

```c++
void PRT(char *fmt, ...);			// 可変のリストを持つprintf代用関数
```

Refactoring:  

```c++
void PRT(const char *fmt, ...);		// 可変のリストを持つprintf代用関数
```

## 文字列

Original:  

```c++
	MessageBox( ghWindow, "CgfGoBan's Internal Error!\nIf you push OK, Program will be terminated by force.", "Debug!", MB_OK);
```

Refactoring:  

```c++
    // 文字列の頭に L を付けた
	MessageBox( ghWindow, L"CgfGoBan's Internal Error!\nIf you push OK, Program will be terminated by force.", L"Debug!", MB_OK);
```

他同様  

## LPSTR, LPWSTR

Original:  

```c++
	LPSTR lpStr;
```

Refactoring:  

```c++
    // W が付いている方に思い切って変える
	LPWSTR lpwStr;
```

他同様

## char xTmp[256]

Original:  

```c++
	char sTmp[256];
```

Refactoring:  

```c++
    // 仕方ないので、思い切って変える
	wchar_t sTmp[256];
```

Original:  

```c++
	sprintf( sTmp,"%d手",tesuu);
```

Refactoring:  

```c++
	wsprintf( sTmp,L"%d手",tesuu);
```

Original:  

```c++
	nLen = strlen( lpwStr );
```

Refactoring:  

```c++
	nLen = wcslen( lpwStr );
```

📖　[vsnprintf_s, _vsnprintf_s, _vsnprintf_s_l, _vsnwprintf_s, _vsnwprintf_s_l](https://learn.microsoft.com/ja-jp/cpp/c-runtime-library/reference/vsnprintf-s-vsnprintf-s-vsnprintf-s-l-vsnwprintf-s-vsnwprintf-s-l?view=msvc-170)  

Original:  

```c++
			for (k=0;k<SBC_NUM;k++) if ( strncmp(p,sBC[k],2) == 0 ) break;
```

Refactoring:  

```c++
			for (k=0;k<SBC_NUM;k++) if ( wcsncmp(p,sBC[k],2) == 0 ) break;
```

芋づる式に変える

## strcpy

Original:  

```c++
	strcpy(lpStr,SgfBuf);
```


