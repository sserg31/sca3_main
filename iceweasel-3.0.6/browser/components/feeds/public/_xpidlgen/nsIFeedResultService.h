/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsIFeedResultService.idl
 */

#ifndef __gen_nsIFeedResultService_h__
#define __gen_nsIFeedResultService_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIURI; /* forward declaration */

class nsIRequest; /* forward declaration */

class nsIFeedResult; /* forward declaration */


/* starting interface:    nsIFeedResultService */
#define NS_IFEEDRESULTSERVICE_IID_STR "950a829e-c20e-4dc3-b447-f8b753ae54da"

#define NS_IFEEDRESULTSERVICE_IID \
  {0x950a829e, 0xc20e, 0x4dc3, \
    { 0xb4, 0x47, 0xf8, 0xb7, 0x53, 0xae, 0x54, 0xda }}

/**
 * nsIFeedResultService provides a globally-accessible object for retrieving
 * the results of feed processing.
 */
class NS_NO_VTABLE NS_SCRIPTABLE nsIFeedResultService : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IFEEDRESULTSERVICE_IID)

  /**
   * When set to true, forces the preview page to be displayed, regardless
   * of the user's preferences.
   */
  /* attribute boolean forcePreviewPage; */
  NS_SCRIPTABLE NS_IMETHOD GetForcePreviewPage(PRBool *aForcePreviewPage) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetForcePreviewPage(PRBool aForcePreviewPage) = 0;

  /**
   * Adds a URI to the user's specified external feed handler, or live 
   * bookmarks. 
   * @param   uri
   *          The uri of the feed to add.
   * @param   title
   *          The title of the feed to add.
   * @param   subtitle
   *          The subtitle of the feed to add.
   * @param   feedType
   *          The nsIFeed type of the feed.  See nsIFeed.idl
   */
  /* void addToClientReader (in AUTF8String uri, in AString title, in AString subtitle, in unsigned long feedType); */
  NS_SCRIPTABLE NS_IMETHOD AddToClientReader(const nsACString & uri, const nsAString & title, const nsAString & subtitle, PRUint32 feedType) = 0;

  /**
   * Registers a Feed Result object with a globally accessible service
   * so that it can be accessed by a singleton method outside the usual
   * flow of control in document loading.
   *
   * @param   feedResult
   *          An object implementing nsIFeedResult representing the feed.
   */
  /* void addFeedResult (in nsIFeedResult feedResult); */
  NS_SCRIPTABLE NS_IMETHOD AddFeedResult(nsIFeedResult *feedResult) = 0;

  /**
   * Gets a Feed Handler object registered using addFeedResult.
   *
   * @param   uri
   *          The URI of the feed a handler is being requested for
   */
  /* nsIFeedResult getFeedResult (in nsIURI uri); */
  NS_SCRIPTABLE NS_IMETHOD GetFeedResult(nsIURI *uri, nsIFeedResult **_retval) = 0;

  /**
   * Unregisters a Feed Handler object registered using addFeedResult.
   * @param   uri
   *          The feed URI the handler was registered under. This must be
   *          the same *instance* the feed was registered under.
   */
  /* void removeFeedResult (in nsIURI uri); */
  NS_SCRIPTABLE NS_IMETHOD RemoveFeedResult(nsIURI *uri) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIFeedResultService, NS_IFEEDRESULTSERVICE_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIFEEDRESULTSERVICE \
  NS_SCRIPTABLE NS_IMETHOD GetForcePreviewPage(PRBool *aForcePreviewPage); \
  NS_SCRIPTABLE NS_IMETHOD SetForcePreviewPage(PRBool aForcePreviewPage); \
  NS_SCRIPTABLE NS_IMETHOD AddToClientReader(const nsACString & uri, const nsAString & title, const nsAString & subtitle, PRUint32 feedType); \
  NS_SCRIPTABLE NS_IMETHOD AddFeedResult(nsIFeedResult *feedResult); \
  NS_SCRIPTABLE NS_IMETHOD GetFeedResult(nsIURI *uri, nsIFeedResult **_retval); \
  NS_SCRIPTABLE NS_IMETHOD RemoveFeedResult(nsIURI *uri); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIFEEDRESULTSERVICE(_to) \
  NS_SCRIPTABLE NS_IMETHOD GetForcePreviewPage(PRBool *aForcePreviewPage) { return _to GetForcePreviewPage(aForcePreviewPage); } \
  NS_SCRIPTABLE NS_IMETHOD SetForcePreviewPage(PRBool aForcePreviewPage) { return _to SetForcePreviewPage(aForcePreviewPage); } \
  NS_SCRIPTABLE NS_IMETHOD AddToClientReader(const nsACString & uri, const nsAString & title, const nsAString & subtitle, PRUint32 feedType) { return _to AddToClientReader(uri, title, subtitle, feedType); } \
  NS_SCRIPTABLE NS_IMETHOD AddFeedResult(nsIFeedResult *feedResult) { return _to AddFeedResult(feedResult); } \
  NS_SCRIPTABLE NS_IMETHOD GetFeedResult(nsIURI *uri, nsIFeedResult **_retval) { return _to GetFeedResult(uri, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD RemoveFeedResult(nsIURI *uri) { return _to RemoveFeedResult(uri); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIFEEDRESULTSERVICE(_to) \
  NS_SCRIPTABLE NS_IMETHOD GetForcePreviewPage(PRBool *aForcePreviewPage) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetForcePreviewPage(aForcePreviewPage); } \
  NS_SCRIPTABLE NS_IMETHOD SetForcePreviewPage(PRBool aForcePreviewPage) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetForcePreviewPage(aForcePreviewPage); } \
  NS_SCRIPTABLE NS_IMETHOD AddToClientReader(const nsACString & uri, const nsAString & title, const nsAString & subtitle, PRUint32 feedType) { return !_to ? NS_ERROR_NULL_POINTER : _to->AddToClientReader(uri, title, subtitle, feedType); } \
  NS_SCRIPTABLE NS_IMETHOD AddFeedResult(nsIFeedResult *feedResult) { return !_to ? NS_ERROR_NULL_POINTER : _to->AddFeedResult(feedResult); } \
  NS_SCRIPTABLE NS_IMETHOD GetFeedResult(nsIURI *uri, nsIFeedResult **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFeedResult(uri, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD RemoveFeedResult(nsIURI *uri) { return !_to ? NS_ERROR_NULL_POINTER : _to->RemoveFeedResult(uri); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsFeedResultService : public nsIFeedResultService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIFEEDRESULTSERVICE

  nsFeedResultService();

private:
  ~nsFeedResultService();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsFeedResultService, nsIFeedResultService)

nsFeedResultService::nsFeedResultService()
{
  /* member initializers and constructor code */
}

nsFeedResultService::~nsFeedResultService()
{
  /* destructor code */
}

/* attribute boolean forcePreviewPage; */
NS_IMETHODIMP nsFeedResultService::GetForcePreviewPage(PRBool *aForcePreviewPage)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsFeedResultService::SetForcePreviewPage(PRBool aForcePreviewPage)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void addToClientReader (in AUTF8String uri, in AString title, in AString subtitle, in unsigned long feedType); */
NS_IMETHODIMP nsFeedResultService::AddToClientReader(const nsACString & uri, const nsAString & title, const nsAString & subtitle, PRUint32 feedType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void addFeedResult (in nsIFeedResult feedResult); */
NS_IMETHODIMP nsFeedResultService::AddFeedResult(nsIFeedResult *feedResult)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIFeedResult getFeedResult (in nsIURI uri); */
NS_IMETHODIMP nsFeedResultService::GetFeedResult(nsIURI *uri, nsIFeedResult **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void removeFeedResult (in nsIURI uri); */
NS_IMETHODIMP nsFeedResultService::RemoveFeedResult(nsIURI *uri)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIFeedResultService_h__ */
