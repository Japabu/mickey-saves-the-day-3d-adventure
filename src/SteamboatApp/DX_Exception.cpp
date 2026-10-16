#include <windows.h>
#include <stdio.h>
#include "DX_Exception.h"

// 0x100c9270 ??0Exception@DX@@QAE@JPADH0@Z
DX::Exception::Exception(long result, char *file, int line, char *message) : mResult(result)
{
    sprintf(m_szDetails, "%s in %s, line %d: %s", message, file, line, ErrorToString(result));
}

// 0x100c92b0 ??0Exception@DX@@QAA@JPADZZ
DX::Exception::Exception(long result, char *format, ...)
{
    mResult = result;
    m_szDetails[0] = 0;
    wvsprintf(m_szDetails, format, (va_list)(&format + 1));
}

// 0x100c92e0 ??0Exception@DX@@QAE@PAD@Z
DX::Exception::Exception(char *message) : mResult(E_FAIL)
{
    m_szDetails[0] = 0;
    wsprintf(m_szDetails, "%s: %s", message, ErrorToString(E_FAIL));
}

// 0x100c9320 ??1Exception@DX@@UAE@XZ
DX::Exception::~Exception()
{
}

// 0x100c9330 ?MessageBoxA@Exception@DX@@SAHIPADZZ
int DX::Exception::MessageBoxA(unsigned int type, char *format, ...)
{
    char text[256];
    wvsprintf(text, format, (va_list)(&format + 1));
    lstrcat(text, "\r\n");
    return ::MessageBoxA(0, text, 0, type);
}

// 0x100c9380 ?GetResult@Exception@DX@@QAEJXZ
long DX::Exception::GetResult()
{
    return mResult;
}

// 0x100c9390 ?GetErrorString@Exception@DX@@QAEPADXZ
char * DX::Exception::GetErrorString()
{
    return ErrorToString(mResult);
}

// 0x100c93a0 ?GetDetails@Exception@DX@@QAEPADXZ
char * DX::Exception::GetDetails()
{
    return m_szDetails;
}

// 0x100c93b0 ?ReportError@Exception@DX@@QAEHI@Z
int DX::Exception::ReportError(unsigned int type)
{
    return MessageBoxA(type, "%s", m_szDetails);
}

