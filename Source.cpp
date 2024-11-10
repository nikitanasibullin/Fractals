#define STRICT
#include <windows.h>
#include <math.h>
#include <complex>
#define MaxIterationsIndex 1000

// Глобальные переменные для хранения текущих границ фрактала
double minx = -2.0, maxx = 2.0, miny = -2.0, maxy = 2.0;

// Параметр c для фрактала Жюлиа
std::complex<double> c(-0.7, 0.27015);

// Прототипы функций
void DrawStudyExample(HWND hWnd);
void Julia(HDC hdc, int xx, int yy, int cx, int cy, double minx, double maxx, double miny, double maxy);
int Iterations(double x, double y);
COLORREF IndexToColor(int index);

// Обработчик сообщений
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_LBUTTONDOWN: {
        // Получаем координаты курсора
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        // Получаем размеры клиентской области окна
        RECT rc;
        GetClientRect(hWnd, &rc);
        double width = maxx - minx;
        double height = maxy - miny;

        // Пересчитываем новые границы фрактала
        double newWidth = width / 4; // Уменьшаем ширину области в 4 раза
        double newHeight = height / 4; // Уменьшаем высоту области в 4 раза

        // Преобразуем координаты мыши в координаты фрактала
        double fractalX = minx + (x / (double)rc.right) * width;
        double fractalY = miny + (y / (double)rc.bottom) * height;

        // Устанавливаем новые границы вокруг точки клика
        minx = fractalX - newWidth / 2;
        maxx = fractalX + newWidth / 2;
        miny = fractalY - newHeight / 2;
        maxy = fractalY + newHeight / 2;

        // Перерисовываем фрактал
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

    //---сначала регистрируем класс главного окна программы---
    WndClass.style = NULL;
    WndClass.lpfnWndProc = WndProc; //адрес функции окна
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.lpszMenuName = TEXT("STUDEXMENU");
    WndClass.lpszClassName = TEXT("StudEx");
    if (!RegisterClass(&WndClass)) return 0;

    // Создаем окно класса StudEx+
    hWnd = CreateWindow(TEXT("StudEx"),
        TEXT("Фрактал Жюлиа"), // заголовок
        WS_OVERLAPPEDWINDOW, // стиль окна
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800, // размеры окна
        800,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hWnd) return NULL;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    //----- организация цикла обработки сообщений -
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
        xk = xx * xx - yy * yy + c.real(); // Используем параметр c
        yk = 2.0 * xx * yy + c.imag(); // Используем параметр c
        xx = xk;
        yy = yk;
        i++;
        if (i >= MaxIterationsIndex) break;
    }
    return i;
}

COLORREF IndexToColor(int index) {
    if (index == MaxIterationsIndex) {
        return RGB(0, 0, 0); // Черный цвет для точек, которые не выходят за границы
    }

    // Нормализуем индекс к диапазону от 0 до 255
    int normalizedIndex = (index * 255) / MaxIterationsIndex;

    // Определяем цвет звезды
    int r = static_cast<int>(normalizedIndex * 0.5); // Уменьшаем красный компонент для фиолетового
    int g = static_cast<int>(normalizedIndex * 0.0); // Зеленый компонент, чтобы избежать зеленого
    int b = normalizedIndex; // Синий компонент для голубого

    // Если яркость слишком низкая, создаем более тусклый фиолетовый
    if (normalizedIndex < 50) {
        return RGB(25, 0, 25); // Тусклый фиолетовый для слабых звезд
    }

    return RGB(r, g, b); // Возвращаем цвет звезды
}
