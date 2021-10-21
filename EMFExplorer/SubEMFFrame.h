#pragma once
#include "MainFrm.h"

using CSubEMFFrameBase = CMainFrame;

class CSubEMFFrame : public CSubEMFFrameBase
{
protected: // create from serialization only
	CSubEMFFrame() noexcept;
	DECLARE_DYNCREATE(CSubEMFFrame)

public:
	bool IsSubEMFFrame() const override { return true; }
protected:
	afx_msg void OnUpdateDisableCommand(CCmdUI* pCmdUI);
protected:
	DECLARE_MESSAGE_MAP()
};
