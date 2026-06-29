#include    "main.h"
#include    "Application.h"

//=======================================
//エントリーポイント
//=======================================
int main(void)
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif//defined(DEBUG) || defined(_DEBUG)

	//FreeConsole();

	// アプリケーション実行
	Application app(SCREEN_WIDTH, SCREEN_HEIGHT);
	app.Run();

	return 0;
}

//変数,定名の決まり！！！！！！！！！！！！！！！！！！！！！！！！！
//メンバ変数の頭にはm_をつける
//変更するものは区切りの部分が大文字&_でつなぐ
//変更しないもの,const定数はすべて大文字
//ポインタは頭にpを付ける
//ローカル変数は全部小文字

//include循環をしないためにclassは前方宣言！

//1.0fで1cmとする
