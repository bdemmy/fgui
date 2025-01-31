//
// FGUI - feature rich graphical user interface
//

// library includes
#include "listbox.hpp"
#include "container.hpp"

namespace FGUI
{
	CListBox::CListBox()
	{
		m_strTitle = "ListBox";
		m_anyFont = 0;
		m_iEntrySpacing = 20;
		m_dmSize = { 250, 300 };
		m_ullSelectedEntry = 0;
		m_iScrollThumbPosition = 0;
		m_fnctCallback = nullptr;
		m_bIsDraggingThumb = false;
		m_prgpEntries = {};
		m_strTooltip = "";
		m_nType = static_cast<int>(WIDGET_TYPE::LISTBOX);
		m_nFlags = static_cast<int>(WIDGET_FLAG::DRAWABLE) | static_cast<int>(WIDGET_FLAG::CLICKABLE) | static_cast<int>(WIDGET_FLAG::SAVABLE);
	}

	void CListBox::SetIndex(std::size_t index)
	{
		m_ullSelectedEntry = index;
	}

	std::size_t CListBox::GetIndex()
	{
		return m_ullSelectedEntry;
	}

	int CListBox::GetValue()
	{
		if (m_ullSelectedEntry < m_prgpEntries.second.size() && m_ullSelectedEntry >= 0) {
			return m_prgpEntries.second[m_ullSelectedEntry];
		}
	}

	void CListBox::AddEntry(std::string name, unsigned int value)
	{
		m_prgpEntries.first.emplace_back(name);
		m_prgpEntries.second.emplace_back(value);
	}

	void CListBox::ClearEntries()
	{
		m_prgpEntries.first.clear();
		m_prgpEntries.second.clear();
		m_iScrollThumbPosition = 0;
	}

	void CListBox::AddCallback(std::function<void()> callback)
	{
		m_fnctCallback = callback;
	}

