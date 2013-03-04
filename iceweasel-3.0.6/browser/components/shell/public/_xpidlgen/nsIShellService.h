/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsIShellService.idl
 */

#ifndef __gen_nsIShellService_h__
#define __gen_nsIShellService_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIDOMElement; /* forward declaration */

class nsILocalFile; /* forward declaration */


/* starting interface:    nsIShellService */
#define NS_ISHELLSERVICE_IID_STR "bb477da4-dddf-4106-a562-f06c85c7f9a8"

#define NS_ISHELLSERVICE_IID \
  {0xbb477da4, 0xdddf, 0x4106, \
    { 0xa5, 0x62, 0xf0, 0x6c, 0x85, 0xc7, 0xf9, 0xa8 }}

class NS_NO_VTABLE NS_SCRIPTABLE nsIShellService : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_ISHELLSERVICE_IID)

  /**
   * Determines whether or not Firefox is the "Default Browser."
   * This is simply whether or not Firefox is registered to handle 
   * http links. 
   * 
   * @param aStartupCheck true if this is the check being performed
   *                      by the first browser window at startup, 
   *                      false otherwise. 
   */
  /* boolean isDefaultBrowser (in boolean aStartupCheck); */
  NS_SCRIPTABLE NS_IMETHOD IsDefaultBrowser(PRBool aStartupCheck, PRBool *_retval) = 0;

  /**
   * Registers Firefox as the "Default Browser."
   *
   * @param aClaimAllTypes Register Firefox as the handler for 
   *                       additional protocols (ftp, chrome etc)
   *                       and web documents (.html, .xhtml etc).
   * @param aForAllUsers   Whether or not Firefox should attempt
   *                       to become the default browser for all
   *                       users on a multi-user system. 
   */
  /* void setDefaultBrowser (in boolean aClaimAllTypes, in boolean aForAllUsers); */
  NS_SCRIPTABLE NS_IMETHOD SetDefaultBrowser(PRBool aClaimAllTypes, PRBool aForAllUsers) = 0;

  /** 
   * Used to determine whether or not to show a "Set Default Browser"
   * query dialog. This attribute is true if the application is starting
   * up and "browser.shell.checkDefaultBrowser" is true, otherwise it
   * is false.
   */
  /* attribute boolean shouldCheckDefaultBrowser; */
  NS_SCRIPTABLE NS_IMETHOD GetShouldCheckDefaultBrowser(PRBool *aShouldCheckDefaultBrowser) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetShouldCheckDefaultBrowser(PRBool aShouldCheckDefaultBrowser) = 0;

  /** 
   * Flags for positioning/sizing of the Desktop Background image.
   */
  enum { BACKGROUND_TILE = 1 };

  enum { BACKGROUND_STRETCH = 2 };

  enum { BACKGROUND_CENTER = 3 };

  enum { BACKGROUND_FILL = 4 };

  /**
     * Sets the desktop background image using either the HTML <IMG> 
     * element supplied or the background image of the element supplied.
     *
     * @param aImageElement Either a HTML <IMG> element or an element with
     *                      a background image from which to source the
     *                      background image. 
     * @param aPosition     How to place the image on the desktop
     */
  /* void setDesktopBackground (in nsIDOMElement aElement, in long aPosition); */
  NS_SCRIPTABLE NS_IMETHOD SetDesktopBackground(nsIDOMElement *aElement, PRInt32 aPosition) = 0;

  /**
   * Constants identifying applications that can be opened with
   * openApplication.
   */
  enum { APPLICATION_MAIL = 0 };

  enum { APPLICATION_NEWS = 1 };

  /**
   * Opens the application specified. If more than one application of the
   * given type is available on the system, the default or "preferred"
   * application is used. 
   */
  /* void openApplication (in long aApplication); */
  NS_SCRIPTABLE NS_IMETHOD OpenApplication(PRInt32 aApplication) = 0;

  /** 
   * The desktop background color, visible when no background image is 
   * used, or if the background image is centered and does not fill the 
   * entire screen. A rgb value, where (r << 16 | g << 8 | b)
   */
  /* attribute unsigned long desktopBackgroundColor; */
  NS_SCRIPTABLE NS_IMETHOD GetDesktopBackgroundColor(PRUint32 *aDesktopBackgroundColor) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetDesktopBackgroundColor(PRUint32 aDesktopBackgroundColor) = 0;

  /**
   * Opens an application with a specific URI to load.
   * @param   application
   *          The application file (or bundle directory, on OS X)
   * @param   uri
   *          The uri to be loaded by the application
   */
  /* void openApplicationWithURI (in nsILocalFile aApplication, in ACString aURI); */
  NS_SCRIPTABLE NS_IMETHOD OpenApplicationWithURI(nsILocalFile *aApplication, const nsACString & aURI) = 0;

  /**
   * The default system handler for web feeds
   */
  /* readonly attribute nsILocalFile defaultFeedReader; */
  NS_SCRIPTABLE NS_IMETHOD GetDefaultFeedReader(nsILocalFile * *aDefaultFeedReader) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIShellService, NS_ISHELLSERVICE_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISHELLSERVICE \
  NS_SCRIPTABLE NS_IMETHOD IsDefaultBrowser(PRBool aStartupCheck, PRBool *_retval); \
  NS_SCRIPTABLE NS_IMETHOD SetDefaultBrowser(PRBool aClaimAllTypes, PRBool aForAllUsers); \
  NS_SCRIPTABLE NS_IMETHOD GetShouldCheckDefaultBrowser(PRBool *aShouldCheckDefaultBrowser); \
  NS_SCRIPTABLE NS_IMETHOD SetShouldCheckDefaultBrowser(PRBool aShouldCheckDefaultBrowser); \
  NS_SCRIPTABLE NS_IMETHOD SetDesktopBackground(nsIDOMElement *aElement, PRInt32 aPosition); \
  NS_SCRIPTABLE NS_IMETHOD OpenApplication(PRInt32 aApplication); \
  NS_SCRIPTABLE NS_IMETHOD GetDesktopBackgroundColor(PRUint32 *aDesktopBackgroundColor); \
  NS_SCRIPTABLE NS_IMETHOD SetDesktopBackgroundColor(PRUint32 aDesktopBackgroundColor); \
  NS_SCRIPTABLE NS_IMETHOD OpenApplicationWithURI(nsILocalFile *aApplication, const nsACString & aURI); \
  NS_SCRIPTABLE NS_IMETHOD GetDefaultFeedReader(nsILocalFile * *aDefaultFeedReader); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISHELLSERVICE(_to) \
  NS_SCRIPTABLE NS_IMETHOD IsDefaultBrowser(PRBool aStartupCheck, PRBool *_retval) { return _to IsDefaultBrowser(aStartupCheck, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetDefaultBrowser(PRBool aClaimAllTypes, PRBool aForAllUsers) { return _to SetDefaultBrowser(aClaimAllTypes, aForAllUsers); } \
  NS_SCRIPTABLE NS_IMETHOD GetShouldCheckDefaultBrowser(PRBool *aShouldCheckDefaultBrowser) { return _to GetShouldCheckDefaultBrowser(aShouldCheckDefaultBrowser); } \
  NS_SCRIPTABLE NS_IMETHOD SetShouldCheckDefaultBrowser(PRBool aShouldCheckDefaultBrowser) { return _to SetShouldCheckDefaultBrowser(aShouldCheckDefaultBrowser); } \
  NS_SCRIPTABLE NS_IMETHOD SetDesktopBackground(nsIDOMElement *aElement, PRInt32 aPosition) { return _to SetDesktopBackground(aElement, aPosition); } \
  NS_SCRIPTABLE NS_IMETHOD OpenApplication(PRInt32 aApplication) { return _to OpenApplication(aApplication); } \
  NS_SCRIPTABLE NS_IMETHOD GetDesktopBackgroundColor(PRUint32 *aDesktopBackgroundColor) { return _to GetDesktopBackgroundColor(aDesktopBackgroundColor); } \
  NS_SCRIPTABLE NS_IMETHOD SetDesktopBackgroundColor(PRUint32 aDesktopBackgroundColor) { return _to SetDesktopBackgroundColor(aDesktopBackgroundColor); } \
  NS_SCRIPTABLE NS_IMETHOD OpenApplicationWithURI(nsILocalFile *aApplication, const nsACString & aURI) { return _to OpenApplicationWithURI(aApplication, aURI); } \
  NS_SCRIPTABLE NS_IMETHOD GetDefaultFeedReader(nsILocalFile * *aDefaultFeedReader) { return _to GetDefaultFeedReader(aDefaultFeedReader); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISHELLSERVICE(_to) \
  NS_SCRIPTABLE NS_IMETHOD IsDefaultBrowser(PRBool aStartupCheck, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsDefaultBrowser(aStartupCheck, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetDefaultBrowser(PRBool aClaimAllTypes, PRBool aForAllUsers) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDefaultBrowser(aClaimAllTypes, aForAllUsers); } \
  NS_SCRIPTABLE NS_IMETHOD GetShouldCheckDefaultBrowser(PRBool *aShouldCheckDefaultBrowser) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetShouldCheckDefaultBrowser(aShouldCheckDefaultBrowser); } \
  NS_SCRIPTABLE NS_IMETHOD SetShouldCheckDefaultBrowser(PRBool aShouldCheckDefaultBrowser) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetShouldCheckDefaultBrowser(aShouldCheckDefaultBrowser); } \
  NS_SCRIPTABLE NS_IMETHOD SetDesktopBackground(nsIDOMElement *aElement, PRInt32 aPosition) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDesktopBackground(aElement, aPosition); } \
  NS_SCRIPTABLE NS_IMETHOD OpenApplication(PRInt32 aApplication) { return !_to ? NS_ERROR_NULL_POINTER : _to->OpenApplication(aApplication); } \
  NS_SCRIPTABLE NS_IMETHOD GetDesktopBackgroundColor(PRUint32 *aDesktopBackgroundColor) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDesktopBackgroundColor(aDesktopBackgroundColor); } \
  NS_SCRIPTABLE NS_IMETHOD SetDesktopBackgroundColor(PRUint32 aDesktopBackgroundColor) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDesktopBackgroundColor(aDesktopBackgroundColor); } \
  NS_SCRIPTABLE NS_IMETHOD OpenApplicationWithURI(nsILocalFile *aApplication, const nsACString & aURI) { return !_to ? NS_ERROR_NULL_POINTER : _to->OpenApplicationWithURI(aApplication, aURI); } \
  NS_SCRIPTABLE NS_IMETHOD GetDefaultFeedReader(nsILocalFile * *aDefaultFeedReader) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDefaultFeedReader(aDefaultFeedReader); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsShellService : public nsIShellService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISHELLSERVICE

  nsShellService();

