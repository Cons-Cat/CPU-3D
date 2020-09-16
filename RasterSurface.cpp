// The RasterSurface is a simple & efficient way to copy a block of pixels to the screen.
// Author: L.Norri CD GX1 & GX2, FullSail University

#include "RasterSurface.h" // definitions
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wingdi.h>
#include <thread>
#include <future>
#include <mutex>
#include <atomic>

// variables used by the RasterSurface
HWND window = nullptr;
HDC windowDC = nullptr;
std::thread windowHandler;
DWORD windowHandlerID = -1;
std::atomic_bool windowClosed;
unsigned int *bitmap = nullptr;
unsigned int bitmapWidth = 0;
unsigned int bitmapHeight = 0;
std::mutex bitmapMutex;
std::condition_variable bitmapRedraw;
std::future<unsigned int *> bitmapAllocator;
std::atomic_bool bitmapPresent;

// Handles all windows messages (Messages may arrive cross-thread without a valid HWND)
// hWnd may be set artifically due to cross-thread message posting (NULL HWNDs are ignored)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case (WM_DESTROY):
    {
        windowClosed = true;       // window closing, updates disabled
        bitmapRedraw.notify_one(); // tell main to stop waiting for a redraw and exit
        // close down the window
        window = nullptr; // dont stall get message
        PostQuitMessage(0);
        break;
    }
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

// This function transfers the internal block of pixels to the screen
// It will also update the title bar FPS once every second
bool PresentFrame()
{
    // update screen contents & increment frame count
    if (bitmap && window && windowDC) // bitmap should be allocated at this point
    {
        // lock down the bitmap object and use it to paint to the window surface
        std::unique_lock<std::mutex> pixelLock(bitmapMutex);
        // SetDIBitsToDevice version
        BITMAPINFO toDraw;
        ZeroMemory(&toDraw, sizeof(BITMAPINFO));
        toDraw.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        toDraw.bmiHeader.biWidth = bitmapWidth;
        toDraw.bmiHeader.biHeight = -int(bitmapHeight); // flip
        toDraw.bmiHeader.biPlanes = 1;
        toDraw.bmiHeader.biBitCount = 32;
        toDraw.bmiHeader.biCompression = BI_RGB;
        // Draw to frontbuffer
        SetDIBitsToDevice(windowDC, 0, 0, bitmapWidth, bitmapHeight, 0, 0, 0,
                          bitmapHeight, bitmap, &toDraw, DIB_RGB_COLORS);
        // increase frame count and notify render thread to continue
        bitmapPresent = false;     // increase frame count
        bitmapRedraw.notify_one(); // tell main thread to continue rendering
        // Update visible frame rate every second
        static ULONGLONG frameCount = 0;
        ++frameCount;
        static ULONGLONG framesPast = frameCount;
        static ULONGLONG prevCount = GetTickCount();
        if (GetTickCount64() - prevCount > 1000) // only update every second
        {
            char buffer[256];
            sprintf_s(buffer, "Raster Surface. FPS: %d", static_cast<int>(frameCount - framesPast));
            SetWindowTextA(window, buffer);
            framesPast = frameCount;
            prevCount = GetTickCount64();
        }
        return true;
    }
    return false;
}

