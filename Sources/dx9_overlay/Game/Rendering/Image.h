#pragma once
#include <d3dx9.h>

#include "RenderBase.h"

class Image : public RenderBase
{
public:
	static void DrawSprite(LPD3DXSPRITE SpriteInterface, LPDIRECT3DTEXTURE9 TextureInterface, int PosX, int PosY, int Rotation, int Align);

	Image(Renderer *renderer, const std::string& file_path, int x, int y, int rotation, int align, bool bShow, int alpha = 0xFF);

	void setFilePath(const std::string & path);
	void setPos(int x, int y);
	void setRotation(int rotation);
	void setAlign(int align);
	void setShown(bool show);
	bool updateImage(const std::string& file_path, int x, int y, int rotation, int align, bool bShow);

protected:
	virtual void draw(IDirect3DDevice9 *pDevice) final;
	virtual void reset(IDirect3DDevice9 *pDevice);

	virtual void show() sealed;
	virtual void hide() sealed;

	virtual void releaseResourcesForDeletion(IDirect3DDevice9 *pDevice);
	virtual bool canBeDeleted();

	virtual bool loadResource(IDirect3DDevice9 *pDevice) override;
	virtual void firstDrawAfterReset(IDirect3DDevice9 *pDevice) override sealed;

protected:
	LPDIRECT3DTEXTURE9 m_pTexture;
	LPD3DXSPRITE m_pSprite;
	int m_x, m_y;

private:
	std::string			m_filePath;

	bool m_bShow;
	int m_rotation, m_align;
	int m_alpha;
};
