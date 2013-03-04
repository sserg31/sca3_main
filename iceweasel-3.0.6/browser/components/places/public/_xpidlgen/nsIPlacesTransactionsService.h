/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsIPlacesTransactionsService.idl
 */

#ifndef __gen_nsIPlacesTransactionsService_h__
#define __gen_nsIPlacesTransactionsService_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_nsITransactionManager_h__
#include "nsITransactionManager.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIVariant; /* forward declaration */

class nsIURI; /* forward declaration */

class nsIMicrosummary; /* forward declaration */

class nsITransaction; /* forward declaration */


/* starting interface:    nsIPlacesTransactionsService */
#define NS_IPLACESTRANSACTIONSSERVICE_IID_STR "7179d28c-bc41-4110-8225-b3ba7e1cb293"

#define NS_IPLACESTRANSACTIONSSERVICE_IID \
  {0x7179d28c, 0xbc41, 0x4110, \
    { 0x82, 0x25, 0xb3, 0xba, 0x7e, 0x1c, 0xb2, 0x93 }}

/**
 * nsIPlacesTransactionService is a service designed to handle
 * nsITransactions that correspond to changes in Places. It is here as a
 * service so that we can keep the transactions around without holding onto
 * the global scope of a js window.
 */
class NS_NO_VTABLE NS_SCRIPTABLE nsIPlacesTransactionsService : public nsITransactionManager {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IPLACESTRANSACTIONSSERVICE_IID)

  /**
   * Transaction for performing several Places Transactions in a single batch. 
   * 
   * @param aName
   *        title of the aggregate transactions
   * @param aTransactions
   *        an array of transactions to perform
   * @returns nsITransaction object
   */
  /* nsITransaction aggregateTransactions (in AString aName, in nsIVariant aTransactions); */
  NS_SCRIPTABLE NS_IMETHOD AggregateTransactions(const nsAString & aName, nsIVariant *aTransactions, nsITransaction **_retval) = 0;

  /**
   * Transaction for creating a new folder item.
   *
   * @param aName
   *        the name of the new folder
   * @param aContainer
   *        the identifier of the folder in which the new folder should be
   *        added.
   * @param [optional] aIndex
   *        the index of the item in aContainer, pass -1 or nothing to create
   *        the item at the end of aContainer.
   * @param [optional] aAnnotations
   *        the annotations to set for the new folder.
   * @param [optional] aChildItemsTransactions
   *        array of transactions for items to be created under the new folder.
   * @returns nsITransaction object
   */
  /* nsITransaction createFolder (in AString aName, in long long aContainer, [optional] in long long aIndex, [optional] in nsIVariant aAnnotations, [optional] in nsIVariant aChildItemsTransactions); */
  NS_SCRIPTABLE NS_IMETHOD CreateFolder(const nsAString & aName, PRInt64 aContainer, PRInt64 aIndex, nsIVariant *aAnnotations, nsIVariant *aChildItemsTransactions, nsITransaction **_retval) = 0;

  /**
   * Transaction for creating a new bookmark item
   *
   * @param aURI
   *        the uri of the new bookmark (nsIURI)
   * @param aContainer
   *        the identifier of the folder in which the bookmark should be added.
   * @param [optional] aIndex
   *        the index of the item in aContainer, pass -1 or nothing to create
   *        the item at the end of aContainer.
   * @param [optional] aTitle
   *        the title of the new bookmark.
   * @param [optional] aKeyword
   *        the keyword of the new bookmark.
   * @param [optional] aAnnotations
   *        the annotations to set for the new bookmark.
   * @param [optional] aChildTransactions
   *        child transactions to commit after creating the bookmark. Prefer
   *        using any of the arguments above if possible. In general, a child
   *        transations should be used only if the change it does has to be
   *        reverted manually when removing the bookmark item.
   *        a child transaction must support setting its bookmark-item
   *        identifier via an "id" js setter.
   * @returns nsITransaction object
   */
  /* nsITransaction createItem (in nsIURI aURI, in long long aContainer, [optional] in long long aIndex, [optional] in AString aTitle, [optional] in AString aKeyword, [optional] in nsIVariant aAnnotations, [optional] in nsIVariant aChildTransactions); */
  NS_SCRIPTABLE NS_IMETHOD CreateItem(nsIURI *aURI, PRInt64 aContainer, PRInt64 aIndex, const nsAString & aTitle, const nsAString & aKeyword, nsIVariant *aAnnotations, nsIVariant *aChildTransactions, nsITransaction **_retval) = 0;

  /**
   * Transaction for creating a new separator item
   *
   * @param aContainer
   *        the identifier of the folder in which the separator should be
   *        added.
   * @param [optional] aIndex
   *        the index of the item in aContainer, pass -1 or nothing to create
   *        the separator at the end of aContainer.
   * @returns nsITransaction object
   */
  /* nsITransaction createSeparator (in long long aContainer, [optional] in long long aIndex); */
  NS_SCRIPTABLE NS_IMETHOD CreateSeparator(PRInt64 aContainer, PRInt64 aIndex, nsITransaction **_retval) = 0;

  /**
   * Transaction for creating a new live-bookmark item.
   *
   * @see nsILivemarksService::createLivemark for documentation regarding the
   * first three arguments.
   *
   * @param aContainer
   *        the identifier of the folder in which the live-bookmark should be
   *        added.
   * @param [optional]  aIndex
   *        the index of the item in aContainer, pass -1 or nothing to create
   *        the item at the end of aContainer.
   * @param [optional] aAnnotations
   *        the annotations to set for the new live-bookmark.
   * @returns nsITransaction object
   */
  /* nsITransaction createLivemark (in nsIURI aFeedURI, in nsIURI aSiteURI, in AString aName, in long long aContainer, [optional] in long long aIndex, [optional] in nsIVariant aAnnotations); */
  NS_SCRIPTABLE NS_IMETHOD CreateLivemark(nsIURI *aFeedURI, nsIURI *aSiteURI, const nsAString & aName, PRInt64 aContainer, PRInt64 aIndex, nsIVariant *aAnnotations, nsITransaction **_retval) = 0;

  /**
   * Transaction for moving an Item.
   *
   * @param aItemId
   *        the id of the item to move
   * @param aNewContainer
   *        id of the new container to move to
   * @param aNewIndex
   *        index of the new position to move to
   * @returns nsITransaction object
   */
  /* nsITransaction moveItem (in long long aItemId, in long long aNewContainer, in long long aNewIndex); */
  NS_SCRIPTABLE NS_IMETHOD MoveItem(PRInt64 aItemId, PRInt64 aNewContainer, PRInt64 aNewIndex, nsITransaction **_retval) = 0;

  /**
   * Transaction for removing an Item
   *
   * @param aItemId
   *        id of the item to remove
   * @returns nsITransaction object
   */
  /* nsITransaction removeItem (in long long aItemId); */
  NS_SCRIPTABLE NS_IMETHOD RemoveItem(PRInt64 aItemId, nsITransaction **_retval) = 0;

  /**
   * Transaction for editting a bookmark's title.
   *
   * @param id
   *        id of the item to edit
   * @param newTitle
   *        new title for the item to edit
   * @returns nsITransaction object
   */
  /* nsITransaction editItemTitle (in long long id, in AString newTitle); */
  NS_SCRIPTABLE NS_IMETHOD EditItemTitle(PRInt64 id, const nsAString & newTitle, nsITransaction **_retval) = 0;

  /**
   * Transaction for editing a bookmark's uri.
   *
   * @param aBookmarkId
   *        id of the bookmark to edit
   * @param aNewURI
   *        new uri for the bookmark
   * @returns nsITransaction object
   */
  /* nsITransaction editBookmarkURI (in long long aBookmarkId, in nsIURI aNewURI); */
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkURI(PRInt64 aBookmarkId, nsIURI *aNewURI, nsITransaction **_retval) = 0;

  /**
   * Transaction for setting/unsetting Load-in-sidebar annotation
   *
   * @param aBookmarkId
   *        id of the selected bookmark
   * @param aLoadInSidebar
   *        boolean value
   * @returns nsITransaction object
   */
  /* nsITransaction setLoadInSidebar (in long long aBookmarkId, in boolean aLoadInSidebar); */
  NS_SCRIPTABLE NS_IMETHOD SetLoadInSidebar(PRInt64 aBookmarkId, PRBool aLoadInSidebar, nsITransaction **_retval) = 0;

  /**
   * Transaction for editing a the description of a bookmark or a folder
   * 
   * @param aItemId
   *        id of the item to edit
   * @param aDescription
   *        new description
   * @returns nsITransaction object
   */
  /* nsITransaction editItemDescription (in long long aItemId, in AString aDescription); */
  NS_SCRIPTABLE NS_IMETHOD EditItemDescription(PRInt64 aItemId, const nsAString & aDescription, nsITransaction **_retval) = 0;

  /**
   * Transaction for editing a bookmark's keyword.
   *
   * @param id
   *        id of the bookmark to edit
   * @param newKeyword
   *        new keyword for the bookmark
   * @returns nsITransaction object
   */
  /* nsITransaction editBookmarkKeyword (in long long aItemId, in AString aNewKeyword); */
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkKeyword(PRInt64 aItemId, const nsAString & aNewKeyword, nsITransaction **_retval) = 0;

  /**
   * Transaction for editing the post data associated with a bookmark.
   *
   * @param aItemId
   *        id of the bookmark to edit
   * @param aPostData
   *        post data
   * @returns nsITransaction object
   */
  /* nsITransaction editBookmarkPostData (in long long aItemId, in AString aPostData); */
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkPostData(PRInt64 aItemId, const nsAString & aPostData, nsITransaction **_retval) = 0;

  /**
   * Transaction for editing a live bookmark's site URI.
   *
   * @param aFolderId
   *        id of the livemark
   * @param aURI
   *        new site uri
   * @returns nsITransaction object
   */
  /* nsITransaction editLivemarkSiteURI (in long long aFolderId, in nsIURI aURI); */
  NS_SCRIPTABLE NS_IMETHOD EditLivemarkSiteURI(PRInt64 aFolderId, nsIURI *aURI, nsITransaction **_retval) = 0;

  /**
   * Transaction for editting a live bookmark's feed URI.
   *
   * @param folderId
   *        id of the livemark
   * @param uri
   *        new feed uri
   * @returns nsITransaction object
   */
  /* nsITransaction editLivemarkFeedURI (in long long folderId, in nsIURI uri); */
  NS_SCRIPTABLE NS_IMETHOD EditLivemarkFeedURI(PRInt64 folderId, nsIURI *uri, nsITransaction **_retval) = 0;

  /**
   * Transaction for editing a bookmark's microsummary.
   *
   * @param aItemId
   *        id of the bookmark to edit
   * @param aNewMicrosummary
   *        new microsummary for the bookmark
   * @returns nsITransaction object
   */
  /* nsITransaction editBookmarkMicrosummary (in long long aItemId, in nsIMicrosummary aNewMicrosummary); */
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkMicrosummary(PRInt64 aItemId, nsIMicrosummary *aNewMicrosummary, nsITransaction **_retval) = 0;

  /**
   * Transaction for editing an item's date added property.
   *
   * @param aItemId
   *        id of the item to edit
   * @param aNewDateAdded
   *        new date added for the item 
   * @returns nsITransaction object
   */
  /* nsITransaction editItemDateAdded (in long long aItemId, in PRTime aNewDateAdded); */
  NS_SCRIPTABLE NS_IMETHOD EditItemDateAdded(PRInt64 aItemId, PRTime aNewDateAdded, nsITransaction **_retval) = 0;

  /**
   * Transaction for editing an item's last modified time.
   *
   * @param aItemId
   *        id of the item to edit
   * @param aNewLastModified
   *        new last modified date for the item 
   * @returns nsITransaction object
   */
  /* nsITransaction editItemLastModified (in long long aItemId, in PRTime aNewLastModified); */
  NS_SCRIPTABLE NS_IMETHOD EditItemLastModified(PRInt64 aItemId, PRTime aNewLastModified, nsITransaction **_retval) = 0;

  /**
   * Transaction for sorting a folder by name
   *
   * @param aFolderId
   *        id of the folder to sort
   * @returns nsITransaction object
   */
  /* nsITransaction sortFolderByName (in long long aFolderId); */
  NS_SCRIPTABLE NS_IMETHOD SortFolderByName(PRInt64 aFolderId, nsITransaction **_retval) = 0;

  /**
   * Transaction for tagging a URL with the given set of tags. Current tags set
   * for the URL persist. It's the caller's job to check whether or not aURI
   * was already tagged by any of the tags in aTags, undoing this tags
   * transaction removes them all from aURL!
   *
   * @param aURI
   *        the URL to tag.
   * @param aTags
   *        Array of tags to set for the given URL.
   */
  /* nsITransaction tagURI (in nsIURI aURI, in nsIVariant aTags); */
  NS_SCRIPTABLE NS_IMETHOD TagURI(nsIURI *aURI, nsIVariant *aTags, nsITransaction **_retval) = 0;

  /**
   * Transaction for removing tags from a URL. It's the caller's job to check
   * whether or not aURI isn't tagged by any of the tags in aTags, undoing this
   * tags transaction adds them all to aURL!
   *
   * @param aURI
   *        the URL to un-tag.
   * @param aTags
   *        Array of tags to unset. pass null to remove all tags from the given
   *        url.
   */
  /* nsITransaction untagURI (in nsIURI aURI, in nsIVariant aTags); */
  NS_SCRIPTABLE NS_IMETHOD UntagURI(nsIURI *aURI, nsIVariant *aTags, nsITransaction **_retval) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIPlacesTransactionsService, NS_IPLACESTRANSACTIONSSERVICE_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIPLACESTRANSACTIONSSERVICE \
  NS_SCRIPTABLE NS_IMETHOD AggregateTransactions(const nsAString & aName, nsIVariant *aTransactions, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD CreateFolder(const nsAString & aName, PRInt64 aContainer, PRInt64 aIndex, nsIVariant *aAnnotations, nsIVariant *aChildItemsTransactions, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD CreateItem(nsIURI *aURI, PRInt64 aContainer, PRInt64 aIndex, const nsAString & aTitle, const nsAString & aKeyword, nsIVariant *aAnnotations, nsIVariant *aChildTransactions, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD CreateSeparator(PRInt64 aContainer, PRInt64 aIndex, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD CreateLivemark(nsIURI *aFeedURI, nsIURI *aSiteURI, const nsAString & aName, PRInt64 aContainer, PRInt64 aIndex, nsIVariant *aAnnotations, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD MoveItem(PRInt64 aItemId, PRInt64 aNewContainer, PRInt64 aNewIndex, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD RemoveItem(PRInt64 aItemId, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD EditItemTitle(PRInt64 id, const nsAString & newTitle, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkURI(PRInt64 aBookmarkId, nsIURI *aNewURI, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD SetLoadInSidebar(PRInt64 aBookmarkId, PRBool aLoadInSidebar, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD EditItemDescription(PRInt64 aItemId, const nsAString & aDescription, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkKeyword(PRInt64 aItemId, const nsAString & aNewKeyword, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkPostData(PRInt64 aItemId, const nsAString & aPostData, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD EditLivemarkSiteURI(PRInt64 aFolderId, nsIURI *aURI, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD EditLivemarkFeedURI(PRInt64 folderId, nsIURI *uri, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkMicrosummary(PRInt64 aItemId, nsIMicrosummary *aNewMicrosummary, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD EditItemDateAdded(PRInt64 aItemId, PRTime aNewDateAdded, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD EditItemLastModified(PRInt64 aItemId, PRTime aNewLastModified, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD SortFolderByName(PRInt64 aFolderId, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD TagURI(nsIURI *aURI, nsIVariant *aTags, nsITransaction **_retval); \
  NS_SCRIPTABLE NS_IMETHOD UntagURI(nsIURI *aURI, nsIVariant *aTags, nsITransaction **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIPLACESTRANSACTIONSSERVICE(_to) \
  NS_SCRIPTABLE NS_IMETHOD AggregateTransactions(const nsAString & aName, nsIVariant *aTransactions, nsITransaction **_retval) { return _to AggregateTransactions(aName, aTransactions, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD CreateFolder(const nsAString & aName, PRInt64 aContainer, PRInt64 aIndex, nsIVariant *aAnnotations, nsIVariant *aChildItemsTransactions, nsITransaction **_retval) { return _to CreateFolder(aName, aContainer, aIndex, aAnnotations, aChildItemsTransactions, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD CreateItem(nsIURI *aURI, PRInt64 aContainer, PRInt64 aIndex, const nsAString & aTitle, const nsAString & aKeyword, nsIVariant *aAnnotations, nsIVariant *aChildTransactions, nsITransaction **_retval) { return _to CreateItem(aURI, aContainer, aIndex, aTitle, aKeyword, aAnnotations, aChildTransactions, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD CreateSeparator(PRInt64 aContainer, PRInt64 aIndex, nsITransaction **_retval) { return _to CreateSeparator(aContainer, aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD CreateLivemark(nsIURI *aFeedURI, nsIURI *aSiteURI, const nsAString & aName, PRInt64 aContainer, PRInt64 aIndex, nsIVariant *aAnnotations, nsITransaction **_retval) { return _to CreateLivemark(aFeedURI, aSiteURI, aName, aContainer, aIndex, aAnnotations, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD MoveItem(PRInt64 aItemId, PRInt64 aNewContainer, PRInt64 aNewIndex, nsITransaction **_retval) { return _to MoveItem(aItemId, aNewContainer, aNewIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD RemoveItem(PRInt64 aItemId, nsITransaction **_retval) { return _to RemoveItem(aItemId, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditItemTitle(PRInt64 id, const nsAString & newTitle, nsITransaction **_retval) { return _to EditItemTitle(id, newTitle, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkURI(PRInt64 aBookmarkId, nsIURI *aNewURI, nsITransaction **_retval) { return _to EditBookmarkURI(aBookmarkId, aNewURI, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetLoadInSidebar(PRInt64 aBookmarkId, PRBool aLoadInSidebar, nsITransaction **_retval) { return _to SetLoadInSidebar(aBookmarkId, aLoadInSidebar, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditItemDescription(PRInt64 aItemId, const nsAString & aDescription, nsITransaction **_retval) { return _to EditItemDescription(aItemId, aDescription, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkKeyword(PRInt64 aItemId, const nsAString & aNewKeyword, nsITransaction **_retval) { return _to EditBookmarkKeyword(aItemId, aNewKeyword, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkPostData(PRInt64 aItemId, const nsAString & aPostData, nsITransaction **_retval) { return _to EditBookmarkPostData(aItemId, aPostData, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditLivemarkSiteURI(PRInt64 aFolderId, nsIURI *aURI, nsITransaction **_retval) { return _to EditLivemarkSiteURI(aFolderId, aURI, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditLivemarkFeedURI(PRInt64 folderId, nsIURI *uri, nsITransaction **_retval) { return _to EditLivemarkFeedURI(folderId, uri, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkMicrosummary(PRInt64 aItemId, nsIMicrosummary *aNewMicrosummary, nsITransaction **_retval) { return _to EditBookmarkMicrosummary(aItemId, aNewMicrosummary, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditItemDateAdded(PRInt64 aItemId, PRTime aNewDateAdded, nsITransaction **_retval) { return _to EditItemDateAdded(aItemId, aNewDateAdded, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditItemLastModified(PRInt64 aItemId, PRTime aNewLastModified, nsITransaction **_retval) { return _to EditItemLastModified(aItemId, aNewLastModified, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SortFolderByName(PRInt64 aFolderId, nsITransaction **_retval) { return _to SortFolderByName(aFolderId, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD TagURI(nsIURI *aURI, nsIVariant *aTags, nsITransaction **_retval) { return _to TagURI(aURI, aTags, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD UntagURI(nsIURI *aURI, nsIVariant *aTags, nsITransaction **_retval) { return _to UntagURI(aURI, aTags, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIPLACESTRANSACTIONSSERVICE(_to) \
  NS_SCRIPTABLE NS_IMETHOD AggregateTransactions(const nsAString & aName, nsIVariant *aTransactions, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->AggregateTransactions(aName, aTransactions, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD CreateFolder(const nsAString & aName, PRInt64 aContainer, PRInt64 aIndex, nsIVariant *aAnnotations, nsIVariant *aChildItemsTransactions, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateFolder(aName, aContainer, aIndex, aAnnotations, aChildItemsTransactions, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD CreateItem(nsIURI *aURI, PRInt64 aContainer, PRInt64 aIndex, const nsAString & aTitle, const nsAString & aKeyword, nsIVariant *aAnnotations, nsIVariant *aChildTransactions, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateItem(aURI, aContainer, aIndex, aTitle, aKeyword, aAnnotations, aChildTransactions, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD CreateSeparator(PRInt64 aContainer, PRInt64 aIndex, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateSeparator(aContainer, aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD CreateLivemark(nsIURI *aFeedURI, nsIURI *aSiteURI, const nsAString & aName, PRInt64 aContainer, PRInt64 aIndex, nsIVariant *aAnnotations, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateLivemark(aFeedURI, aSiteURI, aName, aContainer, aIndex, aAnnotations, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD MoveItem(PRInt64 aItemId, PRInt64 aNewContainer, PRInt64 aNewIndex, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->MoveItem(aItemId, aNewContainer, aNewIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD RemoveItem(PRInt64 aItemId, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->RemoveItem(aItemId, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditItemTitle(PRInt64 id, const nsAString & newTitle, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->EditItemTitle(id, newTitle, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkURI(PRInt64 aBookmarkId, nsIURI *aNewURI, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->EditBookmarkURI(aBookmarkId, aNewURI, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetLoadInSidebar(PRInt64 aBookmarkId, PRBool aLoadInSidebar, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetLoadInSidebar(aBookmarkId, aLoadInSidebar, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditItemDescription(PRInt64 aItemId, const nsAString & aDescription, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->EditItemDescription(aItemId, aDescription, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkKeyword(PRInt64 aItemId, const nsAString & aNewKeyword, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->EditBookmarkKeyword(aItemId, aNewKeyword, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkPostData(PRInt64 aItemId, const nsAString & aPostData, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->EditBookmarkPostData(aItemId, aPostData, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditLivemarkSiteURI(PRInt64 aFolderId, nsIURI *aURI, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->EditLivemarkSiteURI(aFolderId, aURI, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditLivemarkFeedURI(PRInt64 folderId, nsIURI *uri, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->EditLivemarkFeedURI(folderId, uri, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditBookmarkMicrosummary(PRInt64 aItemId, nsIMicrosummary *aNewMicrosummary, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->EditBookmarkMicrosummary(aItemId, aNewMicrosummary, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditItemDateAdded(PRInt64 aItemId, PRTime aNewDateAdded, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->EditItemDateAdded(aItemId, aNewDateAdded, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD EditItemLastModified(PRInt64 aItemId, PRTime aNewLastModified, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->EditItemLastModified(aItemId, aNewLastModified, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SortFolderByName(PRInt64 aFolderId, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SortFolderByName(aFolderId, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD TagURI(nsIURI *aURI, nsIVariant *aTags, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->TagURI(aURI, aTags, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD UntagURI(nsIURI *aURI, nsIVariant *aTags, nsITransaction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->UntagURI(aURI, aTags, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsPlacesTransactionsService : public nsIPlacesTransactionsService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPLACESTRANSACTIONSSERVICE

  nsPlacesTransactionsService();

private:
  ~nsPlacesTransactionsService();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsPlacesTransactionsService, nsIPlacesTransactionsService)

nsPlacesTransactionsService::nsPlacesTransactionsService()
{
  /* member initializers and constructor code */
}

nsPlacesTransactionsService::~nsPlacesTransactionsService()
{
  /* destructor code */
}

/* nsITransaction aggregateTransactions (in AString aName, in nsIVariant aTransactions); */
NS_IMETHODIMP nsPlacesTransactionsService::AggregateTransactions(const nsAString & aName, nsIVariant *aTransactions, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction createFolder (in AString aName, in long long aContainer, [optional] in long long aIndex, [optional] in nsIVariant aAnnotations, [optional] in nsIVariant aChildItemsTransactions); */
NS_IMETHODIMP nsPlacesTransactionsService::CreateFolder(const nsAString & aName, PRInt64 aContainer, PRInt64 aIndex, nsIVariant *aAnnotations, nsIVariant *aChildItemsTransactions, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction createItem (in nsIURI aURI, in long long aContainer, [optional] in long long aIndex, [optional] in AString aTitle, [optional] in AString aKeyword, [optional] in nsIVariant aAnnotations, [optional] in nsIVariant aChildTransactions); */
NS_IMETHODIMP nsPlacesTransactionsService::CreateItem(nsIURI *aURI, PRInt64 aContainer, PRInt64 aIndex, const nsAString & aTitle, const nsAString & aKeyword, nsIVariant *aAnnotations, nsIVariant *aChildTransactions, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction createSeparator (in long long aContainer, [optional] in long long aIndex); */
NS_IMETHODIMP nsPlacesTransactionsService::CreateSeparator(PRInt64 aContainer, PRInt64 aIndex, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction createLivemark (in nsIURI aFeedURI, in nsIURI aSiteURI, in AString aName, in long long aContainer, [optional] in long long aIndex, [optional] in nsIVariant aAnnotations); */
NS_IMETHODIMP nsPlacesTransactionsService::CreateLivemark(nsIURI *aFeedURI, nsIURI *aSiteURI, const nsAString & aName, PRInt64 aContainer, PRInt64 aIndex, nsIVariant *aAnnotations, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction moveItem (in long long aItemId, in long long aNewContainer, in long long aNewIndex); */
NS_IMETHODIMP nsPlacesTransactionsService::MoveItem(PRInt64 aItemId, PRInt64 aNewContainer, PRInt64 aNewIndex, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction removeItem (in long long aItemId); */
NS_IMETHODIMP nsPlacesTransactionsService::RemoveItem(PRInt64 aItemId, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction editItemTitle (in long long id, in AString newTitle); */
NS_IMETHODIMP nsPlacesTransactionsService::EditItemTitle(PRInt64 id, const nsAString & newTitle, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction editBookmarkURI (in long long aBookmarkId, in nsIURI aNewURI); */
NS_IMETHODIMP nsPlacesTransactionsService::EditBookmarkURI(PRInt64 aBookmarkId, nsIURI *aNewURI, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction setLoadInSidebar (in long long aBookmarkId, in boolean aLoadInSidebar); */
NS_IMETHODIMP nsPlacesTransactionsService::SetLoadInSidebar(PRInt64 aBookmarkId, PRBool aLoadInSidebar, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction editItemDescription (in long long aItemId, in AString aDescription); */
NS_IMETHODIMP nsPlacesTransactionsService::EditItemDescription(PRInt64 aItemId, const nsAString & aDescription, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction editBookmarkKeyword (in long long aItemId, in AString aNewKeyword); */
NS_IMETHODIMP nsPlacesTransactionsService::EditBookmarkKeyword(PRInt64 aItemId, const nsAString & aNewKeyword, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction editBookmarkPostData (in long long aItemId, in AString aPostData); */
NS_IMETHODIMP nsPlacesTransactionsService::EditBookmarkPostData(PRInt64 aItemId, const nsAString & aPostData, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction editLivemarkSiteURI (in long long aFolderId, in nsIURI aURI); */
NS_IMETHODIMP nsPlacesTransactionsService::EditLivemarkSiteURI(PRInt64 aFolderId, nsIURI *aURI, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction editLivemarkFeedURI (in long long folderId, in nsIURI uri); */
NS_IMETHODIMP nsPlacesTransactionsService::EditLivemarkFeedURI(PRInt64 folderId, nsIURI *uri, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction editBookmarkMicrosummary (in long long aItemId, in nsIMicrosummary aNewMicrosummary); */
NS_IMETHODIMP nsPlacesTransactionsService::EditBookmarkMicrosummary(PRInt64 aItemId, nsIMicrosummary *aNewMicrosummary, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction editItemDateAdded (in long long aItemId, in PRTime aNewDateAdded); */
NS_IMETHODIMP nsPlacesTransactionsService::EditItemDateAdded(PRInt64 aItemId, PRTime aNewDateAdded, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction editItemLastModified (in long long aItemId, in PRTime aNewLastModified); */
NS_IMETHODIMP nsPlacesTransactionsService::EditItemLastModified(PRInt64 aItemId, PRTime aNewLastModified, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction sortFolderByName (in long long aFolderId); */
NS_IMETHODIMP nsPlacesTransactionsService::SortFolderByName(PRInt64 aFolderId, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction tagURI (in nsIURI aURI, in nsIVariant aTags); */
NS_IMETHODIMP nsPlacesTransactionsService::TagURI(nsIURI *aURI, nsIVariant *aTags, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsITransaction untagURI (in nsIURI aURI, in nsIVariant aTags); */
NS_IMETHODIMP nsPlacesTransactionsService::UntagURI(nsIURI *aURI, nsIVariant *aTags, nsITransaction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIPlacesTransactionsService_h__ */
