#include <ege.h>
#include <graphics.h>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <shellapi.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shell32.lib")

using namespace ege;
using namespace std;

class button
{
public:
    enum buttonstate
    {
        normal_st,
        hover_st,
        pressed_st
    };
    button(int x, int y, int width, int height, const char *text, const char *batpos, int fontsize)
    {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->fontsize = fontsize;
        this->started = FALSE;
        this->last_pressed = FALSE;
        this->startdely = 0;
        this->shoulddely = 30;

        strncpy(this->text, text, sizeof(this->text) - 1);
        strncpy(this->batpos, batpos, sizeof(this->batpos) - 1);
        this->text[63] = '\0';
        this->batpos[63] = '\0';
        normalcolor = RGB(245, 235, 245);  // 普通背景：浅天蓝
        hovercolor = RGB(235, 220, 240);   // 悬停背景：深一点的天蓝
        pressedcolor = RGB(220, 205, 235); // 按下背景：更深的天蓝
        bordercolor = RGB(200, 180, 210);  // 边框色：清新蓝
        textcolor = RGB(130, 80, 160);     // 文字色：深灰（不刺眼）
        fontName = "微软雅黑";             // 字体
        currentstate = normal_st;          // 默认状态
    }
    bool ismousein(int mousex, int mousey)
    {
        if (mousex >= x && mousex <= x + width && mousey >= y && mousey <= y + height)
        {
            return TRUE;
        }
        return FALSE;
    }
    void draw()
    {
        COLORREF bgColor;
        switch (currentstate)
        {
        case hover_st:
            bgColor = hovercolor;
            break;
        case pressed_st:
            bgColor = pressedcolor;
            break;
        default:
            bgColor = normalcolor;
            break;
        }
        // 1. 绘制按钮背景（纯矩形填充）
        setfillcolor(bgColor);
        fillroundrect((float)x, (float)y, (float)(x + width), (float)(y + height), 5, 5);

        // 2. 绘制按钮边框（纯矩形边框）
        setcolor(bordercolor);
        setlinewidth(2);
        roundrect((float)x, (float)y, (float)(x + width), (float)(y + height), 5, 5);

        // 3. 绘制居中文字
        setcolor(textcolor);
        setfont((float)fontsize, 0.0f, fontName);
        setbkmode(TRANSPARENT); // 文字背景透明
        int textWidth = textwidth(text);
        int textHeight = textheight(text);
        // 计算文字居中坐标
        int textX = x + (width - textWidth) / 2;
        int textY = y + (height - textHeight) / 2;
        outtextxy((float)textX, (float)textY, text);

        // 恢复默认线宽，避免影响其他绘图
        setlinewidth(1);
    }

    void updatestate(int mousex, int mousey, bool ispressed)
    {
        if (last_pressed && ismousein(mousex, mousey) && !ispressed && !started)
        {
            ShellExecuteA(NULL, "open", batpos, NULL, NULL, SW_HIDE);
            started = TRUE;
        }
        if (started)
        {
            currentstate = pressed_st;
            startdely++;
        }
        else if (ismousein(mousex, mousey) && started == FALSE)
        {
            currentstate = hover_st;
        }
        else
        {
            currentstate = normal_st;
        }
        last_pressed = ispressed;
    }
    bool cancheck()
    {
        return (startdely > shoulddely);
    }
    bool isstarted()
    {
        return started;
    }

    void reset()
    {
        started = FALSE;
    }

private:
    int x, y, width, height;
    char text[64], batpos[64];
    buttonstate currentstate;
    COLORREF normalcolor, hovercolor, pressedcolor;
    COLORREF bordercolor, textcolor;
    const char *fontName;
    int fontsize, startdely, shoulddely;
    bool started, last_pressed;
};

