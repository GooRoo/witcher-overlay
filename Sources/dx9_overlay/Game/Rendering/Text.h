#pragma once
#include <string>
#include <memory>
#include <d3dx9.h>

#include "D3DFont.h"
#include "RenderBase.h"

class Text : public RenderBase
{
public:
	Text(Renderer *renderer, const std::string& font, int iFontSize, bool Bold, bool Italic, int x, int y, D3DCOLOR color, const std::string& text, bool bShadow, int align, bool bShow);

	bool updateText(const std::string& Font, int FontSize, bool Bold, bool Italic);
	void setText(const std::string& str);
	void setColor(D3DCOLOR color);
	void setPos(int x,int y);
	void setShown(bool bShow);
	void setShadow(bool bShadow);
	void setAlignment(int align);

protected:
	virtual void draw(IDirect3DDevice9 *pDevice) sealed;
	virtual void reset(IDirect3DDevice9 *pDevice) sealed;

	virtual void show() override sealed;
	virtual void hide() override sealed;

	virtual void releaseResourcesForDeletion(IDirect3DDevice9 *pDevice) override sealed;
	virtual bool canBeDeleted() override sealed;

	virtual bool loadResource(IDirect3DDevice9 *pDevice) override sealed;
	virtual void firstDrawAfterReset(IDirect3DDevice9 *pDevice) override sealed;

private:
	std::string	m_Text, m_Font;
	int	m_X, m_Y, m_FontSize;
	D3DCOLOR m_Color;
	std::shared_ptr<CD3DFont> m_D3DFont;
	bool m_bShown, m_bShadow, m_bItalic, m_bBold;
	int m_align;

	void initFont(IDirect3DDevice9 *pDevice);
	void resetFont();
	bool drawText(int x, int y, DWORD dwColor, const std::string& strText, DWORD dwFlags = 0L);
};

