#include "pch.h"
#include "framework.h"
#include "EMFExplorer.h"

#include "SubEMFFrame.h"
#include "EMFExplorerView.h"

IMPLEMENT_DYNCREATE(CSubEMFFrame, CSubEMFFrameBase)

BEGIN_MESSAGE_MAP(CSubEMFFrame, CSubEMFFrameBase)
 	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, &CSubEMFFrame::OnUpdateDisableCommand)
 	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CSubEMFFrame::OnUpdateDisableCommand)
 	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CSubEMFFrame::OnUpdateDisableCommand)
 	ON_UPDATE_COMMAND_UI(ID_APP_EXIT, &CSubEMFFrame::OnUpdateDisableCommand)
END_MESSAGE_MAP()

CSubEMFFrame::CSubEMFFrame() noexcept
{

}

void CSubEMFFrame::OnUpdateDisableCommand(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_pMenu)
	{
		if (pCmdUI->m_nID == ID_APP_EXIT)
		{
			pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
			// Remove the separator
			int nCount = pCmdUI->m_pMenu->GetMenuItemCount();
			pCmdUI->m_pMenu->DeleteMenu(nCount-1, MF_BYPOSITION);
		}
		pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
	}
	else
		pCmdUI->Enable(FALSE);
}


