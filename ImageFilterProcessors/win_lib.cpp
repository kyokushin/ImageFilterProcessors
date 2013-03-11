#include <string>

#include <Windows.h>

#include "win_lib.h"

using namespace std;


bool fileSelectDialog( std::string& fname ){
	HWND hWnd = NULL;
	char path[MAX_PATH+1];
	char title[129];
	OPENFILENAME openfile;

	//構造体を０で埋める
	memset(&openfile, 0, sizeof(OPENFILENAME));
	//結果を格納するバッファの先頭をヌル文字にする（かなり重要）
	path[0] = '\0';

	//構造体のサイズの指定
	openfile.lStructSize = sizeof(OPENFILENAME);
	//親ウインドウのハンドル（NULL可）
	openfile.hwndOwner = hWnd;
	//フィルタの指定
	openfile.lpstrFilter = "画像\0*.jpg;*.bmp;*.png\0全て(*.*)\0*.*\0\0";
	//結果が格納されるバッファの指定
	openfile.lpstrFile = path;
	//バッファのサイズ
	openfile.nMaxFile = MAX_PATH;
	//各種設定フラグ
	//今回は順に、「正しいパスのみ入力可」「正しいファイル名のみ入力可」「読み取り専用チェックを消す」
	openfile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	//標準の拡張子の指定
	openfile.lpstrDefExt = "jpg";
	//選択されたファイル名が格納される
	openfile.lpstrFileTitle = title;
	//バッファのサイズを指定
	openfile.nMaxFileTitle = 128;
	//ダイアログのタイトル
	openfile.lpstrTitle = "画像の選択";

	//ダイアログを開く
	if(GetOpenFileName(&openfile)){
		//MessageBox(hWnd, (LPCTSTR)path,(LPCTSTR)"フルパス",MB_OK);
		//MessageBox(hWnd, (LPCTSTR)title,(LPCTSTR)"ファイル名",MB_OK);
		fname = path;
		return true;
	}else{
		MessageBox(hWnd, (LPCTSTR)"キャンセルされました。",(LPCTSTR)"キャンセル",MB_OK);
		return false;
	}

}


bool colorPicker( cv::Scalar& color)
{
		COLORREF CustColors2[16];
		CHOOSECOLOR cc;
		cc.lStructSize = sizeof(cc);
		cc.hwndOwner = NULL;
		cc.hInstance = NULL;
		cc.rgbResult = RGB(255,0,0);
		cc.lpCustColors = CustColors2;
		cc.Flags = CC_RGBINIT;
		cc.lCustData = NULL;
		cc.lpTemplateName = NULL;

		//カラーダイアログ表示
		ChooseColor(&cc);

		color[0] = (cc.rgbResult>>16)&255;
		color[1] = (cc.rgbResult>>8)&255;
		color[2] = (cc.rgbResult&255;
}
