//
// FGUI - feature rich graphical user interface
//

// library includes
#include "checkbox.hpp"

namespace FGUI
{

	CCheckBox::CCheckBox()
	{
		m_strTitle = "CheckBox";
		m_dmSize = { 16, 16 };
		m_anyFont = 0;
		m_strTooltip = "";
		m_bIsChecked = false;
		m_fnctCallback = nullptr;
		m_nType = static_cast<int>(WIDGET_TYPE::CHECKBOX);
		m_nFlags = static_cast<int>(WIDGET_FLAG::DRAWABLE) | static_cast<int>(WIDGET_FLAG::CLICKABLE) | static_cast<int>(WIDGET_FLAG::SAVABLE);
	}

	void CCheckBox::SetState(bool onoff)
	{
		m_bIsChecked = onoff;
	}

	bool CCheckBox::GetState()
	{
		return m_bIsChecked;
	}

	void CCheckBox::AddCallback(std::function<void()> callback)
	{
		m_fnctCallback = callback;
	}

	void CCheckBox::Geometry(FGUI::WIDGET_STATUS status)
	{
		// Update our bound data
		if (m_eBindType == WIDGET_BIND_TYPE::BOOL) {
			m_bIsChecked = *static_cast<bool*>(m_bBoundData);
		}
		
		FGUI::AREA arWidgetRegion = { GetAbsolutePosition().m_iX, GetAbsolutePosition().m_iY, m_dmSize.m_iWidth, m_dmSize.m_iHeight };

		// checkbox body
		FGUI::RENDER.Outline(arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop, arWidgetRegion.m_iRight, arWidgetRegion.m_iBottom, COLOR_CCHECKBOX_BORDER);
		FGUI::RENDER.Rectangle(arWidgetRegion.m_iLeft + 1, arWidgetRegion.m_iTop + 1, (arWidgetRegion.m_iRight - 2), (arWidgetRegion.m_iBottom - 2), COLOR_CCHECKBOX_BG);

		if (m_bIsChecked)
		{
			if (status == FGUI::WIDGET_STATUS::HOVERED)
			{
				FGUI::RENDER.Outline(arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop, arWidgetRegion.m_iRight, arWidgetRegion.m_iBottom, { 255, 40, 40 });
			}
			 
			FGUI::RENDER.Rectangle((arWidgetRegion.m_iLeft + 6), (arWidgetRegion.m_iTop + 6), (arWidgetRegion.m_iRight - 12), (arWidgetRegion.m_iBottom - 12), { 255, 40, 40 });
		}
		else
		{
			if (status == FGUI::WIDGET_STATUS::HOVERED)
			{
				FGUI::RENDER.Outline(arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop, arWidgetRegion.m_iRight, arWidgetRegion.m_iBottom, COLOR_CCHECKBOX_BG_HOVER);
			}
		}

		// checkbox label
		// Get the size of the button label
		FGUI::DIMENSION dmTitleTextSize = FGUI::RENDER.GetTextSize(m_anyFont, m_strTitle);
		// Draw the button label
		const auto label_x = arWidgetRegion.m_iLeft + arWidgetRegion.m_iRight + 8;
		const auto label_y = arWidgetRegion.m_iTop + (arWidgetRegion.m_iBottom / 2) - (dmTitleTextSize.m_iHeight / 2);
		FGUI::RENDER.Text(label_x, label_y, m_anyFont, COLOR_CCHECKBOX_LABEL, m_strTitle);
	}

	void CCheckBox::Update()
	{
		m_dmSize.m_iWidth = m_dmSize.m_iHeight;
		//m_dmSize = { 16, 16 }; // this is required to keep the widget from being padded on Containers

		if (m_fnctCallback)
		{
			// call function
			m_fnctCallback();
		}
	}

	void CCheckBox::Input()
	{
		m_bIsChecked = !m_bIsChecked;

		// Update our bound data
		if (m_eBindType == WIDGET_BIND_TYPE::BOOL) {
			*static_cast<bool*>(m_bBoundData) = m_bIsChecked;
		}
	}

	void CCheckBox::Save(nlohmann::json& module)
	{
		// remove spaces from widget name
		std::string strFormatedWidgetName = GetTitle();
		std::replace(strFormatedWidgetName.begin(), strFormatedWidgetName.end(), ' ', '_');

		module[strFormatedWidgetName] = m_bIsChecked;
	}

	void CCheckBox::Load(nlohmann::json& module)
	{
		// remove spaces from widget name
		std::string strFormatedWidgetName = GetTitle();
		std::replace(strFormatedWidgetName.begin(), strFormatedWidgetName.end(), ' ', '_');

		// change widget state to the one stored on file
		if (module.contains(strFormatedWidgetName))
		{
			m_bIsChecked = module[strFormatedWidgetName];
		}
	}

	void CCheckBox::Tooltip()
	{
		if (m_strTooltip.length() > 1 && !m_bIsChecked)
		{
			FGUI::DIMENSION dmTooltipTextSize = FGUI::RENDER.GetTextSize(m_anyFont, m_strTooltip);

			FGUI::AREA arTooltipRegion = { (FGUI::INPUT.GetCursorPos().m_iX + 10), (FGUI::INPUT.GetCursorPos().m_iY + 10), (dmTooltipTextSize.m_iWidth + 10), (dmTooltipTextSize.m_iHeight + 10) };

			FGUI::RENDER.Outline(arTooltipRegion.m_iLeft, arTooltipRegion.m_iTop, arTooltipRegion.m_iRight, arTooltipRegion.m_iBottom, { 180, 95, 95 });
			FGUI::RENDER.Rectangle((arTooltipRegion.m_iLeft + 1), (arTooltipRegion.m_iTop + 1), (arTooltipRegion.m_iRight - 2), (arTooltipRegion.m_iBottom - 2), { 225, 90, 75 });
			FGUI::RENDER.Text(arTooltipRegion.m_iLeft + (arTooltipRegion.m_iRight / 2) - (dmTooltipTextSize.m_iWidth / 2),
				arTooltipRegion.m_iTop + (arTooltipRegion.m_iBottom / 2) - (dmTooltipTextSize.m_iHeight / 2), m_anyFont, { 245, 245, 245 }, m_strTooltip);
		}
	}

} // namespace FGUI