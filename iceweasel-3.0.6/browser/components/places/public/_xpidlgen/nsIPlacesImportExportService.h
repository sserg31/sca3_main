/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsIPlacesImportExportService.idl
 */

#ifndef __gen_nsIPlacesImportExportService_h__
#define __gen_nsIPlacesImportExportService_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsILocalFile; /* forward declaration */


/* starting interface:    nsIPlacesImportExportService */
#define NS_IPLACESIMPORTEXPORTSERVICE_IID_STR "21c00314-fa63-11db-8314-0800200c9a66"

#define NS_IPLACESIMPORTEXPORTSERVICE_IID \
  {0x21c00314, 0xfa63, 0x11db, \
    { 0x83, 0x14, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 }}

/**
 * The PlacesImportExport interface provides methods for importing
 * and exporting Places data.
 */
class NS_NO_VTABLE NS_SCRIPTABLE nsIPlacesImportExportService : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IPLACESIMPORTEXPORTSERVICE_IID)

  /**
   * Loads the given bookmarks.html file and replaces it with the current
   * bookmarks hierarchy (if aIsInitialImport is true) or appends it
   * (if aIsInitialImport is false)
   */
  /* void importHTMLFromFile (in nsILocalFile aFile, in boolean aIsInitialImport); */
  NS_SCRIPTABLE NS_IMETHOD ImportHTMLFromFile(nsILocalFile *aFile, PRBool aIsInitialImport) = 0;

  /**
   * Loads the given bookmarks.html file and puts it in the given folder
   */
  /* void importHTMLFromFileToFolder (in nsILocalFile aFile, in PRInt64 aFolder, in boolean aIsInitialImport); */
  NS_SCRIPTABLE NS_IMETHOD ImportHTMLFromFileToFolder(nsILocalFile *aFile, PRInt64 aFolder, PRBool aIsInitialImport) = 0;

  /**
   * Saves the current bookmarks hierarchy to a bookmarks.html file.
   */
  /* void exportHTMLToFile (in nsILocalFile aFile); */
  NS_SCRIPTABLE NS_IMETHOD ExportHTMLToFile(nsILocalFile *aFile) = 0;

  /**
   * Backup the bookmarks.html file.
   */
  /* void backupBookmarksFile (); */
  NS_SCRIPTABLE NS_IMETHOD BackupBookmarksFile(void) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIPlacesImportExportService, NS_IPLACESIMPORTEXPORTSERVICE_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIPLACESIMPORTEXPORTSERVICE \
  NS_SCRIPTABLE NS_IMETHOD ImportHTMLFromFile(nsILocalFile *aFile, PRBool aIsInitialImport); \
  NS_SCRIPTABLE NS_IMETHOD ImportHTMLFromFileToFolder(nsILocalFile *aFile, PRInt64 aFolder, PRBool aIsInitialImport); \
  NS_SCRIPTABLE NS_IMETHOD ExportHTMLToFile(nsILocalFile *aFile); \
  NS_SCRIPTABLE NS_IMETHOD BackupBookmarksFile(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIPLACESIMPORTEXPORTSERVICE(_to) \
  NS_SCRIPTABLE NS_IMETHOD ImportHTMLFromFile(nsILocalFile *aFile, PRBool aIsInitialImport) { return _to ImportHTMLFromFile(aFile, aIsInitialImport); } \
  NS_SCRIPTABLE NS_IMETHOD ImportHTMLFromFileToFolder(nsILocalFile *aFile, PRInt64 aFolder, PRBool aIsInitialImport) { return _to ImportHTMLFromFileToFolder(aFile, aFolder, aIsInitialImport); } \
  NS_SCRIPTABLE NS_IMETHOD ExportHTMLToFile(nsILocalFile *aFile) { return _to ExportHTMLToFile(aFile); } \
  NS_SCRIPTABLE NS_IMETHOD BackupBookmarksFile(void) { return _to BackupBookmarksFile(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIPLACESIMPORTEXPORTSERVICE(_to) \
  NS_SCRIPTABLE NS_IMETHOD ImportHTMLFromFile(nsILocalFile *aFile, PRBool aIsInitialImport) { return !_to ? NS_ERROR_NULL_POINTER : _to->ImportHTMLFromFile(aFile, aIsInitialImport); } \
  NS_SCRIPTABLE NS_IMETHOD ImportHTMLFromFileToFolder(nsILocalFile *aFile, PRInt64 aFolder, PRBool aIsInitialImport) { return !_to ? NS_ERROR_NULL_POINTER : _to->ImportHTMLFromFileToFolder(aFile, aFolder, aIsInitialImport); } \
  NS_SCRIPTABLE NS_IMETHOD ExportHTMLToFile(nsILocalFile *aFile) { return !_to ? NS_ERROR_NULL_POINTER : _to->ExportHTMLToFile(aFile); } \
  NS_SCRIPTABLE NS_IMETHOD BackupBookmarksFile(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->BackupBookmarksFile(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsPlacesImportExportService : public nsIPlacesImportExportService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPLACESIMPORTEXPORTSERVICE

  nsPlacesImportExportService();

private:
  ~nsPlacesImportExportService();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsPlacesImportExportService, nsIPlacesImportExportService)

nsPlacesImportExportService::nsPlacesImportExportService()
{
  /* member initializers and constructor code */
}

nsPlacesImportExportService::~nsPlacesImportExportService()
{
  /* destructor code */
}

/* void importHTMLFromFile (in nsILocalFile aFile, in boolean aIsInitialImport); */
NS_IMETHODIMP nsPlacesImportExportService::ImportHTMLFromFile(nsILocalFile *aFile, PRBool aIsInitialImport)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void importHTMLFromFileToFolder (in nsILocalFile aFile, in PRInt64 aFolder, in boolean aIsInitialImport); */
NS_IMETHODIMP nsPlacesImportExportService::ImportHTMLFromFileToFolder(nsILocalFile *aFile, PRInt64 aFolder, PRBool aIsInitialImport)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void exportHTMLToFile (in nsILocalFile aFile); */
NS_IMETHODIMP nsPlacesImportExportService::ExportHTMLToFile(nsILocalFile *aFile)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void backupBookmarksFile (); */
NS_IMETHODIMP nsPlacesImportExportService::BackupBookmarksFile()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIPlacesImportExportService_h__ */
