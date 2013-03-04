/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsISessionStartup.idl
 */

#ifndef __gen_nsISessionStartup_h__
#define __gen_nsISessionStartup_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    nsISessionStartup */
#define NS_ISESSIONSTARTUP_IID_STR "c0b185e7-0d21-46ac-8eee-7b5065ee7ecd"

#define NS_ISESSIONSTARTUP_IID \
  {0xc0b185e7, 0x0d21, 0x46ac, \
    { 0x8e, 0xee, 0x7b, 0x50, 0x65, 0xee, 0x7e, 0xcd }}

/**
 * nsISessionStore keeps track of the current browsing state - i.e.
 * tab history, cookies, scroll state, form data, POSTDATA and window features
 * - and allows to restore everything into one window.
 */
class NS_NO_VTABLE NS_SCRIPTABLE nsISessionStartup : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_ISESSIONSTARTUP_IID)

  /* readonly attribute AString state; */
  NS_SCRIPTABLE NS_IMETHOD GetState(nsAString & aState) = 0;

  /**
   * Determine if session should be restored 
   */
  /* boolean doRestore (); */
  NS_SCRIPTABLE NS_IMETHOD DoRestore(PRBool *_retval) = 0;

  /**
   * What type of session we're restoring. If we have a session, we're
   * either restoring state from a crash or restoring state that the user
   * requested we save on shutdown.
   */
  enum { NO_SESSION = 0U };

  enum { RECOVER_SESSION = 1U };

  enum { RESUME_SESSION = 2U };

  /* readonly attribute unsigned long sessionType; */
  NS_SCRIPTABLE NS_IMETHOD GetSessionType(PRUint32 *aSessionType) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsISessionStartup, NS_ISESSIONSTARTUP_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISESSIONSTARTUP \
  NS_SCRIPTABLE NS_IMETHOD GetState(nsAString & aState); \
  NS_SCRIPTABLE NS_IMETHOD DoRestore(PRBool *_retval); \
  NS_SCRIPTABLE NS_IMETHOD GetSessionType(PRUint32 *aSessionType); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISESSIONSTARTUP(_to) \
  NS_SCRIPTABLE NS_IMETHOD GetState(nsAString & aState) { return _to GetState(aState); } \
  NS_SCRIPTABLE NS_IMETHOD DoRestore(PRBool *_retval) { return _to DoRestore(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetSessionType(PRUint32 *aSessionType) { return _to GetSessionType(aSessionType); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISESSIONSTARTUP(_to) \
  NS_SCRIPTABLE NS_IMETHOD GetState(nsAString & aState) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetState(aState); } \
  NS_SCRIPTABLE NS_IMETHOD DoRestore(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->DoRestore(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetSessionType(PRUint32 *aSessionType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSessionType(aSessionType); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsSessionStartup : public nsISessionStartup
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISESSIONSTARTUP

  nsSessionStartup();

private:
  ~nsSessionStartup();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsSessionStartup, nsISessionStartup)

nsSessionStartup::nsSessionStartup()
{
  /* member initializers and constructor code */
}

nsSessionStartup::~nsSessionStartup()
{
  /* destructor code */
}

/* readonly attribute AString state; */
NS_IMETHODIMP nsSessionStartup::GetState(nsAString & aState)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean doRestore (); */
NS_IMETHODIMP nsSessionStartup::DoRestore(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute unsigned long sessionType; */
NS_IMETHODIMP nsSessionStartup::GetSessionType(PRUint32 *aSessionType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsISessionStartup_h__ */
