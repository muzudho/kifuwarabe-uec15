# 他の記事

📖　[CGFGoban109を改造しようぜ☆（＾～＾）](https://qiita.com/muzudho1/items/35941c1e56d7a5b0d0e9)  

# cgfthink

## x86

多分、 cgfgoban.exe が恐らく x86 なので、 cgfthink.dll も x64 ではなく **x86** で作らなければいけないと思う 

## 構成の種類

アプリケーション（ `.exe` ）ではなく **ダイナミック ライブラリ** （ `.dll` ）に変更  

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
static const char *sBC[SBC_NUM] = {
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

