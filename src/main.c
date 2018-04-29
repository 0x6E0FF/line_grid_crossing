#include <windows.h> 
#include <GL/gl.h> 
#include <GL/glu.h> 
 
/* Windows globals, defines, and prototypes */ 
CHAR szAppName[]="Win OpenGL"; 
HWND  ghWnd; 
HDC   ghDC; 
HGLRC ghRC; 
 
int WIDTH = 800;
int HEIGHT = 800;
 
LONG WINAPI MainWndProc (HWND, UINT, WPARAM, LPARAM); 
BOOL bSetupPixelFormat(HDC); 
   
extern GLvoid resize(GLsizei w, GLsizei h);
extern GLvoid initializeGL(GLsizei w, GLsizei h);
extern void mouseClick(int x, int y);
extern void key_pressed(char c);

extern GLvoid drawScene(GLvoid);
 
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{ 
    MSG        msg; 
    WNDCLASS   wndclass; 
	RECT rect;
 
    /* Register the frame class */ 
    wndclass.style         = 0; 
    wndclass.lpfnWndProc   = (WNDPROC)MainWndProc; 
    wndclass.cbClsExtra    = 0; 
    wndclass.cbWndExtra    = 0; 
    wndclass.hInstance     = hInstance; 
    wndclass.hIcon         = LoadIcon (hInstance, szAppName); 
    wndclass.hCursor       = LoadCursor (NULL,IDC_ARROW); 
    wndclass.hbrBackground = NULL; 
    wndclass.lpszMenuName  = szAppName; 
    wndclass.lpszClassName = szAppName; 
 
    if (!RegisterClass (&wndclass) ) 
        return FALSE; 
 
	rect.left = 0;
	rect.right = WIDTH;
	rect.top = 0;
	rect.bottom = HEIGHT;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
 
    /* Create the frame */ 
    ghWnd = CreateWindow (szAppName, 
             "Generic OpenGL Sample", 
			 WS_OVERLAPPEDWINDOW, 
             CW_USEDEFAULT, 
             CW_USEDEFAULT, 
             rect.right - rect.left, 
             rect.bottom - rect.top, 
             NULL, 
             NULL, 
             hInstance, 
             NULL); 
 
    /* make sure window was created */ 
    if (!ghWnd) 
        return FALSE; 
 
    /* show and update main window */ 
    ShowWindow (ghWnd, nCmdShow); 
 
    UpdateWindow (ghWnd); 
 
    /* animation loop */ 
	int index = 0;
    while (1) { 
        /* 
         *  Process all pending messages 
         */ 
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) 
        { 
            if (GetMessage(&msg, NULL, 0, 0) ) 
            { 
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            } else { 
                return TRUE; 
            } 
        } 
		
		if (index++ % 1000 == 0)
		{
			drawScene(); 
			SwapBuffers(ghDC);
		}	
    } 
} 
 
/* main window procedure */ 
LONG WINAPI MainWndProc ( 
    HWND    hWnd, 
    UINT    uMsg, 
    WPARAM  wParam, 
    LPARAM  lParam) 
{ 
    LONG    lRet = 1; 
    PAINTSTRUCT    ps; 
    RECT rect; 
 
    switch (uMsg) { 
 
    case WM_CREATE: 
        ghDC = GetDC(hWnd); 
        if (!bSetupPixelFormat(ghDC)) 
            PostQuitMessage (0); 
 
        ghRC = wglCreateContext(ghDC); 
        wglMakeCurrent(ghDC, ghRC); 
        GetClientRect(hWnd, &rect); 
        initializeGL(rect.right, rect.bottom); 
        break; 
 
    case WM_PAINT: 
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps); 
        break; 
 
    case WM_SIZE: 
        GetClientRect(hWnd, &rect); 
        resize(rect.right, rect.bottom); 
        break; 
 
    case WM_CLOSE: 
        if (ghRC) 
            wglDeleteContext(ghRC); 
        if (ghDC) 
            ReleaseDC(hWnd, ghDC); 
        ghRC = 0; 
        ghDC = 0; 
 
        DestroyWindow (hWnd); 
        break; 
 
    case WM_DESTROY: 
        if (ghRC) 
            wglDeleteContext(ghRC); 
        if (ghDC) 
            ReleaseDC(hWnd, ghDC); 
 
        PostQuitMessage (0); 
        break; 
     
	case WM_LBUTTONUP:
		mouseClick((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
		break;
    case WM_KEYDOWN:
		key_pressed(wParam);
		break;
    default: 
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet; 
} 
 
BOOL bSetupPixelFormat(HDC hdc) 
{ 
	static  PIXELFORMATDESCRIPTOR pfd=                  // pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),                  // Size Of This Pixel Format Descriptor
		1,                              // Version Number
		PFD_DRAW_TO_WINDOW |                        // Format Must Support Window
		PFD_SUPPORT_OPENGL |                        // Format Must Support OpenGL
		PFD_DOUBLEBUFFER,                       // Must Support Double Buffering
		PFD_TYPE_RGBA,                          // Request An RGBA Format
		8,                               // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                       // Color Bits Ignored
		0,                              // No Alpha Buffer
		0,                              // Shift Bit Ignored
		0,                              // No Accumulation Buffer
		0, 0, 0, 0,                         // Accumulation Bits Ignored
		16,                             // 16Bit Z-Buffer (Depth Buffer)
		0,                              // No Stencil Buffer
		0,                              // No Auxiliary Buffer
		PFD_MAIN_PLANE,                         // Main Drawing Layer
		0,                              // Reserved
		0, 0, 0                             // Layer Masks Ignored
	};
    PIXELFORMATDESCRIPTOR *ppfd; 
    int pixelformat; 
 
    ppfd = &pfd; 
 
    
    pixelformat = ChoosePixelFormat(hdc, ppfd); 
 
    if ( (pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0 ) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE; 
} 