// This thread will handle all updates to the window
void ProcessRasterSurface(unsigned int _width, unsigned int _height,
                          std::promise<unsigned int *> bitmapInit)
{
    // DIB pixel buffers may not be fragmented across heaps
    // VirtualAlloc garuntees true memory contiguity (needed by SetDIBitsToDevice)
    unsigned int *frontbuffer = (unsigned int *)VirtualAlloc(nullptr, (_width * _height) << 2,
                                                             MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    bitmapInit.set_value(frontbuffer); // fufill promise
    // Create a win32 window and manage it on this thread
    WNDCLASSEX wndClass;
    ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_OWNDC; // optimization
    wndClass.lpfnWndProc = WndProc;
    wndClass.lpszClassName = L"RasterSurfaceApplication";
    wndClass.hInstance = GetModuleHandleW(0);
    wndClass.hCursor = LoadCursorW(0, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
    wndClass.hIcon = LoadIconW(0, IDI_APPLICATION);
    RegisterClassExW(&wndClass);
    // adjust window size to contain backbuffer
    RECT window_size = {0, 0, _width, _height};
    AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);
    // Launch window and start managment on other thread
    window = CreateWindowW(L"RasterSurfaceApplication", L"Raster Surface",
                           WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
                           CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left,
                           window_size.bottom - window_size.top,
                           NULL, NULL, GetModuleHandleW(0), 0);
    // Present visible window
    if (window)
    {
        ShowWindow(window, SW_SHOW);
        windowDC = GetDC(window);
        // Handle window messages
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        while (msg.message != WM_QUIT)
        {                                               // handles ANY message on this thread (not just ones bound to the HWND)
            if (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE)) // HWND NULL to listen for "PostThreadMessage"
            {
                // we assume all messages are directed at our window
                if (msg.hwnd == NULL) // this is a cross-thread message
                    msg.hwnd = window;
                // The below funtions will not operate on NULL HWND messages
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            // If any frames are ready to be presented we should do so
            // This operation is synchronized with "RS_Update"
            if (bitmapPresent)
                PresentFrame();
        }
    }
    // delete the front buffer "aka: bitmap" (before thread shuts down)
    VirtualFree(frontbuffer, 0, MEM_RELEASE);
    bitmap = nullptr;
    // deallocate window
    UnregisterClassW(L"RasterSurfaceApplication", GetModuleHandleW(0));
}

// Handles unexpected termination of the console window.
BOOL WINAPI ConsoleCtrlHandler(DWORD ctrlCode);

// Spawns & manages a win32 window of the requested size. (the "RasterSurface")
bool RS_Initialize(_In_range_(1, 0xFFFF) unsigned int _width,
                   _In_range_(1, 0xFFFF) unsigned int _height)
{
    // Create a win32 window and manage it on another thread
    bitmapPresent = false;  // no bitmap is available yet
    windowClosed = false;   // window is being created
    bitmapWidth = _width;   // save x size
    bitmapHeight = _height; // save y size
    // bitmap creation will be fufilled on secondary thread. (allow immediate drawing)
    std::promise<unsigned int *> bitmapGen;
    bitmapAllocator = bitmapGen.get_future();
    // handle messages & buffer updates on dedicated thread
    windowHandler = std::thread(ProcessRasterSurface, _width, _height, std::move(bitmapGen));
    windowHandlerID = GetThreadId(static_cast<HANDLE>(windowHandler.native_handle())); // what is the new thread's ID?
    // allows gracefull exit when console window is closed
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
    return true;
}

// Updates the RasterSurface with a block of raw XRGB pixel data.
// Incoming data must 32bit pixels 8 bits per channel.
bool RS_Update(_In_reads_(_numPixels) const unsigned int *_argbPixels,
               _In_range_(1, 0xFFFFFFFF) unsigned int _numPixels)
{
    // Wait for the drawing surface to intialize
    if (bitmapAllocator.valid())
        bitmap = bitmapAllocator.get(); // retreive allocated value (blocking)
    // begin transfer
    if (bitmap)
    { // track the current frame being rendered
        static unsigned int internalCount = 0;
        // if we have a valid window, lets transfer the memory block to the screen
        {
            std::unique_lock<std::mutex> pixelLock(bitmapMutex); // protect bitmap
            // wait for last paint to occur if we are ahead
            bitmapRedraw.wait(pixelLock, [&]() {
                return !bitmapPresent || windowClosed;
            });
            // if the window has been closed, allow no more updates
            if (windowClosed)
                return false;
            // copy bitmap data so we can continue to
            // draw while it is transfered to frontbuffer
            memcpy_s(bitmap, _numPixels << 2, _argbPixels, _numPixels << 2);
            // notify win32 thread we are ready to present the new image
            bitmapPresent = true;
        }
        return true;
    }
    return false;
}

// Deallocates the RasterSurface and cleans up any leftover memory.
bool RS_Shutdown()
{
    // tell window to close
    PostThreadMessageW(windowHandlerID, WM_DESTROY, 0, 0);
    // wait for thread to yeild
    windowHandler.join();
    // the bitmap is released by the window
    window = nullptr;
    windowDC = nullptr;
    bitmap = nullptr;
    return true;
}
// Handles unexpected termination of the console window.
BOOL WINAPI ConsoleCtrlHandler(DWORD ctrlCode)
{
    // Cleanly exit in case of unclean close
    if (ctrlCode == CTRL_BREAK_EVENT || ctrlCode == CTRL_CLOSE_EVENT ||
        ctrlCode == CTRL_LOGOFF_EVENT || ctrlCode == CTRL_SHUTDOWN_EVENT)
        RS_Shutdown(); // kill window and wait for shutdown
    // allow other handlers to end process
    return FALSE;
}
