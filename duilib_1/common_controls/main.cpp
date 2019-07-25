// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "flash10a.tlh"

bool _stdcall CALLBACK NewWindow3t(VARIANT_BOOL* Cannel, BSTR url) {
	*Cannel = TRUE;
	return true;
}

typedef  bool (WINAPI * NewWindow3)(VARIANT_BOOL* Cannel, BSTR url);

class CMyWebBrwoser2Event : public DWebBrowserEvents2
{
private:
	NewWindow3 _sc;
	IWebBrowser2* _pWebBrowser;
public:
	CMyWebBrwoser2Event(NewWindow3 n3, IWebBrowser2* pWebBrowser)
		:_sc(n3), _pWebBrowser(pWebBrowser)
	{
	}
public: // IDispatch methods
	STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject)
	{
		HRESULT hr = E_NOINTERFACE;
		if (riid == __uuidof(IDispatch))
		{
			*ppvObject = (IDispatch*)this;
			AddRef();
			hr = S_OK;
		}
		else if (riid == __uuidof(DWebBrowserEvents2))
		{
			*ppvObject = (DWebBrowserEvents2*)this;
			AddRef();
			hr = S_OK;
		}

		return hr;
	}
	STDMETHODIMP_(ULONG) AddRef(void)
	{
		return 1;
	};
	STDMETHODIMP_(ULONG) Release(void)
	{
		return 1;
	}
	STDMETHOD(GetTypeInfoCount)(UINT*)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetTypeInfo)(UINT, LCID, ITypeInfo**)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetIDsOfNames)(REFIID, LPOLESTR *rgszNames, UINT, LCID, DISPID *rgDispId)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR* pDispParams, VARIANT FAR* pVarResult, EXCEPINFO FAR* pExcepInfo, unsigned int FAR* puArgErr)
	{
		HRESULT hr = S_OK;
		if (dispIdMember == DISPID_NEWWINDOW3)
		{
			_sc((pDispParams->rgvarg[3].pboolVal), (pDispParams->rgvarg->bstrVal));
			_pWebBrowser->Navigate(pDispParams->rgvarg->bstrVal, NULL, NULL, NULL, NULL);
		}

		return hr;
	}
};

CMyWebBrwoser2Event*  events = 0;

CDuiFrameWnd::CDuiFrameWnd() {

}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	 
	m_pStepTabLayout = static_cast<CAnimationTabLayoutUI*>(m_PaintManager.FindControl(_T("stepTabLayout")));
	m_pInstallText = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("textProgress")));
	m_pProgressBar = static_cast<CProgressUI*>(m_PaintManager.FindControl(_T("install")));
	m_pAgainBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnAgain")));	

	CActiveXUI* pActiveXUI = static_cast<CActiveXUI*>(m_PaintManager.FindControl(_T("ActiveXDemo1")));
	if (pActiveXUI)   
	{
		IWebBrowser2* pWebBrowser = NULL;

		pActiveXUI->SetDelayCreate(false);              // 相当于界面设计器里的DelayCreate属性改为FALSE，在duilib自带的FlashDemo里可以看到此属性为TRUE             
		pActiveXUI->CreateControl(CLSID_WebBrowser);    // 相当于界面设计器里的Clsid属性里填入{8856F961-340A-11D0-A96B-00C04FD705A2}，建议用CLSID_WebBrowser，如果想看相应的值，请见<ExDisp.h>
		pActiveXUI->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);

		if (pWebBrowser != NULL)
		{
			std::string gourl_ = "http://news.baidu.com/";
			pWebBrowser->put_Silent(VARIANT_TRUE);
			BSTR url = _bstr_t(gourl_.c_str());;
			IConnectionPointContainer* pCPC = NULL;
			IConnectionPoint* pCP = NULL;
			pWebBrowser->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
			pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2, &pCP);
			DWORD dwCookie = 0;
			events = new CMyWebBrwoser2Event(NewWindow3t, pWebBrowser);
			pCP->Advise((IUnknown*)(void*)events, &dwCookie);
			pWebBrowser->Navigate(url, NULL, NULL, NULL, NULL);
			pWebBrowser->Release();
		}
	}

	WindowImplBase::InitWindow();   
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString  name = msg.pSender->GetName();

	if (msg.sType == _T("click")) {
		if (name == "BtnOneClick") {
			InstallStart();
			return;
		}
		else if (name == "btnAgain") {
			m_pStepTabLayout->SelectItem(0);
		}
	}
	else if (msg.sType == _T("showactivex"))
	{
		if (name.CompareNoCase(_T("ani_flash")) == 0) {
			CActiveXUI* pActiveX = static_cast<CActiveXUI*>(m_PaintManager.FindControl(_T("ani_flash")));		
			DuiLib::CDuiString path = CPaintManagerUI::GetInstancePath() + GetSkinFolder() + "\\waterdrop.swf";
			IShockwaveFlash* pFlash = NULL;
			pActiveX->GetControl(__uuidof(IShockwaveFlash), (void**)&pFlash);
			if (pFlash != NULL) {
				pFlash->put_WMode(_bstr_t(_T("Transparent"))); 
				pFlash->put_Movie(_bstr_t(path));
				pFlash->DisableLocalSecurity();
				pFlash->put_AllowScriptAccess(L"always");         
				BSTR response;
				pFlash->CallFunction(L"<invoke name=\"setButtonText\" returntype=\"xml\"><arguments><string>Click me!</string></arguments></invoke>", &response);
				pFlash->Release(); 
			}
		}
	}
	WindowImplBase::Notify(msg);
}
          
LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_TIMER) {
		if ( wParam == TIMER_ID_PROGRESS) {
			OnMyTimer();   
		}
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}


void CDuiFrameWnd::InstallStart() {
	m_pAgainBtn->SetVisible(false);
	m_pProgressBar->SetValue(0);
	m_pInstallText->SetText(_T("正在安装中，请稍候..."));

	m_pStepTabLayout->SelectItem(1);	
	SetTimer(GetHWND(), TIMER_ID_PROGRESS, 100, NULL);
}

void  CDuiFrameWnd::InstallFinished() {
	m_pInstallText->SetText(_T("安装完成！"));
	KillTimer(GetHWND(), TIMER_ID_PROGRESS);
	m_pAgainBtn->SetVisible(true);
}

void CDuiFrameWnd::OnMyTimer() {
	int nVal = m_pProgressBar->GetValue();
	CDuiString strPercent;
	strPercent.Format(_T("正在安装（%d%%）"), nVal);
	m_pInstallText->SetText(strPercent);
	if (nVal >= 99)
	{
		m_pProgressBar->SetValue(100);
		InstallFinished();
	}
	else
	{
		nVal += 1;
		m_pProgressBar->SetValue(nVal);
	}
}






int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	CoInitialize(NULL);

	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

	CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
	duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame->CenterWindow();

	// 设置icon
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	duiFrame->ShowModal();
	delete duiFrame;

	g_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	if (events)
		delete events;
	return 0;
}


