
// EMFExplorerDoc.h : interface of the CEMFExplorerDoc class
//


#pragma once
#include <memory>
#include "EMFRecAccess.h"

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

	void UpdateEMFData(const std::vector<emfplus::u8t>& data, EMFType type);

	std::shared_ptr<EMFAccess> GetEMFAccess() const { return m_emf; }

	void SetEMFAccess(std::shared_ptr<EMFAccess> emf, EMFType type);

// Operations
public:
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
	std::shared_ptr<EMFAccess>	m_emf;

	EMFType			m_type = EMFType::Invalid;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