// The messages end in an explicit "\0" as in the DirectX SDK samples' error-string helper (the
// string symbols are two bytes longer than the text); 0x8876017c sits in numeric order, sharing its
// pooled message with 0x8007000e.
// 0x100c93d0 ?ErrorToString@Exception@DX@@KAPADJ@Z
char * DX::Exception::ErrorToString(long error)
{
    switch (error) {
    case 0x80004005:
        return "Generic failure.\0";
    case 0x80004001:
        return "Action not supported.\0";
    case 0x8007000e:
        return "DirectDraw does not have enough memory to perform the operation.\0";
    case 0x80070057:
        return "One or more of the parameters passed to the function are incorrect.\0";
    case 0x88760005:
        return "This object is already initialized.\0";
    case 0x88760028:
        return "Support is currently not available.\0";
    case 0x88760014:
        return "This surface can not be detached from the requested surface.\0";
    case 0x8876000a:
        return "This surface can not be attached to the requested surface.\0";
    case 0x88760037:
        return "An exception was encountered while performing the requested operation.\0";
    case 0x8876005a:
        return "Height of rectangle provided is not a multiple of reqd alignment.\0";
    case 0x8876005f:
        return "Unable to match primary surface creation request with existing primary surface.\0";
    case 0x88760064:
        return "One or more of the caps bits passed to the callback are incorrect.\0";
    case 0x8876006e:
        return "DirectDraw does not support the provided cliplist.\0";
    case 0x88760078:
        return "DirectDraw does not support the requested mode.\0";
    case 0x88760082:
        return "DirectDraw received a pointer that was an invalid DIRECTDRAW object.\0";
    case 0x88760091:
        return "The pixel format was invalid as specified.\0";
    case 0x88760096:
        return "Rectangle provided was invalid.\0";
    case 0x887600a0:
        return "Operation could not be carried out because one or more surfaces are locked.\0";
    case 0x887600aa:
        return "There is no 3D present.\0";
    case 0x887600b4:
        return "Operation could not be carried out because there is no alpha accleration hardware present or available.\0";
    case 0x887600cd:
        return "No cliplist available.\0";
    case 0x887600d2:
        return "Operation could not be carried out because there is no color conversion hardware present or available.\0";
    case 0x887600d7:
        return "Surface doesn't currently have a color key\0";
    case 0x887600dc:
        return "Operation could not be carried out because there is no hardware support of the destination color key.\0";
    case 0x887600d4:
        return "Create function called without DirectDraw object method SetCooperativeLevel being called.\0";
    case 0x887600e1:
        return "Operation requires the application to have exclusive mode but the application does not have exclusive mode.\0";
    case 0x887600e6:
        return "Flipping visible surfaces is not supported.\0";
    case 0x887600f0:
        return "There is no GDI present.\0";
    case 0x887600fa:
        return "Operation could not be carried out because there is no hardware present or available.\0";
    case 0x88760104:
        return "Operation could not be carried out because there is no overlay hardware present or available.\0";
    case 0x88760118:
        return "Operation could not be carried out because there is no appropriate raster op hardware present or available.\0";
    case 0x887600ff:
        return "Requested item was not found.\0";
    case 0x88760122:
        return "Operation could not be carried out because there is no rotation hardware present or available.\0";
    case 0x88760190:
        return "No src color key specified for this operation.\0";
    case 0x88760232:
        return "A DirectDraw object representing this driver has already been created for this process.\0";
    case 0x88760231:
        return "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.\0";
    case 0x88760233:
        return "A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.\0";
    case 0x88760136:
        return "Operation could not be carried out because there is no hardware support for stretching.\0";
    case 0x8876013c:
        return "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.\0";
    case 0x8876013d:
        return "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.\0";
    case 0x88760140:
        return "DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color.\0";
    case 0x8876014a:
        return "Operation could not be carried out because there is no texture mapping hardware present or available.\0";
    case 0x8876014f:
        return "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.\0";
    case 0x88760154:
        return "Operation could not be carried out because there is no hardware support for zbuffer blitting.\0";
    case 0x8876015e:
        return "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.\0";
    case 0x88760168:
        return "The hardware needed for the requested operation has already been allocated.\0";
    case 0x8876017c:
        return "DirectDraw does not have enough memory to perform the operation.\0";
    case 0x8876017e:
        return "The hardware does not support clipped overlays.\0";
    case 0x88760180:
        return "Can only have ony color key active at one time for overlays.\0";
    case 0x88760183:
        return "Access to this palette is being refused because the palette is already locked by another thread.\0";
    case 0x88760234:
        return "This process already has created a primary surface.\0";
    case 0x8876019a:
        return "This surface is already attached to the surface it is being attached to.\0";
    case 0x887601a4:
        return "This surface is already a dependency of the surface it is being made a dependency of.\0";
    case 0x887601ae:
        return "Access to this surface is being refused because the surface is already locked by another thread.\0";
    case 0x887601b8:
        return "Access to surface refused because the surface is obscured.\0";
    case 0x887601c2:
        return "Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have Restore called on it.\0";
    case 0x887601cc:
        return "The requested surface is not attached.\0";
    case 0x887601d6:
        return "Height requested by DirectDraw is too large.\0";
    case 0x887601e0:
        return "Size requested by DirectDraw is too large, but the individual height and width are OK.\0";
    case 0x887601ea:
        return "Width requested by DirectDraw is too large.\0";
    case 0x887601fe:
        return "FOURCC format requested is unsupported by DirectDraw.\0";
    case 0x88760208:
        return "Bitmask in the pixel format requested is unsupported by DirectDraw.\0";
    case 0x88760219:
        return "Vertical blank is in progress.\0";
    case 0x8876021c:
        return "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.\0";
    case 0x88760230:
        return "Rectangle provided was not horizontally aligned on required boundary.\0";
    case 0x88760235:
        return "Software emulation not available.\0";
    case 0x8876023e:
        return "Return if a clipper object is attached to the source surface passed into a BltFast call.\0";
    case 0x88760249:
        return "Windows can not create any more DCs.\0";
    case 0x88760247:
        return "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.\0";
    case 0x88760237:
        return "An attempt was made to set a cliplist for a clipper object that is already monitoring an m_hWnd.\0";
    case 0x88760245:
        return "An attempt was made to set the cooperative level when it was already set to exclusive.\0";
    case 0x8876023b:
        return "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.\0";
    case 0x8876023a:
        return "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.\0";
    case 0x8876024c:
        return "This surface can not be restored because it is an implicitly created surface.\0";
    case 0x88760243:
        return "Returned when the position of the overlay on the destination is no longer legal for that destination.\0";
    case 0x8876023f:
        return "No blitter hardware present.\0";
    case 0x88760238:
        return "No clipper object attached to surface object.\0";
    case 0x8876024a:
        return "No DC was ever created for this surface.\0";
    case 0x88760240:
        return "No DirectDraw ROP hardware.\0";
    case 0x88760239:
        return "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.\0";
    case 0x88760242:
        return "Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination.\0";
    case 0x8876023c:
        return "No palette object attached to this surface.\0";
    case 0x8876023d:
        return "No hardware support for 16 or 256 color palettes.\0";
    case 0x88760244:
        return "Returned when an overlay member is called for a non-overlay surface.\0";
    case 0x88760246:
        return "An attempt has been made to flip a surface that is not flippable.\0";
    case 0x88760248:
        return "Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.\0";
    case 0x8876024d:
        return "The surface being used is not a palette-based surface.\0";
    case 0x88760241:
        return "Returned when GetOverlayPosition is called on a hidden overlay.\0";
    case 0x88760236:
        return "Region passed to Clipper::GetClipList is too small.\0";
    case 0x8876024b:
        return "This surface can not be restored because it was created in a different mode.\0";
    case 0x887602bc:
        return "D3DERR_BADMAJORVERSION\0";
    case 0x887602bd:
        return "D3DERR_BADMINORVERSION\0";
    case 0x887602ca:
        return "D3DERR_EXECUTE_LOCKED\0";
    case 0x887602cb:
        return "D3DERR_EXECUTE_NOT_LOCKED\0";
    case 0x887602c6:
        return "D3DERR_EXECUTE_CREATE_FAILED\0";
    case 0x887602c7:
        return "D3DERR_EXECUTE_DESTROY_FAILED\0";
    case 0x887602c8:
        return "D3DERR_EXECUTE_LOCK_FAILED\0";
    case 0x887602c9:
        return "D3DERR_EXECUTE_UNLOCK_FAILED\0";
    case 0x887602cc:
        return "D3DERR_EXECUTE_FAILED\0";
    case 0x887602cd:
        return "D3DERR_EXECUTE_CLIPPED_FAILED\0";
    case 0x887602d0:
        return "D3DERR_TEXTURE_NO_SUPPORT\0";
    case 0x887602d8:
        return "D3DERR_TEXTURE_NOT_LOCKED\0";
    case 0x887602d7:
        return "D3DERR_TEXTURELOCKED\0";
    case 0x887602d1:
        return "D3DERR_TEXTURE_CREATE_FAILED\0";
    case 0x887602d2:
        return "D3DERR_TEXTURE_DESTROY_FAILED\0";
    case 0x887602d3:
        return "D3DERR_TEXTURE_LOCK_FAILED\0";
    case 0x887602d4:
        return "D3DERR_TEXTURE_UNLOCK_FAILED\0";
    case 0x887602d5:
        return "D3DERR_TEXTURE_LOAD_FAILED\0";
    case 0x887602da:
        return "D3DERR_MATRIX_CREATE_FAILED\0";
    case 0x887602db:
        return "D3DERR_MATRIX_DESTROY_FAILED\0";
    case 0x887602dc:
        return "D3DERR_MATRIX_SETDATA_FAILED\0";
    case 0x887602de:
        return "D3DERR_SETVIEWPORTDATA_FAILED\0";
    case 0x887602e4:
        return "D3DERR_MATERIAL_CREATE_FAILED\0";
    case 0x887602e5:
        return "D3DERR_MATERIAL_DESTROY_FAILED\0";
    case 0x887602e6:
        return "D3DERR_MATERIAL_SETDATA_FAILED\0";
    case 0x887602ee:
        return "D3DERR_LIGHT_SET_FAILED\0";
    case 0x8876030d:
        return "D3DRMERR_BADOBJECT\0";
    case 0x8876030e:
        return "D3DRMERR_BADTYPE\0";
    case 0x8876030f:
        return "D3DRMERR_BADALLOC\0";
    case 0x88760310:
        return "D3DRMERR_FACEUSED\0";
    case 0x88760311:
        return "D3DRMERR_NOTFOUND\0";
    case 0x88760312:
        return "D3DRMERR_NOTDONEYET\0";
    case 0x88760313:
        return "The file was not found.\0";
    case 0x88760314:
        return "D3DRMERR_BADFILE\0";
    case 0x88760315:
        return "D3DRMERR_BADDEVICE\0";
    case 0x88760316:
        return "D3DRMERR_BADVALUE\0";
    case 0x88760317:
        return "D3DRMERR_BADMAJORVERSION\0";
    case 0x88760318:
        return "D3DRMERR_BADMINORVERSION\0";
    case 0x88760319:
        return "D3DRMERR_UNABLETOEXECUTE\0";
    case 0x00000000:
        return "No error.\0";
    default:
        return "Unrecognized error value.\0";
    }
}
