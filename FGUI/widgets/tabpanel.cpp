//
// FGUI - feature rich graphical user interface
//

// library includes
#include "tabpanel.hpp"
#include "container.hpp"

namespace FGUI
{

	CTabPanel::CTabPanel()
	{
		m_strTitle = "TabsPanel";
		m_anyFont = 0;
		m_ullSelectedEntry = 0;
		m_dmSize = { 110, 25 };
		m_prgpTabButtons = {};
		m_strTooltip = "";
		m_nStyle = static_cast<int>(TAB_STYLE::HORIZONTAL);
		m_nType = static_cast<int>(WIDGET_TYPE::TABPANEL);
		m_nFlags = static_cast<int>(WIDGET_FLAG::DRAWABLE) | static_cast<int>(WIDGET_FLAG::CLICKABLE);
	}

	void CTabPanel::AddTab(std::string title)
	{
		m_prgpTabButtons.emplace_back(title);
	}

	void CTabPanel::SetIndex(std::size_t index)
	{
		m_ullSelectedEntry = index;
	}

	std::size_t CTabPanel::GetIndex()
	{
		return m_ullSelectedEntry;
	}

	void CTabPanel::SetStyle(FGUI::TAB_STYLE style)
	{
		m_nStyle = static_cast<int>(style);
	}

	int CTabPanel::GetStyle()
	{
		return m_nStyle;
	}

	void CTabPanel::Geometry(FGUI::WIDGET_STATUS status)
	{
		// don't proceed if the tab container is empty
		if (m_prgpTabButtons.empty())
		{
			return;
		}

		FGUI::AREA arWidgetRegion = { 0, 0, 0, 0 };

		for (std::size_t i = 0; i < m_prgpTabButtons.size(); i++)
		{
			if (m_nStyle == static_cast<int>(TAB_STYLE::HORIZONTAL))
			{
				arWidgetRegion = { GetAbsolutePosition().m_iX + (static_cast<int>(i) * m_iEntrySpacing), GetAbsolutePosition().m_iY, m_iEntrySpacing, m_dmSize.m_iHeight };
				
				// Draw the rectangle using the color we determined
				FGUI::RENDER.Rectangle(arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop + m_dmSize.m_iHeight - 1, arWidgetRegion.m_iRight, 1, {15, 15, 15});

				arWidgetRegion.m_iBottom -= 1;
				
				// Get the color for the rectangle
				auto color = FGUI::COLOR(COLOR_CTABPANEL_BG);
				// If we are highlighting, change the color
				if (FGUI::INPUT.IsCursorInArea(arWidgetRegion)) {
					color = COLOR_CTABPANEL_BG_HOVER;
				}

				// Draw the rectangle using the color we determined
				FGUI::RENDER.Rectangle(arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop, arWidgetRegion.m_iRight, arWidgetRegion.m_iBottom, color);

				// Draw the highlight under the selected tab
				if (m_ullSelectedEntry == i)
				{
					FGUI::RENDER.Rectangle(
						arWidgetRegion.m_iLeft + (m_iEntrySpacing / 2) - (m_iEntrySpacing / 6),
						arWidgetRegion.m_iTop + m_dmSize.m_iHeight - 6,
						(m_iEntrySpacing / 3),
						2,
						COLOR_CTABPANEL_INDICATOR
					);
				}
				
				// Get the tab title size and position
				const auto textSize = FGUI::RENDER.GetTextSize(m_anyFont, m_prgpTabButtons[i]);
				const auto textX = (m_iEntrySpacing / 2) - (textSize.m_iWidth / 2);
				FGUI::RENDER.Text((arWidgetRegion.m_iLeft + textX), (arWidgetRegion.m_iTop + (arWidgetRegion.m_iBottom / 2) - (textSize.m_iHeight / 2)), m_anyFont, { 255, 255, 255 }, m_prgpTabButtons[i]);
			}

			else if (m_nStyle == static_cast<int>(TAB_STYLE::VERTICAL))
			{
				arWidgetRegion = { GetAbsolutePosition().m_iX, GetAbsolutePosition().m_iY + (static_cast<int>(i) * m_iEntrySpacing), m_dmSize.m_iWidth, m_iEntrySpacing };

				// Get the color for the rectangle
				auto color = FGUI::COLOR(COLOR_CTABPANEL_BG);
				// If we are highlighting, change the color
				if (FGUI::INPUT.IsCursorInArea(arWidgetRegion)) {
					color = COLOR_CTABPANEL_BG_HOVER;
				}

				// Draw the rectangle using the color we determined
				FGUI::RENDER.Rectangle(arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop, arWidgetRegion.m_iRight, arWidgetRegion.m_iBottom, color);

				// Draw the highlight under the selected tab
				if (m_ullSelectedEntry == i)
				{
					FGUI::RENDER.Rectangle(
						arWidgetRegion.m_iLeft + (m_dmSize.m_iWidth / 2) - (m_dmSize.m_iWidth / 6),
						arWidgetRegion.m_iTop + m_iEntrySpacing - 25,
						(m_dmSize.m_iWidth / 3),
						2,
						COLOR_CTABPANEL_INDICATOR
					);
				}

				// Get the tab title size and position
				const auto textSize = FGUI::RENDER.GetTextSize(m_anyFont, m_prgpTabButtons[i]);
				const auto textX = (m_dmSize.m_iWidth / 2) - (textSize.m_iWidth / 2);
				FGUI::RENDER.Text((arWidgetRegion.m_iLeft + textX), (arWidgetRegion.m_iTop + (arWidgetRegion.m_iBottom / 2) - (textSize.m_iHeight / 2)), m_anyFont, { 255, 255, 255 }, m_prgpTabButtons[i]);
			}
		}

		IGNORE_ARGS(status);
	}