bool isexerunning(const std::wstring &processname)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    if (Process32FirstW(hSnapshot, &pe32))
    {
        do
        {
            std::wstring currentprocess = pe32.szExeFile;
            if (_wcsicmp(currentprocess.c_str(), processname.c_str()) == 0)
            {
                CloseHandle(hSnapshot);
                return true;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
    return false;
}

bool isexerunningwithargs(const wstring &processname, const wstring &args)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    if (Process32FirstW(hSnapshot, &pe32))
    {
        do
        {
            std::wstring currentprocess = pe32.szExeFile;
            if (_wcsicmp(currentprocess.c_str(), processname.c_str()) != 0)
            {
                continue;
            }
            HANDLE hProcess = OpenProcess(
                PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                FALSE,
                pe32.th32ProcessID);
            if (hProcess == NULL)
            {
                continue;
            }
            wchar_t cmdline[2048] = {0};
            if (GetModuleFileNameExW(hProcess, NULL, cmdline, 2048) == 0)
            {
                CloseHandle(hProcess);
                continue;
            }
            wstring cmdlinestr = cmdline;
            if (cmdlinestr.find(args) != wstring::npos)
            {
                CloseHandle(hProcess);
                CloseHandle(hSnapshot);
                return true;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
    return false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    // 初始化时添加：
    initgraph(800, 600, INIT_RENDERMANUAL);
    // 禁用GDI绘图中断（Windows底层）
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    setbkcolor(EGERGB(250, 245, 250));
    const char *title = "tuyuanshen";
    initgraph(800, 800, INIT_RENDERMANUAL);
    SetWindowTextA(GetForegroundWindow(), title);
    cleardevice();

    button startbt(300, 200, 200, 100, "启动!", ".\\start.bat", 65);
    button soundbt(200, 350, 100, 50, "启动声音", ".\\sndcpy.bat", 30);
    button closebt(500, 350, 100, 50, "关闭声音", ".\\exit.bat", 30);
    bool flag = TRUE;
    int retry = 0;
    HWND egeHwnd = NULL;
    while (egeHwnd == NULL && retry < 5)
    { // 最多重试5次
        egeHwnd = FindWindowA(NULL, title);
        delay_ms(10);
        retry++;
    }
    // 兜底：如果仍获取失败，直接用前台窗口句柄
    if (egeHwnd == NULL)
    {
        egeHwnd = GetForegroundWindow();
    }

    bool isRunning = true;
    bool lastLeftPressed = false;
    PIMAGE backBuffer = newimage(800, 800);
    int statusx = 150, statusy = 450, statusw = 500, statush = 200;
    while (flag)
    {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(egeHwnd, &mousePos);
        int mousex = mousePos.x;
        int mousey = mousePos.y;
        bool isleftpressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        settarget(backBuffer);

        setbkcolor(EGERGB(242, 237, 245));
        cleardevice();

        startbt.updatestate(mousex, mousey, isleftpressed);
        soundbt.updatestate(mousex, mousey, isleftpressed);
        closebt.updatestate(mousex, mousey, isleftpressed);
        startbt.draw();
        soundbt.draw();
        closebt.draw();

        if (closebt.isstarted())
        {
            soundbt.reset();
            closebt.reset();
        }

        setbkmode(TRANSPARENT); // 只设一次，无需重复设
        setcolor(EGERGB(140, 90, 180));
        setfont(84, 0, "微软雅黑", 0, 0, 700, false, false, false);
        setbkmode(TRANSPARENT);
        outtextxy(220, 53, "好想玩原伸");

        setcolor(EGERGB(140, 90, 180));
        setfont(20, 0, "Impact", 0, 0, 200, false, false, false);
        setbkmode(TRANSPARENT);
        outtextxy(530, 100, "v1.0");

        /*     setcolor(EGERGB(220, 120, 150));
               setfont(72, 0, "微软雅黑", 0, 0, 600, false, false, false);
               setbkmode(TRANSPARENT);
               outtextxy(333, 252, "启动！");
               艾什适合大部分时间使用，尤其适合打对面手短的英雄。
               开镜后要注意输出距离，保持安全距离
               需要有意识地练习伸缩雷管和预瞄，才能熟练掌握
               */

        setcolor(EGERGB(140, 90, 180));
        setfont(30, 0, "微软雅黑", 0, 0, 700, false, false, false);
        setbkmode(TRANSPARENT);
        outtextxy(220, 700, "—————按ESC键退出—————");

        setcolor(EGERGB(188, 168, 204));
        setfillcolor(EGERGB(235, 225, 240));
        setlinewidth(4);
        fillroundrect(statusx, statusy, statusx + statusw, statusy + statush, 20, 20);
        roundrect(statusx, statusy, statusx + statusw, statusy + statush, 20, 20);
        setlinewidth(1);

        if (isexerunning(L"scrcpy.exe"))
        {
            setcolor(EGERGB(76, 136, 110));
            setfont(28, 0, "Comic Sans MS");
            setbkmode(TRANSPARENT);
            outtextxy(statusx + 20, statusy + 20, "scrcpy running ...");
        }
        else
        {
            setcolor(EGERGB(130, 115, 140));
            setfont(28, 0, "Comic Sans MS");
            setbkmode(TRANSPARENT);
            outtextxy(statusx + 20, statusy + 20, "scrcpy ...........");
        }

        if (!isexerunning(L"scrcpy.exe") && startbt.isstarted() && startbt.cancheck())
        {
            startbt.reset();
        }

        if (isexerunning(L"vlc.exe"))
        {
            setcolor(EGERGB(76, 136, 110));
            setfont(28, 0, "Comic Sans MS");
            setbkmode(TRANSPARENT);
            outtextxy(statusx + 20, statusy + 80, "sndcpy running ...");
        }
        else
        {
            setcolor(EGERGB(130, 115, 140));
            setfont(28, 0, "Comic Sans MS");
            setbkmode(TRANSPARENT);
            outtextxy(statusx + 20, statusy + 80, "sndcpy ...........");
        }

        setcolor(EGERGB(160, 100, 180));
        setfont(20, 0, "Comic Sans MS");
        setbkmode(TRANSPARENT);

        outtextxy(statusx + 20, statusy+statush - 35,L"Created by s5k1dayo \u00A9 2026 | v1.0");
        settarget(NULL);
        putimage(0, 0, backBuffer);

        if (kbhit())
        {
            int key = getch();
            if (key == 27)
            {
                flag = FALSE;
            }
        }
        delay_ms(33);
    }

    closegraph();

    return 0;
}