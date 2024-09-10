#include <iostream>
#include <chrono>
#include "framework.h"
#include "PowerSaverJikken.h"
#include "resource.h"

HINSTANCE hInst;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK MyDlgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    hInst = hInstance;
    DialogBox(hInst, L"MyTestDlgBase_Main", NULL, (DLGPROC)MyDlgProc);
    return (int)0;
}

std::wstring getTimeStamp()
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    
    tm time;
    auto err = localtime_s(&time, &now_c);
    auto timeObj = std::put_time(&time, L"%Y/%m/%d %H:%M:%S");
    
    std::wostringstream ss;
    ss << timeObj;
    return ss.str();
}

void AddMsg(HWND hDlg, std::wstring msg)
{
    auto list = GetDlgItem(hDlg, IDC_LIST1);
    auto msgWithTime = getTimeStamp() + L" " +  msg;
    SendMessage(list, LB_INSERTSTRING, (WPARAM)0, (LPARAM)msgWithTime.c_str());
}

BOOL CALLBACK MyDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
        case WM_INITDIALOG:
        {
            RegisterPowerSettingNotification(hDlg, &GUID_POWER_SAVING_STATUS, DEVICE_NOTIFY_WINDOW_HANDLE);
            RegisterPowerSettingNotification(hDlg, &GUID_ENERGY_SAVER_STATUS, DEVICE_NOTIFY_WINDOW_HANDLE);
            break;
        }

        case WM_POWERBROADCAST:
        {
            if (wp == PBT_POWERSETTINGCHANGE)
            {
                auto lppbc = (POWERBROADCAST_SETTING*)lp;

                // 24H1までの「バッテリー節約」
                if (lppbc->PowerSetting == GUID_POWER_SAVING_STATUS) {
                    if (lppbc->Data[0] == 0)
                        AddMsg(hDlg, L"Power Saving OFF(GUID_POWER_SAVING_STATUS)");
                    else
                        AddMsg(hDlg, L"Power Saving ON(GUID_POWER_SAVING_STATUS)");
                }

                // 24H2以降の「省電力」
                if (lppbc->PowerSetting == GUID_ENERGY_SAVER_STATUS) {
                    if      (lppbc->Data[0] == ENERGY_SAVER_OFF)
                        AddMsg(hDlg, L" ENERGY_SAVER_OFF");
                    else if (lppbc->Data[0] == ENERGY_SAVER_STANDARD)
                        AddMsg(hDlg, L" ENERGY_SAVER_STANDARD");
                    else if (lppbc->Data[0] == ENERGY_SAVER_HIGH_SAVINGS)
                        AddMsg(hDlg, L" ENERGY_SAVER_HIGH_SAVINGS");
                    else
                        AddMsg(hDlg, L" ???");
                }
            }
            break;
        }
    }
    return FALSE;
}