/*http://stackoverflow.com/questions/7305614/include-gdiplus-h-causes-error*/
/*https://vctipsplusplus.wordpress.com/tag/image-conversion-c/*/
/*https://msdn.microsoft.com/en-us/library/ms533837(VS.85).aspx*/

#include "stdafx.h"
#include "ImageUtilities.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

ImageUtilities::ImageUtilities()
{
}


ImageUtilities::~ImageUtilities()
{
}
