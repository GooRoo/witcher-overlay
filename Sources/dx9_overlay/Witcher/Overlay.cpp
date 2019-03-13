#include "Overlay.h"

#include "Game/Game.h"
#include "Game/Rendering/Renderer.h"

#include <string>
#include <Windowsx.h>

static const std::string sFontFamily = "Arial";
static const int sFontBaseSize = 12;

static const std::string sActiveText = "Ctrl+Space to disable overlay";
static const std::string sInactiveText = "Ctrl+Space to enable overlay";

Overlay::Overlay()
	: m_isActive{false}
	, m_isCtrlPressed{false}
{
	m_activationText = std::make_shared<Text>(&g_pRenderer, 
		sFontFamily, 
		sFontBaseSize,
		false,
		true, 
		800,
		600, 
		0xFFFFFFFF,
		m_isActive? sActiveText : sInactiveText, 
		false,
		2, // bottom-right
		true
	);

	m_captionText = std::make_shared<Text>(&g_pRenderer,
		sFontFamily,
		sFontBaseSize,
		true,
		false,
		400,
		100,
		0xFFFFFFFF,
		"Overlay Test Application",
		false,
		1, // centered
		m_isActive
	);

	m_button = std::make_shared<Button>(&g_pRenderer,
		m_assetsPath + "\\Assets\\buttonActive.png",
		m_assetsPath + "\\Assets\\buttonHovered.png",
		325,
		200,
		0,
		0,
		m_isActive
	);
	m_images.push_back(std::make_pair(m_button, "\\Assets\\buttonActive.png"));

	m_buttonCaption = std::make_shared<Text>(&g_pRenderer,
		sFontFamily,
		sFontBaseSize - 2,
		false,
		false,
		400,
		250,
		D3DCOLOR_RGBA(0, 0, 0, 0xFF),
		"Disable",
		false,
		1, // centered
		m_isActive
	);

	m_background = std::make_shared<Image>(&g_pRenderer,
		m_assetsPath + "\\Assets\\gog.png",
		0,
		0,
		0,
		1,
		m_isActive,
		0x80
	);
	m_images.push_back(std::make_pair(m_background, "\\Assets\\gog.png"));
}

void Overlay::init()
{
	g_pRenderer.add(m_background);
	g_pRenderer.add(m_captionText);
	g_pRenderer.add(m_activationText);
	g_pRenderer.add(m_button);
	g_pRenderer.add(m_buttonCaption);
	onActiveStateChanged(m_isActive);
}

bool Overlay::handleMessage(LPMSG lpMsg)
{
	switch (lpMsg->message)
	{
	case WM_KEYUP:
		if (lpMsg->wParam == VK_CONTROL)
			m_isCtrlPressed = false;
		else if (lpMsg->wParam == VK_SPACE)
		{
			if (m_isCtrlPressed)
			{
				toggleActive();
				return true;
			}
		}

		if (m_isActive)
			return true;
		break;

	case WM_KEYDOWN:
		if (lpMsg->wParam == VK_CONTROL)
			m_isCtrlPressed = true;

		if (m_isActive)
			return true;
		break;

	case WM_INPUT:
		if (m_isCtrlPressed)
		{
			bool skip = false;

			PRAWINPUT pRawInput = nullptr;
			UINT dataSize = 0, headerSize = sizeof(RAWINPUTHEADER);

			// get size of data
			GetRawInputData((HRAWINPUT)lpMsg->lParam, RID_INPUT, nullptr, &dataSize, headerSize);

			// alloc
			HANDLE hHeap = GetProcessHeap();
			pRawInput = (PRAWINPUT)HeapAlloc(hHeap, 0, dataSize);
			if (!pRawInput)
				return false;

			// read actual data
			GetRawInputData((HRAWINPUT)lpMsg->lParam, RID_INPUT, pRawInput, &dataSize, headerSize);

			if (pRawInput->data.keyboard.VKey == VK_SPACE)
				skip = true;

			HeapFree(hHeap, 0, pRawInput);
			return skip;
		}

		if (m_isActive)
			return true;
		break;

	case WM_MOUSEMOVE:
		if (m_isActive)
		{
			int xPos, yPos;
			xPos = GET_X_LPARAM(lpMsg->lParam);
			yPos = GET_Y_LPARAM(lpMsg->lParam);
			m_button->checkMouse(xPos, yPos);

			return true;
		}		
		break;

	case WM_LBUTTONUP:
		if (m_isActive)
		{
			int xPos, yPos;
			xPos = GET_X_LPARAM(lpMsg->lParam);
			yPos = GET_Y_LPARAM(lpMsg->lParam);
			if (m_button->checkMouse(xPos, yPos))
				toggleActive();

			return true;
		}
		break;

	case WM_CHAR:
	case WM_MOUSEHOVER:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_NCMOUSEMOVE:
	case WM_NCMOUSEHOVER:
		if (m_isActive)
			return true;
		break;
	}

	return false;
}

void Overlay::setActive(bool active)
{
	if (m_isActive != active)
	{
		m_isActive = active;
		onActiveStateChanged(m_isActive);
	}
}

void Overlay::setAssetsPath(const std::string & path)
{
	m_assetsPath = path;
	onAssetsPathChanged(m_assetsPath);
}

void Overlay::toggleActive()
{
	setActive(!m_isActive);
}

bool Overlay::isActive() const
{
	return m_isActive;
}

void Overlay::onActiveStateChanged(bool active)
{
	m_background->setShown(active);
	m_captionText->setShown(active);
	m_activationText->setText(active? sActiveText : sInactiveText);
	m_button->setShown(active);
	m_buttonCaption->setShown(active);

	ShowCursor(active);
	SetCursor(GetCursor()); // operation has no meaning in usual conditions, but we have a hook
}

void Overlay::onAssetsPathChanged(const std::string & newPath)
{
	for (const auto & it : m_images)
		it.first->setFilePath(m_assetsPath + it.second);

	m_button->setHoveredFilePath(m_assetsPath + "\\Assets\\buttonHovered.png");
}
