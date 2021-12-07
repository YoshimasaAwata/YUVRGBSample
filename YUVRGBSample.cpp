// YUVRGBSample.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <fstream>
#include <cstring>
#include <cmath>
#include <windows.h>
#include <wingdi.h>

const int CIF_WIDTH = 352;
const int CIF_HEIGHT = 288;
const char* INPUT_FILE_NAME = "akiyo_cif.yuv";
const char* OUTPUT_FILE_NAME = "akiyo_cif.bmp";

void read_yuv_file(unsigned char y[], unsigned char u[], unsigned char v[])
{
    std::ifstream inf(INPUT_FILE_NAME, std::ifstream::in | std::ifstream::binary);

    inf.read((char*)y, sizeof(unsigned char) *  CIF_WIDTH * CIF_HEIGHT);
    inf.read((char*)u, sizeof(unsigned char) * (CIF_WIDTH / 2) * (CIF_HEIGHT / 2));
    inf.read((char*)v, sizeof(unsigned char) * (CIF_WIDTH / 2) * (CIF_HEIGHT / 2));

    inf.close();

    return;
}

void calc_rgb(unsigned char y[], unsigned char u[], unsigned char v[], unsigned int xrgb[])
{
    for (int h = 0; h < CIF_HEIGHT; h++)
    {
        auto clip = [](long n) { return (n <= 0) ? 0 : ((n >= 256) ? 255 : n); };

        int y_pos = h * CIF_WIDTH;
        int uv_pos = (h / 2) * (CIF_WIDTH / 2);

        for (int w = 0; w < CIF_WIDTH; w++)
        {
            double y16 = y[y_pos + w] - 16.0;
            double u128 = u[uv_pos + (w / 2)] - 128.0;
            double v128 = v[uv_pos + (w / 2)] - 128.0;

            long r = lrint((1.164 * y16) + ( 0.0   * u128) + ( 1.596 * v128));
            long g = lrint((1.164 * y16) + (-0.392 * u128) + (-0.813 * v128));
            long b = lrint((1.164 * y16) + ( 2.017 * u128) + ( 0.0   * v128));
            r = clip(r);
            g = clip(g);
            b = clip(b);
            xrgb[y_pos + w] = (unsigned int)((r << 16) | (g << 8) | b);
        }
    }

    return;
}

void write_bmp_file(unsigned int xrgb[])
{
    BITMAPFILEHEADER bmpfh;
    BITMAPINFO bmpinfo;
    std::ofstream outf;

    memset(&bmpfh, 0, sizeof(BITMAPFILEHEADER));
    memset(&bmpinfo, 0, sizeof(BITMAPINFO));

    bmpfh.bfType = 0x4D42;  // "BM"
    bmpfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + CIF_WIDTH * CIF_HEIGHT * sizeof(unsigned int);
    bmpfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);

    bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpinfo.bmiHeader.biWidth = CIF_WIDTH;
    bmpinfo.bmiHeader.biHeight = -CIF_HEIGHT;
    bmpinfo.bmiHeader.biPlanes = 1;
    bmpinfo.bmiHeader.biBitCount = 32;
    bmpinfo.bmiHeader.biCompression = BI_RGB;

    outf.open(OUTPUT_FILE_NAME, std::ofstream::out | std::ofstream::binary);

    outf.write((char*)&bmpfh, sizeof(BITMAPFILEHEADER));
    outf.write((char*)&bmpinfo, sizeof(BITMAPINFO));
    outf.write((char*)xrgb, sizeof(unsigned int) * CIF_WIDTH * CIF_HEIGHT);

    outf.close();

    return;
}

int main()
{
    unsigned char y[CIF_WIDTH * CIF_HEIGHT];
    unsigned char u[(CIF_WIDTH / 2) * (CIF_HEIGHT / 2)];
    unsigned char v[(CIF_WIDTH / 2) * (CIF_HEIGHT / 2)];
    unsigned int xrgb[CIF_WIDTH * CIF_HEIGHT];

    read_yuv_file(y, u, v);
    calc_rgb(y, u, v, xrgb);
    write_bmp_file(xrgb);

    return 0;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
