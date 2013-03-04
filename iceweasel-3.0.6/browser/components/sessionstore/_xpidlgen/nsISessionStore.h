/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsISessionStore.idl
 */

#ifndef __gen_nsISessionStore_h__
#define __gen_nsISessionStore_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIDOMWindow; /* forward declaration */

class nsIDOMNode; /* forward declaration */


/* starting interface:    nsISessionStore */
#define NS_ISESSIONSTORE_IID_STR "58d17e12-a80f-11dc-8314-0800200c9a66"

#define NS_ISESSIONSTORE_IID \
  {0x58d17e12, 0xa80f, 0x11dc, \
    { 0x83, 0x14, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 }}

/**
 * nsISessionStore keeps track of the current browsing state - i.e.
 * tab history, cookies, scroll state, form data, POSTDATA and window features
 * - and allows to restore everything into one window.
 */
class NS_NO_VTABLE NS_SCRIPTABLE nsISessionStore : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_ISESSIONSTORE_IID)

  /**
   * Initialize the service
   */
  /* void init (in nsIDOMWindow aWindow); */
  NS_SCRIPTABLE NS_IMETHOD Init(nsIDOMWindow *aWindow) = 0;

  /**
   * Get the current browsing state.
   * @return a JSON string representing the session state.
   */
  /* AString getBrowserState (); */
  NS_SCRIPTABLE NS_IMETHOD GetBrowserState(nsAString & _retval) = 0;

  /**
   * Set the browsing state.
   * This will immediately restore the state of the whole application to the state
   * passed in, *replacing* the current session.
   *
   * @param aState is a JSON string representing the session state.
   */
  /* void setBrowserState (in AString aState); */
  NS_SCRIPTABLE NS_IMETHOD SetBrowserState(const nsAString & aState) = 0;

  /**
   * @param aWindow is the window whose state is to be returned.
   * 
   * @return a JSON string representing a session state with only one window.
   */
  /* AString getWindowState (in nsIDOMWindow aWindow); */
  NS_SCRIPTABLE NS_IMETHOD GetWindowState(nsIDOMWindow *aWindow, nsAString & _retval) = 0;

  /**
   * @param aWindow    is the window whose state is to be set.
   * @param aState     is a JSON string representing a session state.
   * @param aOverwrite boolean overwrite existing tabs
   */
  /* void setWindowState (in nsIDOMWindow aWindow, in AString aState, in boolean aOverwrite); */
  NS_SCRIPTABLE NS_IMETHOD SetWindowState(nsIDOMWindow *aWindow, const nsAString & aState, PRBool aOverwrite) = 0;

  /**
   * @param aTab is the tab whose state is to be returned.
   * 
   * @return a JSON string representing the state of the tab
   *         (note: doesn't contain cookies - if you need them, use getWindowState instead).
   */
  /* AString getTabState (in nsIDOMNode aTab); */
  NS_SCRIPTABLE NS_IMETHOD GetTabState(nsIDOMNode *aTab, nsAString & _retval) = 0;

  /**
   * @param aTab   is the tab whose state is to be set.
   * @param aState is a JSON string representing a session state.
   */
  /* void setTabState (in nsIDOMNode aTab, in AString aState); */
  NS_SCRIPTABLE NS_IMETHOD SetTabState(nsIDOMNode *aTab, const nsAString & aState) = 0;

  /**
   * Duplicates a given tab as thoroughly as possible.
   *
   * @param aWindow is the window into which the tab will be duplicated.
   * @param aTab    is the tab to duplicate (can be from a different window).
   * @return a reference to the newly created tab.
   */
  /* nsIDOMNode duplicateTab (in nsIDOMWindow aWindow, in nsIDOMNode aTab); */
  NS_SCRIPTABLE NS_IMETHOD DuplicateTab(nsIDOMWindow *aWindow, nsIDOMNode *aTab, nsIDOMNode **_retval) = 0;

  /**
   * Get the number of restore-able tabs for a window
   */
  /* unsigned long getClosedTabCount (in nsIDOMWindow aWindow); */
  NS_SCRIPTABLE NS_IMETHOD GetClosedTabCount(nsIDOMWindow *aWindow, PRUint32 *_retval) = 0;

  /**
   * Get closed tab data
   * @return a JSON string representing the list of closed tabs.
   */
  /* AString getClosedTabData (in nsIDOMWindow aWindow); */
  NS_SCRIPTABLE NS_IMETHOD GetClosedTabData(nsIDOMWindow *aWindow, nsAString & _retval) = 0;

  /**
   * @param aWindow
   *          The window to reopen a closed tab in.
   * @param aIndex
   *          Indicates the window to be restored (FIFO ordered).
   */
  /* void undoCloseTab (in nsIDOMWindow aWindow, in unsigned long aIndex); */
  NS_SCRIPTABLE NS_IMETHOD UndoCloseTab(nsIDOMWindow *aWindow, PRUint32 aIndex) = 0;

  /**
   * @param aWindow is the window to get the value for.
   * @param aKey    is the value's name.
   * 
   * @return A string value or "" if none is set.
   */
  /* AString getWindowValue (in nsIDOMWindow aWindow, in AString aKey); */
  NS_SCRIPTABLE NS_IMETHOD GetWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey, nsAString & _retval) = 0;

  /**
   * @param aWindow      is the window to set the value for.
   * @param aKey         is the value's name.
   * @param aStringValue is the value itself (use toSource/eval before setting JS objects).
   */
  /* void setWindowValue (in nsIDOMWindow aWindow, in AString aKey, in AString aStringValue); */
  NS_SCRIPTABLE NS_IMETHOD SetWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey, const nsAString & aStringValue) = 0;

  /**
   * @param aWindow is the window to get the value for.
   * @param aKey    is the value's name.
   */
  /* void deleteWindowValue (in nsIDOMWindow aWindow, in AString aKey); */
  NS_SCRIPTABLE NS_IMETHOD DeleteWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey) = 0;

  /**
   * @param aTab is the tab to get the value for.
   * @param aKey is the value's name.
   * 
   * @return A string value or "" if none is set.
   */
  /* AString getTabValue (in nsIDOMNode aTab, in AString aKey); */
  NS_SCRIPTABLE NS_IMETHOD GetTabValue(nsIDOMNode *aTab, const nsAString & aKey, nsAString & _retval) = 0;

  /**
   * @param aTab         is the tab to set the value for.
   * @param aKey         is the value's name.
   * @param aStringValue is the value itself (use toSource/eval before setting JS objects).
   */
  /* void setTabValue (in nsIDOMNode aTab, in AString aKey, in AString aStringValue); */
  NS_SCRIPTABLE NS_IMETHOD SetTabValue(nsIDOMNode *aTab, const nsAString & aKey, const nsAString & aStringValue) = 0;

  /**
   * @param aTab is the tab to get the value for.
   * @param aKey is the value's name.
   */
  /* void deleteTabValue (in nsIDOMNode aTab, in AString aKey); */
  NS_SCRIPTABLE NS_IMETHOD DeleteTabValue(nsIDOMNode *aTab, const nsAString & aKey) = 0;

  /**
   * @param aName is the name of the attribute to save/restore for all xul:tabs.
   */
  /* void persistTabAttribute (in AString aName); */
  NS_SCRIPTABLE NS_IMETHOD PersistTabAttribute(const nsAString & aName) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsISessionStore, NS_ISESSIONSTORE_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISESSIONSTORE \
  NS_SCRIPTABLE NS_IMETHOD Init(nsIDOMWindow *aWindow); \
  NS_SCRIPTABLE NS_IMETHOD GetBrowserState(nsAString & _retval); \
  NS_SCRIPTABLE NS_IMETHOD SetBrowserState(const nsAString & aState); \
  NS_SCRIPTABLE NS_IMETHOD GetWindowState(nsIDOMWindow *aWindow, nsAString & _retval); \
  NS_SCRIPTABLE NS_IMETHOD SetWindowState(nsIDOMWindow *aWindow, const nsAString & aState, PRBool aOverwrite); \
  NS_SCRIPTABLE NS_IMETHOD GetTabState(nsIDOMNode *aTab, nsAString & _retval); \
  NS_SCRIPTABLE NS_IMETHOD SetTabState(nsIDOMNode *aTab, const nsAString & aState); \
  NS_SCRIPTABLE NS_IMETHOD DuplicateTab(nsIDOMWindow *aWindow, nsIDOMNode *aTab, nsIDOMNode **_retval); \
  NS_SCRIPTABLE NS_IMETHOD GetClosedTabCount(nsIDOMWindow *aWindow, PRUint32 *_retval); \
  NS_SCRIPTABLE NS_IMETHOD GetClosedTabData(nsIDOMWindow *aWindow, nsAString & _retval); \
  NS_SCRIPTABLE NS_IMETHOD UndoCloseTab(nsIDOMWindow *aWindow, PRUint32 aIndex); \
  NS_SCRIPTABLE NS_IMETHOD GetWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey, nsAString & _retval); \
  NS_SCRIPTABLE NS_IMETHOD SetWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey, const nsAString & aStringValue); \
  NS_SCRIPTABLE NS_IMETHOD DeleteWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey); \
  NS_SCRIPTABLE NS_IMETHOD GetTabValue(nsIDOMNode *aTab, const nsAString & aKey, nsAString & _retval); \
  NS_SCRIPTABLE NS_IMETHOD SetTabValue(nsIDOMNode *aTab, const nsAString & aKey, const nsAString & aStringValue); \
  NS_SCRIPTABLE NS_IMETHOD DeleteTabValue(nsIDOMNode *aTab, const nsAString & aKey); \
  NS_SCRIPTABLE NS_IMETHOD PersistTabAttribute(const nsAString & aName); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISESSIONSTORE(_to) \
  NS_SCRIPTABLE NS_IMETHOD Init(nsIDOMWindow *aWindow) { return _to Init(aWindow); } \
  NS_SCRIPTABLE NS_IMETHOD GetBrowserState(nsAString & _retval) { return _to GetBrowserState(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetBrowserState(const nsAString & aState) { return _to SetBrowserState(aState); } \
  NS_SCRIPTABLE NS_IMETHOD GetWindowState(nsIDOMWindow *aWindow, nsAString & _retval) { return _to GetWindowState(aWindow, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetWindowState(nsIDOMWindow *aWindow, const nsAString & aState, PRBool aOverwrite) { return _to SetWindowState(aWindow, aState, aOverwrite); } \
  NS_SCRIPTABLE NS_IMETHOD GetTabState(nsIDOMNode *aTab, nsAString & _retval) { return _to GetTabState(aTab, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetTabState(nsIDOMNode *aTab, const nsAString & aState) { return _to SetTabState(aTab, aState); } \
  NS_SCRIPTABLE NS_IMETHOD DuplicateTab(nsIDOMWindow *aWindow, nsIDOMNode *aTab, nsIDOMNode **_retval) { return _to DuplicateTab(aWindow, aTab, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetClosedTabCount(nsIDOMWindow *aWindow, PRUint32 *_retval) { return _to GetClosedTabCount(aWindow, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetClosedTabData(nsIDOMWindow *aWindow, nsAString & _retval) { return _to GetClosedTabData(aWindow, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD UndoCloseTab(nsIDOMWindow *aWindow, PRUint32 aIndex) { return _to UndoCloseTab(aWindow, aIndex); } \
  NS_SCRIPTABLE NS_IMETHOD GetWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey, nsAString & _retval) { return _to GetWindowValue(aWindow, aKey, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey, const nsAString & aStringValue) { return _to SetWindowValue(aWindow, aKey, aStringValue); } \
  NS_SCRIPTABLE NS_IMETHOD DeleteWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey) { return _to DeleteWindowValue(aWindow, aKey); } \
  NS_SCRIPTABLE NS_IMETHOD GetTabValue(nsIDOMNode *aTab, const nsAString & aKey, nsAString & _retval) { return _to GetTabValue(aTab, aKey, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetTabValue(nsIDOMNode *aTab, const nsAString & aKey, const nsAString & aStringValue) { return _to SetTabValue(aTab, aKey, aStringValue); } \
  NS_SCRIPTABLE NS_IMETHOD DeleteTabValue(nsIDOMNode *aTab, const nsAString & aKey) { return _to DeleteTabValue(aTab, aKey); } \
  NS_SCRIPTABLE NS_IMETHOD PersistTabAttribute(const nsAString & aName) { return _to PersistTabAttribute(aName); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISESSIONSTORE(_to) \
  NS_SCRIPTABLE NS_IMETHOD Init(nsIDOMWindow *aWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->Init(aWindow); } \
  NS_SCRIPTABLE NS_IMETHOD GetBrowserState(nsAString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetBrowserState(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetBrowserState(const nsAString & aState) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetBrowserState(aState); } \
  NS_SCRIPTABLE NS_IMETHOD GetWindowState(nsIDOMWindow *aWindow, nsAString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetWindowState(aWindow, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetWindowState(nsIDOMWindow *aWindow, const nsAString & aState, PRBool aOverwrite) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetWindowState(aWindow, aState, aOverwrite); } \
  NS_SCRIPTABLE NS_IMETHOD GetTabState(nsIDOMNode *aTab, nsAString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTabState(aTab, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetTabState(nsIDOMNode *aTab, const nsAString & aState) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTabState(aTab, aState); } \
  NS_SCRIPTABLE NS_IMETHOD DuplicateTab(nsIDOMWindow *aWindow, nsIDOMNode *aTab, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->DuplicateTab(aWindow, aTab, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetClosedTabCount(nsIDOMWindow *aWindow, PRUint32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetClosedTabCount(aWindow, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetClosedTabData(nsIDOMWindow *aWindow, nsAString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetClosedTabData(aWindow, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD UndoCloseTab(nsIDOMWindow *aWindow, PRUint32 aIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->UndoCloseTab(aWindow, aIndex); } \
  NS_SCRIPTABLE NS_IMETHOD GetWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey, nsAString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetWindowValue(aWindow, aKey, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey, const nsAString & aStringValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetWindowValue(aWindow, aKey, aStringValue); } \
  NS_SCRIPTABLE NS_IMETHOD DeleteWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->DeleteWindowValue(aWindow, aKey); } \
  NS_SCRIPTABLE NS_IMETHOD GetTabValue(nsIDOMNode *aTab, const nsAString & aKey, nsAString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTabValue(aTab, aKey, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetTabValue(nsIDOMNode *aTab, const nsAString & aKey, const nsAString & aStringValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTabValue(aTab, aKey, aStringValue); } \
  NS_SCRIPTABLE NS_IMETHOD DeleteTabValue(nsIDOMNode *aTab, const nsAString & aKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->DeleteTabValue(aTab, aKey); } \
  NS_SCRIPTABLE NS_IMETHOD PersistTabAttribute(const nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->PersistTabAttribute(aName); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsSessionStore : public nsISessionStore
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISESSIONSTORE

  nsSessionStore();

private:
  ~nsSessionStore();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsSessionStore, nsISessionStore)

nsSessionStore::nsSessionStore()
{
  /* member initializers and constructor code */
}

nsSessionStore::~nsSessionStore()
{
  /* destructor code */
}

/* void init (in nsIDOMWindow aWindow); */
NS_IMETHODIMP nsSessionStore::Init(nsIDOMWindow *aWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AString getBrowserState (); */
NS_IMETHODIMP nsSessionStore::GetBrowserState(nsAString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setBrowserState (in AString aState); */
NS_IMETHODIMP nsSessionStore::SetBrowserState(const nsAString & aState)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AString getWindowState (in nsIDOMWindow aWindow); */
NS_IMETHODIMP nsSessionStore::GetWindowState(nsIDOMWindow *aWindow, nsAString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setWindowState (in nsIDOMWindow aWindow, in AString aState, in boolean aOverwrite); */
NS_IMETHODIMP nsSessionStore::SetWindowState(nsIDOMWindow *aWindow, const nsAString & aState, PRBool aOverwrite)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AString getTabState (in nsIDOMNode aTab); */
NS_IMETHODIMP nsSessionStore::GetTabState(nsIDOMNode *aTab, nsAString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setTabState (in nsIDOMNode aTab, in AString aState); */
NS_IMETHODIMP nsSessionStore::SetTabState(nsIDOMNode *aTab, const nsAString & aState)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode duplicateTab (in nsIDOMWindow aWindow, in nsIDOMNode aTab); */
NS_IMETHODIMP nsSessionStore::DuplicateTab(nsIDOMWindow *aWindow, nsIDOMNode *aTab, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* unsigned long getClosedTabCount (in nsIDOMWindow aWindow); */
NS_IMETHODIMP nsSessionStore::GetClosedTabCount(nsIDOMWindow *aWindow, PRUint32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AString getClosedTabData (in nsIDOMWindow aWindow); */
NS_IMETHODIMP nsSessionStore::GetClosedTabData(nsIDOMWindow *aWindow, nsAString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void undoCloseTab (in nsIDOMWindow aWindow, in unsigned long aIndex); */
NS_IMETHODIMP nsSessionStore::UndoCloseTab(nsIDOMWindow *aWindow, PRUint32 aIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AString getWindowValue (in nsIDOMWindow aWindow, in AString aKey); */
NS_IMETHODIMP nsSessionStore::GetWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey, nsAString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setWindowValue (in nsIDOMWindow aWindow, in AString aKey, in AString aStringValue); */
NS_IMETHODIMP nsSessionStore::SetWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey, const nsAString & aStringValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void deleteWindowValue (in nsIDOMWindow aWindow, in AString aKey); */
NS_IMETHODIMP nsSessionStore::DeleteWindowValue(nsIDOMWindow *aWindow, const nsAString & aKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AString getTabValue (in nsIDOMNode aTab, in AString aKey); */
NS_IMETHODIMP nsSessionStore::GetTabValue(nsIDOMNode *aTab, const nsAString & aKey, nsAString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setTabValue (in nsIDOMNode aTab, in AString aKey, in AString aStringValue); */
NS_IMETHODIMP nsSessionStore::SetTabValue(nsIDOMNode *aTab, const nsAString & aKey, const nsAString & aStringValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void deleteTabValue (in nsIDOMNode aTab, in AString aKey); */
NS_IMETHODIMP nsSessionStore::DeleteTabValue(nsIDOMNode *aTab, const nsAString & aKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void persistTabAttribute (in AString aName); */
NS_IMETHODIMP nsSessionStore::PersistTabAttribute(const nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsISessionStore_h__ */
