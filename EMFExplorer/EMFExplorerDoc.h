
// EMFExplorerDoc.h : interface of the CEMFExplorerDoc class
//


#pragma once
#include <memory>
#include "EMFAccess.h"

#ifdef SHARED_HANDLERS
using EMFAccessT = EMFAccessBase;
#else
using EMFAccessT = EMFAccess;
#endif // SHARED_HANDLERS

CRect GetFitRect(const CRect& rcDest, const SIZE& szSrc, bool bCenter = false, float* pfScale = nullptr);

class CEMFExplorerDoc : public CDocument
{
protected: // create from serialization only
	CEMFExplorerDoc() noexcept;
	DECLARE_DYNCREATE(CEMFExplorerDoc)

// Attributes
public:
	enum class EMFType
	{
		Invalid,
		FromFile,
		FromClipboard,
		FromEMFRecord,
	};
	inline EMFType GetEMFType() const { return m_type; }

	std::shared_ptr<EMFAccessT> GetEMFAccess() const { return m_emf; }

	void UpdateEMFData(const std::vector<BYTE>& data, EMFType type);

#ifndef SHARED_HANDLERS
	void SetEMFAccess(std::shared_ptr<EMFAccessT> emf, EMFType type);
#endif // SHARED_HANDLERS

// Operations
public:
#ifndef SHARED_HANDLERS
	BOOL DoFileSave() override;

	BOOL OnSaveDocument(LPCTSTR lpszPathName) override;
#endif // SHARED_HANDLERS
// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	void DeleteContents() override;

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	
#if 0	// not needed for now
	void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds) override;
#endif
	
	BOOL GetThumbnail(UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha) override;	
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CEMFExplorerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	std::shared_ptr<EMFAccessT>	m_emf;

	EMFType			m_type = EMFType::Invalid;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
