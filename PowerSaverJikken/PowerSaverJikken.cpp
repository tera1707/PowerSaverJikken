#include "framework.h"
#include "PowerSaverJikken.h"
#include "resource.h"

// グローバル変数:
HINSTANCE hInst;

// このコード モジュールに含まれる関数の宣言を転送します:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK MyDlgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    hInst = hInstance;

    DialogBox(hInst, L"MyTestDlgBase_Main", NULL, (DLGPROC)MyDlgProc);

    return (int)0;
}

// ダイアログプロシージャ
BOOL CALLBACK MyDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
        case WM_INITDIALOG:
        {
            GUID guid = GUID_CONSOLE_DISPLAY_STATE;
            RegisterPowerSettingNotification(hDlg, &guid, DEVICE_NOTIFY_WINDOW_HANDLE);
            break;
        }

        case WM_POWERBROADCAST:
        {
            if (wp == PBT_POWERSETTINGCHANGE)
            {
                auto lppbc = (POWERBROADCAST_SETTING*)lp;

                // 24H1までの「バッテリー節約」
                if (lppbc->PowerSetting == GUID_POWER_SAVING_STATUS) {
                    if (lppbc->Data[0] == 0)    wprintf(L"Power Saving ON(GUID_POWER_SAVING_STATUS)");
                    else                        wprintf(L"Power Saving OFF(GUID_POWER_SAVING_STATUS)");
                }

                // 24H2以降の「省電力」
                if (lppbc->PowerSetting == GUID_ENERGY_SAVER_STATUS) {
                    if      (lppbc->Data[0] == ENERGY_SAVER_OFF)            wprintf(L"ENERGY_SAVER_OFF");
                    else if (lppbc->Data[0] == ENERGY_SAVER_STANDARD)       wprintf(L"ENERGY_SAVER_STANDARD");
                    else if (lppbc->Data[0] == ENERGY_SAVER_HIGH_SAVINGS)   wprintf(L"ENERGY_SAVER_HIGH_SAVINGS ");
                    else                                                    wprintf(L"?????");
                }
            }
            break;
        }
    }
    return FALSE;
}