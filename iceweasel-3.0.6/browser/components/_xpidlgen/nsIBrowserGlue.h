/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsIBrowserGlue.idl
 */

#ifndef __gen_nsIBrowserGlue_h__
#define __gen_nsIBrowserGlue_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIDOMWindow; /* forward declaration */


/* starting interface:    nsIBrowserGlue */
#define NS_IBROWSERGLUE_IID_STR "01639c88-c0eb-4d75-9ee1-f325c1e04215"

#define NS_IBROWSERGLUE_IID \
  {0x01639c88, 0xc0eb, 0x4d75, \
    { 0x9e, 0xe1, 0xf3, 0x25, 0xc1, 0xe0, 0x42, 0x15 }}

/**
 * nsIBrowserGlue is a dirty and rather fluid interface to host shared utility 
 * methods used by browser UI code, but which are not local to a browser window.
 * The component implementing this interface is meant to be a singleton
 * (service) and should progressively replace some of the shared "glue" code 
 * scattered in browser/base/content (e.g. bits of utilOverlay.js, 
 * contentAreaUtils.js, globalOverlay.js, browser.js), avoiding dynamic 
 * inclusion and initialization of a ton of JS code for *each* window.
 * Dued to its nature and origin, this interface won't probably be the most
 * elegant or stable in the mozilla codebase, but its aim is rather pragmatic:
 * 1) reducing the performance overhead which affects browser window load;
 * 2) allow global hooks (e.g. startup and shutdown observers) which survive
 * browser windows to accomplish browser-related activities, such as shutdown
 * sanitization (see bug #284086)
 *
 */
class NS_NO_VTABLE NS_SCRIPTABLE nsIBrowserGlue : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IBROWSERGLUE_IID)

  /** 
   * Deletes privacy sensitive data according to user preferences
   *
   * @param aParentWindow an optionally null window which is the parent of the 
   *        sanitization dialog (if it has to be shown per user preferences)
   *
   */
  /* void sanitize (in nsIDOMWindow aParentWindow); */
  NS_SCRIPTABLE NS_IMETHOD Sanitize(nsIDOMWindow *aParentWindow) = 0;

  /**
   * Add Smart Bookmarks special queries to bookmarks menu and toolbar folder.
   */
  /* void ensurePlacesDefaultQueriesInitialized (); */
  NS_SCRIPTABLE NS_IMETHOD EnsurePlacesDefaultQueriesInitialized(void) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIBrowserGlue, NS_IBROWSERGLUE_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIBROWSERGLUE \
  NS_SCRIPTABLE NS_IMETHOD Sanitize(nsIDOMWindow *aParentWindow); \
  NS_SCRIPTABLE NS_IMETHOD EnsurePlacesDefaultQueriesInitialized(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIBROWSERGLUE(_to) \
  NS_SCRIPTABLE NS_IMETHOD Sanitize(nsIDOMWindow *aParentWindow) { return _to Sanitize(aParentWindow); } \
  NS_SCRIPTABLE NS_IMETHOD EnsurePlacesDefaultQueriesInitialized(void) { return _to EnsurePlacesDefaultQueriesInitialized(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIBROWSERGLUE(_to) \
  NS_SCRIPTABLE NS_IMETHOD Sanitize(nsIDOMWindow *aParentWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->Sanitize(aParentWindow); } \
  NS_SCRIPTABLE NS_IMETHOD EnsurePlacesDefaultQueriesInitialized(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->EnsurePlacesDefaultQueriesInitialized(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsBrowserGlue : public nsIBrowserGlue
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIBROWSERGLUE

  nsBrowserGlue();

private:
  ~nsBrowserGlue();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsBrowserGlue, nsIBrowserGlue)

nsBrowserGlue::nsBrowserGlue()
{
  /* member initializers and constructor code */
}

nsBrowserGlue::~nsBrowserGlue()
{
  /* destructor code */
}

/* void sanitize (in nsIDOMWindow aParentWindow); */
NS_IMETHODIMP nsBrowserGlue::Sanitize(nsIDOMWindow *aParentWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void ensurePlacesDefaultQueriesInitialized (); */
NS_IMETHODIMP nsBrowserGlue::EnsurePlacesDefaultQueriesInitialized()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIBrowserGlue_h__ */