	void CListBox::Geometry(FGUI::WIDGET_STATUS status)
	{
		FGUI::AREA arWidgetRegion = { GetAbsolutePosition().m_iX, GetAbsolutePosition().m_iY, m_dmSize.m_iWidth, m_dmSize.m_iHeight };

		FGUI::AREA arScrollBarRegion = { (arWidgetRegion.m_iLeft + arWidgetRegion.m_iRight) - 15, arWidgetRegion.m_iTop, 15, m_dmSize.m_iHeight };

		FGUI::DIMENSION dmTitleTextSize = FGUI::RENDER.GetTextSize(m_anyFont, m_strTitle);

		// entries displayed
		int iEntriesDisplayed = 0;

		// calculate the amount of entries that will be drawn on the listbox
		int iCalculatedEntries = (m_dmSize.m_iHeight / m_iEntrySpacing);

		// listbox body
		FGUI::RENDER.Outline(arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop + (iEntriesDisplayed * m_iEntrySpacing), m_dmSize.m_iWidth, m_dmSize.m_iHeight, COLOR_CLISTBOX_BORDER);
		FGUI::RENDER.Rectangle((arWidgetRegion.m_iLeft + 1), (arWidgetRegion.m_iTop + (iEntriesDisplayed * m_iEntrySpacing)) + 1, (m_dmSize.m_iWidth - 2), (m_dmSize.m_iHeight - 2), COLOR_CLISTBOX_BG);

		// listbox label
		FGUI::RENDER.Text(arWidgetRegion.m_iLeft, (arWidgetRegion.m_iTop - dmTitleTextSize.m_iHeight) - 2, m_anyFont, COLOR_CLISTBOX_LABEL, m_strTitle);

		for (std::size_t i = m_iScrollThumbPosition; (i < m_prgpEntries.first.size()) && (iEntriesDisplayed < iCalculatedEntries); i++)
		{
			FGUI::AREA arEntryRegion = { arWidgetRegion.m_iLeft + 1, arWidgetRegion.m_iTop + (m_iEntrySpacing * iEntriesDisplayed) + 1, (arWidgetRegion.m_iRight - arScrollBarRegion.m_iRight) - 2, m_iEntrySpacing - 2 };

			// check if the user is hovering something on the listbox
			if (FGUI::INPUT.IsCursorInArea(arEntryRegion))
			{
				FGUI::RENDER.Outline(arEntryRegion.m_iLeft, arEntryRegion.m_iTop, arEntryRegion.m_iRight, arEntryRegion.m_iBottom, COLOR_CLISTBOX_SCROLL);
				FGUI::RENDER.Rectangle(arEntryRegion.m_iLeft + 1, arEntryRegion.m_iTop + 1, arEntryRegion.m_iRight - 2, arEntryRegion.m_iBottom - 2, COLOR_CLISTBOX_BORDER);
				FGUI::RENDER.Text((arEntryRegion.m_iLeft + 5), (arEntryRegion.m_iTop + 3), m_anyFont, COLOR_CLISTBOX_LABEL, m_prgpEntries.first[i]);
			}
			else
			{
				FGUI::RENDER.Rectangle(arEntryRegion.m_iLeft, (arEntryRegion.m_iTop + arEntryRegion.m_iBottom) - 1, arEntryRegion.m_iRight, 1, COLOR_CLISTBOX_BORDER);
				FGUI::RENDER.Text((arEntryRegion.m_iLeft + 5), (arEntryRegion.m_iTop + 3), m_anyFont, COLOR_CLISTBOX_LABEL, m_prgpEntries.first[i]);
			}

			// if the user selects an entry on the listbox
			if (m_ullSelectedEntry == i)
			{
				FGUI::RENDER.Outline(arEntryRegion.m_iLeft, arEntryRegion.m_iTop, arEntryRegion.m_iRight, arEntryRegion.m_iBottom, COLOR_CLISTBOX_SCROLL);
				FGUI::RENDER.Rectangle(arEntryRegion.m_iLeft + 1, arEntryRegion.m_iTop + 1, arEntryRegion.m_iRight - 2, arEntryRegion.m_iBottom - 2, { 255, 40, 40 });
				FGUI::RENDER.Text((arEntryRegion.m_iLeft + 5), (arEntryRegion.m_iTop + 3), m_anyFont, COLOR_CLISTBOX_LABEL, m_prgpEntries.first[i]);
			}

			iEntriesDisplayed++;
		}

		static constexpr FGUI::DIMENSION dmScrollBarThumb = { 8, 5 };
		
		// calculate thumb size
		auto thumb_ratio = static_cast<float>(iCalculatedEntries) / static_cast<float>(m_prgpEntries.first.size());
		thumb_ratio = std::min(thumb_ratio, 1.f);
		const auto thumb_size = (m_dmSize.m_iHeight - 6) * thumb_ratio;

		// calculate thumb position
		const auto count_positions = m_prgpEntries.first.size() - iCalculatedEntries;
		const auto segment_size = (m_dmSize.m_iHeight - 6 - thumb_size) / count_positions;
		
		auto flCalculatedPosition = segment_size * m_iScrollThumbPosition;
		if (count_positions == 0) {
			flCalculatedPosition = 0;
		}

		// scrollbar body
		FGUI::RENDER.Rectangle(arScrollBarRegion.m_iLeft, arScrollBarRegion.m_iTop, arScrollBarRegion.m_iRight, arScrollBarRegion.m_iBottom, COLOR_CLISTBOX_BORDER);

		if (m_prgpEntries.first.size() > 50)
		{
			FGUI::RENDER.Rectangle((arScrollBarRegion.m_iLeft + 3), (arScrollBarRegion.m_iTop + flCalculatedPosition) + 3, dmScrollBarThumb.m_iWidth + 1, dmScrollBarThumb.m_iHeight, COLOR_CLISTBOX_SCROLL);
		}
		else
		{
			FGUI::RENDER.Rectangle((arScrollBarRegion.m_iLeft + 3), (arScrollBarRegion.m_iTop + flCalculatedPosition) + 3, dmScrollBarThumb.m_iWidth + 1, thumb_size, COLOR_CLISTBOX_SCROLL);
		}

		IGNORE_ARGS(status);
	}

