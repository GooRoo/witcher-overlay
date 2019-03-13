#include "Image.h"
#include "dx_utils.h"

Image::Image(Renderer *renderer, const std::string& file_path, int x, int y, int rotation, int align, bool bShow, int alpha)
	: RenderBase(renderer), m_pSprite(NULL), m_pTexture(NULL), m_alpha{alpha}
{
	setFilePath(file_path);
	setPos(x, y);
	setRotation(rotation);
	setAlign(align);
	setShown(bShow);
}

void Image::setFilePath(const std::string & path)
{
	m_filePath = path;
}

void Image::setPos(int x, int y)
{
	m_x = x, m_y = y;
}

void Image::setRotation(int rotation)
{
	m_rotation = rotation;
}

void Image::setAlign(int align)
{
	m_align = align;
}

void Image::setShown(bool show)
{
	m_bShow = show;
}

bool Image::updateImage(const std::string& file_path, int x, int y, int rotation, int align, bool bShow)
{
	setFilePath(file_path);
	setPos(x, y);
	setRotation(rotation);
	setAlign(align);
	setShown(bShow);

	changeResource();

	return true;
}

void Image::draw(IDirect3DDevice9 *pDevice)
{
	if(!m_bShow)
		return;

	int x = calculatedXPos(m_x);
	int y = calculatedYPos(m_y);

	if(m_pTexture && m_pSprite)
		Drawing::DrawSprite(m_pSprite, m_pTexture, x, y, m_rotation, m_align, m_alpha);
}

void Image::reset(IDirect3DDevice9 *pDevice)
{
	if(m_pSprite)
	{
		m_pSprite->OnLostDevice();
		m_pSprite->OnResetDevice();
	}
}


void Image::show()
{
	setShown(true);
}

void Image::hide()
{
	setShown(false);
}


void Image::releaseResourcesForDeletion(IDirect3DDevice9 *pDevice)
{
	if(m_pSprite)
	{
		m_pSprite->Release();
		m_pSprite = NULL;
	}

	if(m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
}

bool Image::canBeDeleted()
{
	return (m_pTexture == NULL && m_pSprite == NULL);
}

bool Image::loadResource(IDirect3DDevice9 *pDevice)
{
	if(m_pSprite)
	{
		m_pSprite->Release();
		m_pSprite = NULL;
	}

	if(m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}

	D3DXCreateTextureFromFileA(pDevice, m_filePath.c_str(), &m_pTexture);
	D3DXCreateSprite(pDevice, &m_pSprite);

	return (m_pTexture != NULL && m_pSprite != NULL);
}

void Image::firstDrawAfterReset(IDirect3DDevice9 *pDevice)
{

}