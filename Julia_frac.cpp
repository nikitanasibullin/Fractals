#define STRICT
#include <windows.h>
#include <math.h>
#include <complex>
#define MaxIterationsIndex 1000

// Ãëîáàëüíûå ïåðåìåííûå äëÿ õðàíåíèÿ òåêóùèõ ãðàíèö ôðàêòàëà
double minx = -2.0, maxx = 2.0, miny = -2.0, maxy = 2.0;

// Ïàðàìåòð c äëÿ ôðàêòàëà Æþëèà
std::complex<double> c(-0.7, 0.27015);

// Ïðîòîòèïû ôóíêöèé
void DrawStudyExample(HWND hWnd);
void Julia(HDC hdc, int xx, int yy, int cx, int cy, double minx, double maxx, double miny, double maxy);
int Iterations(double x, double y);
COLORREF IndexToColor(int index);

// Îáðàáîò÷èê ñîîáùåíèé
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_LBUTTONDOWN: {
        // Ïîëó÷àåì êîîðäèíàòû êóðñîðà
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        // Ïîëó÷àåì ðàçìåðû êëèåíòñêîé îáëàñòè îêíà
        RECT rc;
        GetClientRect(hWnd, &rc);
        double width = maxx - minx;
        double height = maxy - miny;

        // Ïåðåñ÷èòûâàåì íîâûå ãðàíèöû ôðàêòàëà
        double newWidth = width / 4; // Óìåíüøàåì øèðèíó îáëàñòè â 4 ðàçà
        double newHeight = height / 4; // Óìåíüøàåì âûñîòó îáëàñòè â 4 ðàçà

        // Ïðåîáðàçóåì êîîðäèíàòû ìûøè â êîîðäèíàòû ôðàêòàëà
        double fractalX = minx + (x / (double)rc.right) * width;
        double fractalY = miny + (y / (double)rc.bottom) * height;

        // Óñòàíàâëèâàåì íîâûå ãðàíèöû âîêðóã òî÷êè êëèêà
        minx = fractalX - newWidth / 2;
        maxx = fractalX + newWidth / 2;
        miny = fractalY - newHeight / 2;
        maxy = fractalY + newHeight / 2;

        // Ïåðåðèñîâûâàåì ôðàêòàë
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DrawStudyExample(hWnd);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmd, int nCmdShow) {
    MSG msg;
    HWND hWnd;
    WNDCLASS WndClass;

    //---ñíà÷àëà ðåãèñòðèðóåì êëàññ ãëàâíîãî îêíà ïðîãðàììû---
    WndClass.style = NULL;
    WndClass.lpfnWndProc = WndProc; //àäðåñ ôóíêöèè îêíà
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.lpszMenuName = TEXT("STUDEXMENU");
    WndClass.lpszClassName = TEXT("StudEx");
    if (!RegisterClass(&WndClass)) return 0;

    // Ñîçäàåì îêíî êëàññà StudEx+
    hWnd = CreateWindow(TEXT("StudEx"),
        TEXT("Ôðàêòàë Æþëèà"), // çàãîëîâîê
        WS_OVERLAPPEDWINDOW, // ñòèëü îêíà
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800, // ðàçìåðû îêíà
        800,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hWnd) return NULL;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    //----- îðãàíèçàöèÿ öèêëà îáðàáîòêè ñîîáùåíèé -
    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

void DrawStudyExample(HWND hWnd) {
    HDC hdc;
    RECT rc;
    hdc = GetDC(hWnd);
    if (hdc == NULL) return;
    GetClientRect(hWnd, &rc);
    Julia(hdc, 0, 0, rc.right, rc.bottom, minx, maxx, miny, maxy);
    ReleaseDC(hWnd, hdc);
}

void Julia(HDC hdc, int xx, int yy, int cx, int cy, double minx, double maxx, double miny, double maxy) {
    double stepx, stepy;
    int i, j, iter;
    double x, y;

    stepx = (maxx - minx) / (double)cx;
    stepy = (maxy - miny) / (double)cy;
    y = miny;
    for (j = 0; j < cy; j++) {
        x = minx;
        for (i = 0; i < cx; i++) {
            iter = Iterations(x, y);
            SetPixel(hdc, xx + i, yy + j, IndexToColor(iter));
            x += stepx;
        }
        y += stepy;
    }
}

int Iterations(double x, double y) {
    int i;
    double xx, yy, xk, yk;

    xx = x;
    yy = y;
    i = 0;
    while (xx * xx + yy * yy <= 4.0) {
        xk = xx * xx - yy * yy + c.real(); // Èñïîëüçóåì ïàðàìåòð c
        yk = 2.0 * xx * yy + c.imag(); // Èñïîëüçóåì ïàðàìåòð c
        xx = xk;
        yy = yk;
        i++;
        if (i >= MaxIterationsIndex) break;
    }
    return i;
}

COLORREF IndexToColor(int index) {
    if (index == MaxIterationsIndex) {
        return RGB(0, 0, 0); // ×åðíûé öâåò äëÿ òî÷åê, êîòîðûå íå âûõîäÿò çà ãðàíèöû
    }

    // Íîðìàëèçóåì èíäåêñ ê äèàïàçîíó îò 0 äî 255
    int normalizedIndex = (index * 255) / MaxIterationsIndex;

    // Îïðåäåëÿåì öâåò çâåçäû
    int r = static_cast<int>(normalizedIndex * 0.5); // Óìåíüøàåì êðàñíûé êîìïîíåíò äëÿ ôèîëåòîâîãî
    int g = static_cast<int>(normalizedIndex * 0.0); // Çåëåíûé êîìïîíåíò, ÷òîáû èçáåæàòü çåëåíîãî
    int b = normalizedIndex; // Ñèíèé êîìïîíåíò äëÿ ãîëóáîãî

    // Åñëè ÿðêîñòü ñëèøêîì íèçêàÿ, ñîçäàåì áîëåå òóñêëûé ôèîëåòîâûé
    if (normalizedIndex < 50) {
        return RGB(25, 0, 25); // Òóñêëûé ôèîëåòîâûé äëÿ ñëàáûõ çâåçä
    }

    return RGB(r, g, b); // Âîçâðàùàåì öâåò çâåçäû
}