	void CListBox::Update()
	{
		// calculate the amount of entries that will be drawn on the listbox
		int iCalculatedEntries = (m_dmSize.m_iHeight / m_iEntrySpacing);
		const int count_positions = m_prgpEntries.first.size() - iCalculatedEntries;

		FGUI::AREA arWidgetRegion = { GetAbsolutePosition().m_iX, GetAbsolutePosition().m_iY, m_dmSize.m_iWidth, m_dmSize.m_iHeight };
		if (FGUI::INPUT.IsCursorInArea(arWidgetRegion)) {
			const auto scroll = FGUI::INPUT.GetScroll();
			if (scroll != 0 && count_positions != 0) {
				m_iScrollThumbPosition += scroll;

				m_iScrollThumbPosition = std::clamp(m_iScrollThumbPosition, 0, count_positions);

				INPUT.SetScroll(0);
			}
		}

		if (m_bIsDraggingThumb)
		{
			FGUI::POINT ptCursorPos = FGUI::INPUT.GetCursorPos();

			if (FGUI::INPUT.IsKeyHeld(MOUSE_1))
			{
				// move thumb vertically
				ptCursorPos.m_iY -= GetAbsolutePosition().m_iY;

				// ratio of how many visible to how many are hidden
				float flVisibleRatio = static_cast<float>(iCalculatedEntries) / static_cast<float>(m_prgpEntries.first.size());
				flVisibleRatio *= (m_dmSize.m_iHeight - m_iEntrySpacing);

				// calculate height delta
				float flHeightDelta = static_cast<float>(ptCursorPos.m_iY + flVisibleRatio) - static_cast<float>(m_dmSize.m_iHeight);

				if (flHeightDelta >= 0.f)
				{
					ptCursorPos.m_iY -= flHeightDelta;
				}

				// new vertical position ratio
				float flNewYRatio = static_cast<float>(ptCursorPos.m_iY) / static_cast<float>(m_dmSize.m_iHeight);
				m_iScrollThumbPosition = (flNewYRatio * m_prgpEntries.first.size());

				// clamp position (don't let the user drag the scroll thumb if it reaches the "start" of the scrollbar area)
				if (m_iScrollThumbPosition <= 0)
				{
					m_iScrollThumbPosition = 0;
				}
			}
			else
			{
				m_bIsDraggingThumb = false;
			}
		}

		// stop scrolling if another widget is being focused
		if (std::reinterpret_pointer_cast<FGUI::CContainer>(GetParentWidget())->GetFocusedWidget())
		{
			m_bIsDraggingThumb = false;
		}
	}

	void CListBox::Input()
	{
		FGUI::AREA arWidgetRegion = { GetAbsolutePosition().m_iX, GetAbsolutePosition().m_iY, m_dmSize.m_iWidth, m_dmSize.m_iHeight };

		static constexpr FGUI::DIMENSION dmScrollBarThumb = { 8, 5 };

		FGUI::AREA arScrollBarRegion = { ((arWidgetRegion.m_iLeft + arWidgetRegion.m_iRight) - 15), arWidgetRegion.m_iTop, dmScrollBarThumb.m_iWidth, (m_dmSize.m_iHeight - m_iEntrySpacing) };

		if (FGUI::INPUT.IsCursorInArea(arScrollBarRegion))
		{
			m_bIsDraggingThumb = true;
		}

		// entries displayed
		int iEntriesDisplayed = 0;

		// calculate the amount of entries that will be drawn on the listbox
		int iCalculatedEntries = (m_dmSize.m_iHeight / m_iEntrySpacing);

		// listbox entries
		for (std::size_t i = m_iScrollThumbPosition; (i < m_prgpEntries.first.size()) && (iEntriesDisplayed < iCalculatedEntries); i++)
		{
			FGUI::AREA arEntryRegion = { arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop + (m_iEntrySpacing * iEntriesDisplayed), (arWidgetRegion.m_iRight - 15), m_iEntrySpacing };

			// select an entry
			if (FGUI::INPUT.IsCursorInArea(arEntryRegion))
			{
				m_ullSelectedEntry = i;
			}

			if (m_ullSelectedEntry == i)
			{
				if (m_fnctCallback)
				{
					// call function
					m_fnctCallback();
				}
			}

			iEntriesDisplayed++;
		}
	}

	void CListBox::Save(nlohmann::json& module)
	{
		// remove spaces from widget name
		std::string strFormatedWidgetName = GetTitle();
		std::replace(strFormatedWidgetName.begin(), strFormatedWidgetName.end(), ' ', '_');

		module[strFormatedWidgetName] = m_ullSelectedEntry;
	}

	void CListBox::Load(nlohmann::json& module)
	{
		// remove spaces from widget name
		std::string strFormatedWidgetName = GetTitle();
		std::replace(strFormatedWidgetName.begin(), strFormatedWidgetName.end(), ' ', '_');

		// change widget selected entry to the one stored on file
		if (module.contains(strFormatedWidgetName))
		{
			m_ullSelectedEntry = module[strFormatedWidgetName];
		}
	}

	void CListBox::Tooltip()
	{
		if (m_strTooltip.length() > 1)
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