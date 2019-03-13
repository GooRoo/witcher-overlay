#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Game/Rendering/Text.h"
#include "Game/Rendering/Image.h"
#include <Game/Rendering/Button.h>

class Overlay
{
public:
	Overlay();
	virtual ~Overlay() = default;

	void init(); // HACK: don't like two-steps initialization

	//! @return true if message was processed by overlay, false otherwise
	bool handleMessage(LPMSG lpMsg);

	void toggleActive();
	bool isActive() const;

	//! Set directory with image assets
	void setAssetsPath(const std::string & path);

protected:
	virtual void onActiveStateChanged(bool active);
	virtual void onAssetsPathChanged(const std::string & newPath);

private:
	void setActive(bool active);

private:
	std::shared_ptr<Text> m_activationText;
	std::shared_ptr<Text> m_captionText;
	std::shared_ptr<Image> m_background;

	// TODO: Obviously, button should be encapsulated in one single class, but maybe next time...
	std::shared_ptr<Button> m_button;
	std::shared_ptr<Text> m_buttonCaption;

	// almost useless currently. Should be refactored
	std::vector<std::pair<std::shared_ptr<Image>, std::string>> m_images;

	bool m_isActive;
	bool m_isCtrlPressed;

	std::string m_assetsPath;
};