	void CTabPanel::Update()
	{
		FGUI::AREA arWidgetRegion = { 0, 0, 0, 0 };

		if (m_nStyle == static_cast<int>(TAB_STYLE::HORIZONTAL)) {
			m_iEntrySpacing = m_dmSize.m_iWidth / m_prgpTabButtons.size();
		} else {
			m_iEntrySpacing = m_dmSize.m_iHeight / m_prgpTabButtons.size();
		}

		for (std::size_t i = 0; i < m_prgpTabButtons.size(); i++)
		{
			if (m_nStyle == static_cast<int>(TAB_STYLE::HORIZONTAL))
			{
				arWidgetRegion = {
					GetAbsolutePosition().m_iX + (static_cast<int>(i) * m_iEntrySpacing),
					GetAbsolutePosition().m_iY,
					m_dmSize.m_iWidth,
					m_dmSize.m_iHeight
				};
			}
			else if (m_nStyle == static_cast<int>(TAB_STYLE::VERTICAL))
			{
				arWidgetRegion = {
					GetAbsolutePosition().m_iX,
					GetAbsolutePosition().m_iY + (static_cast<int>(i) * m_iEntrySpacing),
					m_dmSize.m_iWidth,
					m_dmSize.m_iHeight
				};
			}

			if (FGUI::INPUT.IsCursorInArea(arWidgetRegion))
			{
				if (FGUI::INPUT.IsKeyPressed(MOUSE_1))
				{
					m_ullSelectedEntry = i;

					// loose focus if another tab is selected
					if (m_ullSelectedEntry != i)
					{
						std::reinterpret_pointer_cast<FGUI::CContainer>(GetParentWidget())->SetFocusedWidget(nullptr);
					}
				}
			}
		}
	}

	void CTabPanel::Input()
	{
	}

	void CTabPanel::Save(nlohmann::json& module)
	{
		IGNORE_ARGS(module);
	}

	void CTabPanel::Load(nlohmann::json& module)
	{
		IGNORE_ARGS(module);
	}

	void CTabPanel::Tooltip()
	{
	}

} // namespace FGUI