private:
  ~nsShellService();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsShellService, nsIShellService)

nsShellService::nsShellService()
{
  /* member initializers and constructor code */
}

nsShellService::~nsShellService()
{
  /* destructor code */
}

/* boolean isDefaultBrowser (in boolean aStartupCheck); */
NS_IMETHODIMP nsShellService::IsDefaultBrowser(PRBool aStartupCheck, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setDefaultBrowser (in boolean aClaimAllTypes, in boolean aForAllUsers); */
NS_IMETHODIMP nsShellService::SetDefaultBrowser(PRBool aClaimAllTypes, PRBool aForAllUsers)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean shouldCheckDefaultBrowser; */
NS_IMETHODIMP nsShellService::GetShouldCheckDefaultBrowser(PRBool *aShouldCheckDefaultBrowser)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsShellService::SetShouldCheckDefaultBrowser(PRBool aShouldCheckDefaultBrowser)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setDesktopBackground (in nsIDOMElement aElement, in long aPosition); */
NS_IMETHODIMP nsShellService::SetDesktopBackground(nsIDOMElement *aElement, PRInt32 aPosition)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void openApplication (in long aApplication); */
NS_IMETHODIMP nsShellService::OpenApplication(PRInt32 aApplication)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute unsigned long desktopBackgroundColor; */
NS_IMETHODIMP nsShellService::GetDesktopBackgroundColor(PRUint32 *aDesktopBackgroundColor)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsShellService::SetDesktopBackgroundColor(PRUint32 aDesktopBackgroundColor)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void openApplicationWithURI (in nsILocalFile aApplication, in ACString aURI); */
NS_IMETHODIMP nsShellService::OpenApplicationWithURI(nsILocalFile *aApplication, const nsACString & aURI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsILocalFile defaultFeedReader; */
NS_IMETHODIMP nsShellService::GetDefaultFeedReader(nsILocalFile * *aDefaultFeedReader)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIShellService_h__ */
