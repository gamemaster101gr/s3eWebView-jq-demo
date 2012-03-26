#include <IwGx.h>
#include "s3eKeyboard.h"
#include "s3eOSExec.h"
#include "s3ePointer.h"
#include "s3eWebView.h"


s3eWebView* webView = 0;
bool continueApp = true;

int32 WebViewFailedLoading(s3eWebView *instance, void *systemData, void *userData)  
{
	//const char* url = (const char*)systemData;
	s3eDebugAssertShow(S3E_MESSAGE_CONTINUE,"Can't open url");
	continueApp = false;
	return 0;
}

int32 WebViewUrlLoading(s3eWebView *instance, void *systemData, void *userData)  
{
	const char* url = (const char*)systemData;
	
	const char* touchlink = "s3e://jqtouch";
	if (strncmp(url, touchlink, strlen(touchlink)) == 0)
	{
		if (s3eOSExecAvailable())
			s3eOSExecExecute("http://www.jqtouch.com/preview/demos/main/", true);
	}
	const char* mobilelink = "s3e://jqmobile";
	if (strncmp(url, mobilelink, strlen(mobilelink)) == 0)
	{
		if (s3eOSExecAvailable())
			s3eOSExecExecute("http://jquerymobile.com/demos/1.1.0-rc.1/", true);
	}
	
	return 0;
}

int32 WebViewFromJavascript(s3eWebView *instance, void *systemData, void *userData)  
{
	//const char* data = (const char*)systemData;
	return 0;
}

//-----------------------------------------------------------------------------
// Rotate the layout on rotation
//-----------------------------------------------------------------------------
static void rotationCallback()
{
    // Draw two panels, side by side
	s3eWebViewResize(webView,0,0, IwGxGetScreenWidth(), IwGxGetScreenHeight());
}

//-----------------------------------------------------------------------------
// Main global function
//-----------------------------------------------------------------------------
int main (int argc, char* argv[])
{
	IwGxInit();
	IwResManagerInit();
	//IwGraphicsInit();

	if (!s3eWebViewAvailable())
        return 0;

	webView = s3eWebViewCreate();
	
	/*s3eWebViewRegister(S3E_WEBVIEW_FAILED_LOADING,WebViewFailedLoading,0,webView);
	*/
	s3eWebViewRegister(S3E_WEBVIEW_CUSTOM_URL_LOADING,WebViewUrlLoading,0,webView);
	s3eWebViewRegister(S3E_WEBVIEW_FROM_JAVASCRIPT,WebViewFromJavascript,0,webView);

	IwGxRegister(IW_GX_SCREENSIZE, rotationCallback);
	s3eWebViewShow(webView, 0, 0, IwGxGetScreenWidth(), IwGxGetScreenHeight());

	s3eDeviceYield(0);
	s3eWebViewNavigate(webView,  "rom://www/index.html");
	
	for (;continueApp;)
	{
		s3eDeviceYield(0);

		s3eKeyboardUpdate();
		s3ePointerUpdate();

		if  (
			(s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_DOWN) ||
            (s3eKeyboardGetState(s3eKeyAbsBSK) & S3E_KEY_STATE_DOWN) ||
            (s3eDeviceCheckQuitRequest()))
            break;

		// Clear the screen
		//IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
		// Start with no lighting
		//IwGxLightingOff();
		// End drawing
		IwGxFlush();
		// Swap buffers
		IwGxSwapBuffers();
	}
	/*s3eWebViewUnRegister(S3E_WEBVIEW_FAILED_LOADING,WebViewFailedLoading,webView);
	*/
	s3eWebViewUnRegister(S3E_WEBVIEW_CUSTOM_URL_LOADING,WebViewUrlLoading,webView);
	s3eWebViewUnRegister(S3E_WEBVIEW_FROM_JAVASCRIPT,WebViewFromJavascript,webView);
	s3eWebViewDestroy(webView);

	//IwGraphicsTerminate();
	IwResManagerTerminate();
	IwGxTerminate();

	return 0;
}