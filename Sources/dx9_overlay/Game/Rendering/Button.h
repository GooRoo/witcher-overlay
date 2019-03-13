#pragma once
#include <d3dx9.h>

#include "Image.h"

class Button : public Image
{
public:
	Button(Renderer *renderer, const std::string& file_path, const std::string& hovered_file_path, int x, int y, int rotation, int align, bool bShow);

	bool checkMouse(int x, int y);
	void setHoveredFilePath(const std::string & hoveredFilePath);

protected:
	void reset(IDirect3DDevice9 *pDevice) override;
	
	virtual void toggleHovered();

	virtual void releaseResourcesForDeletion(IDirect3DDevice9 *pDevice) override;
	virtual bool canBeDeleted() override;

	virtual bool loadResource(IDirect3DDevice9 *pDevice) override;
private:
	std::string m_hoveredFilePath;
	bool m_isHovered;

	LPDIRECT3DTEXTURE9 m_pHoveredTexture;
	LPD3DXSPRITE m_pHoveredSprite;
};
