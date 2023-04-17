#include "Globals.h"
// FURYWOLF#9999 
// SIMPLE ColorAimbot Using OpenCV.
// Note that we take screenshot With win API but we copy it into openCV and we use openCV to check for colors etc...
// the code is a bit messy,i'll leave the rest for you.
// try to limit fps on a stable one, or you'll have a stuttering issue in game.


using namespace cv;
using namespace std;



//this is our target color we give it.
Scalar targetColor(52, 52, 255);//we are using BGR in our current project
// the color i gave it, its kinda weird, its aiming at his DICK in close range for some reason. ig you can fix it by making X and Y offset of the aim cords. i'll leave that for you to make it urself its simple af.


//tolerance or threshold, this is the range of our color, so if we give it 1 pixel, it'll not work smoothly and great, so we give it a bunch of Pixels, you can also do this by giving it like 20-30 Colors
//so instead of doing that, we're just making a tolerance and we're making a range like so.
int colorTolerance = 30;

// Calculate the lower and upper color bounds with the given tolerance or Threshold
Scalar lowerColorBound(
    max(targetColor[0] - colorTolerance, 0.0),
    max(targetColor[1] - colorTolerance, 0.0),
    max(targetColor[2] - colorTolerance, 0.0)
);
Scalar upperColorBound(
    min(targetColor[0] + colorTolerance, 255.0),
    min(targetColor[1] + colorTolerance, 255.0),
    min(targetColor[2] + colorTolerance, 255.0)
);

//getting bitMap to the mat where we'll use it later on.
void GetBitmapToMat(HBITMAP hBitmap, Mat& mat) {
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    int width = bmp.bmWidth;
    int height = bmp.bmHeight;

    BITMAPINFO bi;
    memset(&bi, 0, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    mat.create(height, width, CV_8UC4);
    GetDIBits(GetDC(0), hBitmap, 0, height, mat.data, &bi, DIB_RGB_COLORS);
}
//getting da center X and Y, there is other simpler way to do this...
POINT GetScreenCenter() {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    POINT center;
    center.x = screenWidth / 2;
    center.y = screenHeight / 2;

    return center;
}



//main, where all the goofy happens.
int main() {

    cout << "waiting for game window" << endl;
    while (!globals::hWnd) {
        globals::hWnd = FindWindowA("Respawn001", 0);
        Sleep(1000);

        break;
    }
    system("cls");

    // Set up screen capture
    HDC hDC = GetDC(globals::hWnd);
    HDC hCaptureDC = CreateCompatibleDC(hDC);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDC, screenWidth, screenHeight);
    SelectObject(hCaptureDC, hCaptureBitmap);
    while (true) {
        // Capture the screen
        if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) // if we dont make this on top over here, our pc would burn, it'd scan 24/7
        {
            BitBlt(hCaptureDC, 0, 0, screenWidth, screenHeight, hDC, 0, 0, SRCCOPY | CAPTUREBLT);
            Mat screen;
            //converting the bitmap image to the and we give it to Mat Screen we have.
            GetBitmapToMat(hCaptureBitmap, screen);
            cvtColor(screen, screen, COLOR_BGRA2BGR);

            // Get the scanning area to center and Calculate the size.
            POINT center = GetScreenCenter();

            Rect centerRect(center.x - globals::ScanningSize / 2, center.y - globals::ScanningSize / 2, globals::ScanningSize, globals::ScanningSize);
            Mat croppedScreen = screen(centerRect);

            // Threshold or tolerance for that specific color we gave it.
            Mat colorMask;
            inRange(croppedScreen, lowerColorBound, upperColorBound, colorMask);

            // Find the centroid of the color in the scannned image
            Moments m = moments(colorMask, true);
            int x = (int)(m.m10 / m.m00);
            int y = (int)(m.m01 / m.m00);

            // Move the mouse to the color centroid if found, Note that if you dont do this if statement your aimbot still works but when the color isnt found it'll go crazy, so here we are making sure the color is found
            if (m.m00 > 0) {
                //some Other calculation for the cordinates of the Color and CenterX, so we move Center X to the Color not the actual cursor. 
                float aimX = (center.x - globals::ScanningSize / 2 + x) - center.x;
                float aimY = (center.y - globals::ScanningSize / 2 + y) - center.y;

                //Smoothing calculation, Depending on  the PC performance + Ingame sensitivity, this will have to be changed, Higher/lower
                aimX /= globals::Smoothing;
                aimY /= globals::Smoothing;

                //using mouse event to move mouse.
                mouse_event(MOUSEEVENTF_MOVE, aimX, aimY, 0, 0);
            }

            Sleep(1);  // The delay of scanning speed, basically the Higher this means the More laggy the aimbot and the less CPU it takes. if you have a Shitty PC, you'll just burn it down.
        }
    }

    // Clean up stff
    DeleteObject(hCaptureBitmap);
    DeleteDC(hCaptureDC);
    ReleaseDC(globals::hWnd, hDC);

    return 0;
}


