#include "Button.h"

Button::Button(Renderer* renderer, const std::string& file_path, const std::string& hovered_file_path, int x, int y, int rotation, int align, bool bShow)
	: Image{renderer, file_path, x, y, rotation, align, bShow}
	, m_hoveredFilePath(hovered_file_path)
	, m_isHovered{false}
	, m_pHoveredSprite{nullptr}
	, m_pHoveredTexture{nullptr}
{
	
}

bool Button::checkMouse(int x, int y)
{
	int bx = calculatedXPos(m_x);
	int by = calculatedYPos(m_y);

	D3DSURFACE_DESC desc;
	m_pHoveredTexture->GetLevelDesc(0, &desc);

	int bx2 = bx + desc.Width;
	int by2 = by + desc.Height;

	bool newHovered = (x > bx && y > by && x < bx2 && y < by2);
	if (newHovered != m_isHovered)
		toggleHovered();

	return newHovered;
}

void Button::setHoveredFilePath(const std::string& hoveredFilePath)
{
	m_hoveredFilePath = hoveredFilePath;
	changeResource();
}

void Button::toggleHovered()
{
	m_isHovered = !m_isHovered;

	std::swap(m_pSprite, m_pHoveredSprite);
	std::swap(m_pTexture, m_pHoveredTexture);
}

void Button::releaseResourcesForDeletion(IDirect3DDevice9* pDevice)
{
	if (m_pHoveredSprite)
	{
		m_pHoveredSprite->Release();
		m_pHoveredSprite = nullptr;
	}

	if (m_pHoveredTexture)
	{
		m_pHoveredTexture->Release();
		m_pHoveredTexture = nullptr;
	}
	Image::releaseResourcesForDeletion(pDevice);
}

bool Button::canBeDeleted()
{
	return (m_pHoveredTexture == nullptr && m_pHoveredSprite == nullptr && Image::canBeDeleted());
}

bool Button::loadResource(IDirect3DDevice9* pDevice)
{
	if (m_pHoveredSprite)
	{
		m_pHoveredSprite->Release();
		m_pHoveredSprite = nullptr;
	}

	if (m_pHoveredTexture)
	{
		m_pHoveredTexture->Release();
		m_pHoveredTexture = nullptr;
	}

	D3DXCreateTextureFromFileA(pDevice, m_hoveredFilePath.c_str(), &m_pHoveredTexture);
	D3DXCreateSprite(pDevice, &m_pHoveredSprite);

	return (m_pHoveredTexture != nullptr && m_pHoveredSprite != nullptr && Image::loadResource(pDevice));

}

void Button::reset(IDirect3DDevice9* pDevice)
{
	if (m_pHoveredSprite)
	{
		m_pHoveredSprite->OnLostDevice();
		m_pHoveredSprite->OnResetDevice();
	}
	Image::reset(pDevice);
}
