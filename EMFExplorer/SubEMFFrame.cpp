#include "pch.h"
#include "framework.h"
#include "EMFExplorer.h"

#include "SubEMFFrame.h"
#include "EMFExplorerView.h"

IMPLEMENT_DYNCREATE(CSubEMFFrame, CSubEMFFrameBase)

BEGIN_MESSAGE_MAP(CSubEMFFrame, CSubEMFFrameBase)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, &CSubEMFFrame::OnUpdateDisableCommand)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CSubEMFFrame::OnUpdateDisableCommand)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CSubEMFFrame::OnUpdateDisableCommand)
END_MESSAGE_MAP()

CSubEMFFrame::CSubEMFFrame() noexcept
{

}

void CSubEMFFrame::OnUpdateDisableCommand(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}


