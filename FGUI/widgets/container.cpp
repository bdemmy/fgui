//
// FGUI - feature rich graphical user interface
//

// library includes
#include "container.hpp"
#include "checkbox.hpp"
#include "../../examples/d3d9/renderer.hpp"

namespace FGUI
{
	void CContainer::SetBorder(bool border) {
		m_bDrawBorder = false;
	}

	CContainer::CContainer()
	{
		m_strTitle = "";
		m_strTooltip = "";
		m_bScrollBarState = false;
		m_bIsOpened = false;
		m_bDrawBorder = true;
		m_uiKey = 0;
		m_anyFont = 0;
		m_iWidgetScrollOffset = 0;
		m_fnctCallback = nullptr;
		m_bIsFocusingOnWidget = false;
		m_pParentWidget = nullptr;
		m_pFocusedWidget = nullptr;
		m_nCursorStyle = static_cast<int>(CURSOR_STYLE::ARROW);
		m_nType = static_cast<int>(WIDGET_TYPE::CONTAINER);
		m_nFlags = static_cast<int>(WIDGET_FLAG::DRAWABLE) | static_cast<int>(WIDGET_FLAG::DRAW_FIRST);
	}

	void CContainer::Render()
	{
		// listen for input
		FGUI::INPUT.PullInput();

		if (!GetParentWidget())
		{
			if (FGUI::INPUT.IsKeyPressed(GetKey()))
			{
				SetState(!GetState());
			}

			if (GetState())
			{
				Update();
				Geometry(FGUI::WIDGET_STATUS::NONE);
				Tooltip();
				//Cursor();
			}
		}
	}

	void CContainer::SaveToFile(std::string file)
	{
		nlohmann::json jsModule;

		if (m_prgpWidgets.empty())
		{
			return;
		}

		for (std::shared_ptr<FGUI::CWidgets> pWidgets : m_prgpWidgets)
		{
			// save widget state
			pWidgets->Save(jsModule);
		}

		std::ofstream ofsFileToSave(file);

		if (ofsFileToSave.fail())
		{
			return; // TODO: handle this properly
		}

		// write the file
		ofsFileToSave << std::setw(4) << jsModule << std::endl;
	}

	void CContainer::LoadFromFile(std::string file)
	{
		nlohmann::json jsModule;

		if (m_prgpWidgets.empty())
		{
			return;
		}

		std::ifstream ifsFileToLoad(file, std::ifstream::binary);

		if (ifsFileToLoad.fail())
		{
			return; // TODO: handle this properly
		}

		jsModule = nlohmann::json::parse(ifsFileToLoad);

		for (std::shared_ptr<FGUI::CWidgets> pWidgets : m_prgpWidgets)
		{
			pWidgets->Load(jsModule);
		}
	}

	void CContainer::SetState(bool state)
	{
		if (GetParentWidget()) {
			m_bIsOpened = state;
			return;
		}

		last_time = high_resolution_clock::now();
		if (!m_bIsOpened) {
			m_bIsOpened = true;
		}
		fading_out = !fading_out;
	}

	bool CContainer::GetState()
	{
		return m_bIsOpened;
	}

	void CContainer::SetScrollBarState(bool state)
	{
		m_bScrollBarState = state;
	}

	bool CContainer::GetScrollBarState()
	{
		return m_bScrollBarState;
	}

	int CContainer::GetScrollOffset()
	{
		return m_iWidgetScrollOffset;
	}

	void CContainer::SetFocusedWidget(std::shared_ptr<FGUI::CWidgets> widget)
	{
		if (widget)
		{
			m_pFocusedWidget = widget;

			if (m_pFocusedWidget)
			{
				m_bIsFocusingOnWidget = true;
			}
		}
		else
		{
			m_pFocusedWidget.reset();

			if (!m_pFocusedWidget)
			{
				m_bIsFocusingOnWidget = false;
			}
		}
	}

	std::shared_ptr<FGUI::CWidgets> CContainer::GetFocusedWidget()
	{
		return m_pFocusedWidget;
	}

	void CContainer::AddCallback(std::function<void()> callback)
	{
		m_fnctCallback = callback;
	}

	void CContainer::SetKey(unsigned int key)
	{
		m_uiKey = key;
	}

	unsigned int CContainer::GetKey()
	{
		return m_uiKey;
	}

	void CContainer::AddWidget(std::shared_ptr<FGUI::CWidgets> widget, bool padding)
	{
		// set the parent widget
		widget->SetParentWidget(shared_from_this());

		// configure padding
		if (padding)
		{
			static constexpr int iScrollBarWidth = 15;

			if (GetParentWidget())
			{
				if (m_bScrollBarState)
				{
					widget->SetSize(m_dmSize.m_iWidth - (widget->GetPosition().m_iX * 2) - iScrollBarWidth, widget->GetSize().m_iHeight);
				}
				else
				{
					widget->SetSize(m_dmSize.m_iWidth - (widget->GetPosition().m_iX * 2), widget->GetSize().m_iHeight);
				}
			}
			else
			{
				widget->SetSize(m_dmSize.m_iWidth - (widget->GetPosition().m_iX * 2), widget->GetSize().m_iHeight);
			}
		}

		// pupulate container
		m_prgpWidgets.emplace_back(widget);

		// re-sort widgets
		if (widget->GetFlags(WIDGET_FLAG::DRAW_FIRST) && widget->GetType() == static_cast<int>(WIDGET_TYPE::CONTAINER))
		{
			// check if the widget is a groupbox Container
			if (widget->GetParentWidget())
			{
				m_prgpWidgets.erase(std::remove(m_prgpWidgets.begin(), m_prgpWidgets.end(), widget), m_prgpWidgets.end());
				m_prgpWidgets.insert(m_prgpWidgets.begin(), widget);
			}
		}
	}

	void CContainer::SetCursor(FGUI::CURSOR_STYLE style)
	{
		m_nCursorStyle = static_cast<int>(style);
	}

	void CContainer::Geometry(FGUI::WIDGET_STATUS status)
	{
		float oldAlpha = 255;
		if (!GetParentWidget()) {
			// Set the global alpha
			oldAlpha = FGUI::RENDER.GetAlpha();
			FGUI::RENDER.SetAlpha(m_flAlpha);
		}
		
		FGUI::AREA arWidgetRegion = { GetAbsolutePosition().m_iX, GetAbsolutePosition().m_iY, m_dmSize.m_iWidth, m_dmSize.m_iHeight };

		FGUI::DIMENSION dmTitleTextSize = FGUI::RENDER.GetTextSize(m_anyFont, m_strTitle);

		// if the container doesn't have a parent widget, it will behave like a normal window
		if (!GetParentWidget())
		{
			staticHue += rainbowSpeed;
			if (staticHue > 1.f) {
				staticHue = 0;
			}

			auto hue2 = staticHue + 0.5f;
			if (hue2 > 1.f) {
				hue2 -= 1.f;
			}

			const auto col1 = FGUI::COLOR::HSBToRGB(staticHue, 1.f, 1.f);
			const auto col2 = FGUI::COLOR::HSBToRGB(hue2, 1.f, 1.f);

			FGUI::RENDER.Gradient(arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop, arWidgetRegion.m_iRight, RAINBOW_SIZE, col1, col2, true);
			auto vert_offset = RAINBOW_SIZE;
			
			// Container background (including title bar)
			FGUI::RENDER.Rectangle(arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop + vert_offset, arWidgetRegion.m_iRight, arWidgetRegion.m_iBottom - vert_offset, COLOR_CCONTAINER_TITLE_BG);
			vert_offset += TITLE_HEIGHT;

			// Container inner body
			FGUI::RENDER.Rectangle(arWidgetRegion.m_iLeft + 1, arWidgetRegion.m_iTop + vert_offset, arWidgetRegion.m_iRight - 2, arWidgetRegion.m_iBottom - vert_offset - 1, COLOR_CCONTAINER_BG);

			// container title
			const auto text_x = m_ptPosition.m_iX + (m_dmSize.m_iWidth / 2) - (dmTitleTextSize.m_iWidth / 2);
			const auto text_y = m_ptPosition.m_iY + ((TITLE_HEIGHT + 2) / 2) - (dmTitleTextSize.m_iHeight / 2);
			FGUI::RENDER.Text(text_x, text_y, m_anyFont, { 255, 255, 255 }, m_strTitle);

			// if the container has a function
			if (m_fnctCallback)
			{
				// invoke function
				m_fnctCallback();
			}

			// Render icon in top left
			// TODO

			// Render close circle in top right
			const auto close_button_x = arWidgetRegion.m_iLeft + arWidgetRegion.m_iRight - (TITLE_HEIGHT / 2) - CLOSE_BUTTON_RADIUS;
			const auto close_button_y = arWidgetRegion.m_iTop + (TITLE_HEIGHT / 2) - CLOSE_BUTTON_RADIUS + RAINBOW_SIZE;
			const auto close_button_w = CLOSE_BUTTON_RADIUS * 2;
			
			FGUI_D3D9::Rectangle(close_button_x,
				close_button_y,
				close_button_w,
				close_button_w,
				COLOR3
			);

			if (FGUI::INPUT.IsCursorInArea({close_button_x, close_button_y, close_button_w, close_button_w})) {
				FGUI_D3D9::Rectangle(close_button_x + 1,
					close_button_y + 1,
					close_button_w - 2,
					close_button_w - 2,
					ACCENT
				);
			} else {
				FGUI_D3D9::Rectangle(close_button_x + 1,
					close_button_y + 1,
					close_button_w - 2,
					close_button_w - 2,
					COLOR2
				);
			}
		}
		
		else // otherwise, behave like a normal groupbox
		{
			if (m_bDrawBorder) {// groupbox body
				if (m_strTitle.length() > 0)
				{
					FGUI::RENDER.Rectangle(arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop, 5, 1, COLOR_CCONTAINER_BORDER);                                                                                           // top1
					FGUI::RENDER.Rectangle((arWidgetRegion.m_iLeft + dmTitleTextSize.m_iWidth) + 10, arWidgetRegion.m_iTop, (arWidgetRegion.m_iRight - dmTitleTextSize.m_iWidth) - 10, 1, COLOR_CCONTAINER_BORDER); // top2
				}
				else
				{
					FGUI::RENDER.Rectangle(arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop, arWidgetRegion.m_iRight, 1, COLOR_CCONTAINER_BORDER); // top1
				}

				FGUI::RENDER.Rectangle(arWidgetRegion.m_iLeft, arWidgetRegion.m_iTop, 1, arWidgetRegion.m_iBottom, COLOR_CCONTAINER_BORDER);                                               // left
				FGUI::RENDER.Rectangle((arWidgetRegion.m_iLeft + arWidgetRegion.m_iRight) - 1, arWidgetRegion.m_iTop, 1, arWidgetRegion.m_iBottom, COLOR_CCONTAINER_BORDER);                   // right
				FGUI::RENDER.Rectangle(arWidgetRegion.m_iLeft, (arWidgetRegion.m_iTop + arWidgetRegion.m_iBottom), arWidgetRegion.m_iRight, 1, COLOR_CCONTAINER_BORDER);                   // bottom
				FGUI::RENDER.Rectangle((arWidgetRegion.m_iLeft + 1), (arWidgetRegion.m_iTop + 1), (arWidgetRegion.m_iRight - 2), (arWidgetRegion.m_iBottom - 2), COLOR_CCONTAINER_BG); // background
			}

			// groupbox label
			if (m_strTitle.length() > 0)
			{
				FGUI::RENDER.Text((arWidgetRegion.m_iLeft + 10), arWidgetRegion.m_iTop - (dmTitleTextSize.m_iHeight / 2), m_anyFont, COLOR_CCONTAINER_TEXT, m_strTitle);
			}

			if (m_bScrollBarState)
			{
				FGUI::AREA arScrollBarRegion = { (arWidgetRegion.m_iLeft + arWidgetRegion.m_iRight) - 15, arWidgetRegion.m_iTop, 15, m_dmSize.m_iHeight };

				static constexpr FGUI::DIMENSION dmScrollBarThumbWidth = { 8, 5 };

				// scrollbar thumb size
				float flScrollbarThumbSize = ((m_dmSize.m_iHeight - m_prgpWidgets.back()->GetSize().m_iHeight) /
					static_cast<float>(m_prgpWidgets.back()->GetPosition().m_iY)) *
					static_cast<float>((m_dmSize.m_iHeight - m_prgpWidgets.back()->GetSize().m_iHeight));

				// calculate the scrollbar thumb position
				float flScrollbarThumbPosition = ((m_dmSize.m_iHeight - 10) - flScrollbarThumbSize) * static_cast<float>(m_iWidgetScrollOffset /
					static_cast<float>((m_prgpWidgets.back()->GetPosition().m_iY + m_prgpWidgets.back()->GetSize().m_iHeight) - (m_dmSize.m_iHeight - 10)));

				// scrollbar body
				FGUI::RENDER.Rectangle(arScrollBarRegion.m_iLeft, arScrollBarRegion.m_iTop, arScrollBarRegion.m_iRight, arScrollBarRegion.m_iBottom, { 235, 235, 235 });

				// scrollbar thumb
				FGUI::RENDER.Rectangle((arScrollBarRegion.m_iLeft + 4), (arScrollBarRegion.m_iTop + flScrollbarThumbPosition) + 5, dmScrollBarThumbWidth.m_iWidth, flScrollbarThumbSize, { 220, 223, 231 });
			}
		}

		// this will tell the container to skip focused widgets (so it can be drawned after all other widgets)
		bool bSkipWidget = false;

		// this will hold the current skipped widget
		std::shared_ptr<FGUI::CWidgets> pWidgetToSkip = nullptr;

		if (m_bIsFocusingOnWidget)
		{
			if (m_pFocusedWidget)
			{
				// set the widget that will be skipped
				pWidgetToSkip = m_pFocusedWidget;

				// tell the container to skip this widget
				bSkipWidget = true;
			}
		}

		// iterate over the rest of the widgets
		for (std::shared_ptr<FGUI::CWidgets> pWidgets : m_prgpWidgets)
		{
			// if a widget is currently being skipped
			if (bSkipWidget)
			{
				// we don't want to draw the skipped widget here
				if (pWidgetToSkip == pWidgets)
				{
					continue;
				}
			}

			// check if widgets are unlocked
			if (pWidgets && pWidgets->IsUnlocked() && pWidgets->GetFlags(WIDGET_FLAG::DRAWABLE))
			{
				// tell if the widget is being hovered or not
				FGUI::WIDGET_STATUS wsWidgetStatus = FGUI::WIDGET_STATUS::NONE;

				FGUI::AREA arWidgetsRegion = { pWidgets->GetAbsolutePosition().m_iX, pWidgets->GetAbsolutePosition().m_iY, pWidgets->GetSize().m_iWidth, pWidgets->GetSize().m_iHeight };

				if (m_bScrollBarState)
				{
					// check if the widgets are inside the boundaries of the groupbox
					if ((pWidgets->GetAbsolutePosition().m_iY + pWidgets->GetSize().m_iHeight) <= (GetAbsolutePosition().m_iY + GetSize().m_iHeight) && (pWidgets->GetAbsolutePosition().m_iY >= GetAbsolutePosition().m_iY))
					{
						if (FGUI::INPUT.IsCursorInArea(arWidgetsRegion) && !GetFocusedWidget())
						{
							wsWidgetStatus = FGUI::WIDGET_STATUS::HOVERED;
						}

						pWidgets->Geometry(wsWidgetStatus);
					}
				}
				else
				{
					if (FGUI::INPUT.IsCursorInArea(arWidgetsRegion) && !GetFocusedWidget())
					{
						wsWidgetStatus = FGUI::WIDGET_STATUS::HOVERED;
					}

					pWidgets->Geometry(wsWidgetStatus);
				}
			}
		}

		// now the container can draw skipped widgets
		if (bSkipWidget)
		{
			// check if the skipped widget can be drawned
			if (pWidgetToSkip && pWidgetToSkip->IsUnlocked() && pWidgetToSkip->GetFlags(WIDGET_FLAG::DRAWABLE))
			{
				// tell if the widget is being hovered or not
				FGUI::WIDGET_STATUS wsSkippedWidgetStatus = FGUI::WIDGET_STATUS::NONE;

				FGUI::AREA arSkippedWidgetRegion = { pWidgetToSkip->GetAbsolutePosition().m_iX, pWidgetToSkip->GetAbsolutePosition().m_iY, pWidgetToSkip->GetSize().m_iWidth, pWidgetToSkip->GetSize().m_iHeight };

				if (m_bScrollBarState)
				{
					// check if the widgets are inside the boundaries of the groupbox
					if ((pWidgetToSkip->GetAbsolutePosition().m_iY + pWidgetToSkip->GetSize().m_iHeight) <= (GetAbsolutePosition().m_iY + GetSize().m_iHeight) && (pWidgetToSkip->GetAbsolutePosition().m_iY >= GetAbsolutePosition().m_iY))
					{
						if (FGUI::INPUT.IsCursorInArea(arSkippedWidgetRegion))
						{
							wsSkippedWidgetStatus = FGUI::WIDGET_STATUS::HOVERED;
						}

						pWidgetToSkip->Geometry(wsSkippedWidgetStatus);
					}
				}
				else
				{
					if (FGUI::INPUT.IsCursorInArea(arSkippedWidgetRegion))
					{
						wsSkippedWidgetStatus = FGUI::WIDGET_STATUS::HOVERED;
					}

					pWidgetToSkip->Geometry(wsSkippedWidgetStatus);
				}
			}
		}

		if (!GetParentWidget()) {
			FGUI::RENDER.SetAlpha(oldAlpha);
		}
		
		IGNORE_ARGS(status);
	}

	void CContainer::Update()
	{
		const auto now = high_resolution_clock::now();
		const auto duration = duration_cast<milliseconds>(now - last_time).count();
		
		auto delta = duration / MENU_FADE_DURATION;
		if (delta > 1) {
			delta = 1;
		}
		auto alpha = delta * 255.f;

		if (fading_out) {
			alpha = 255.f - alpha;

			if (alpha <= 0.01) {
				m_bIsOpened = false;
			}
		}

		m_flAlpha = alpha;
		
		// reset cursor
		SetCursor(CURSOR_STYLE::ARROW);

		// check if the container is behaving like a window
		if (!GetParentWidget())
		{
			if (GetFlags(WIDGET_FLAG::FULLSCREEN))
			{
				// change container size
				SetSize(FGUI::RENDER.GetScreenSize());
			}

			FGUI::AREA arDraggableArea = { m_ptPosition.m_iX, m_ptPosition.m_iY, m_dmSize.m_iWidth, 20 };
			FGUI::AREA arClosebuttonArea = {
				m_ptPosition.m_iX + m_dmSize.m_iWidth - (TITLE_HEIGHT / 2) - CLOSE_BUTTON_RADIUS,
				m_ptPosition.m_iY + (TITLE_HEIGHT / 2) - CLOSE_BUTTON_RADIUS + RAINBOW_SIZE,
				2 * CLOSE_BUTTON_RADIUS,
				2 * CLOSE_BUTTON_RADIUS
			};

			static bool bIsDraggingContainer = false;

			// See if the user pressed the close button
			if (FGUI::INPUT.IsCursorInArea(arClosebuttonArea)) {
				if (FGUI::INPUT.IsKeyReleased(MOUSE_1))
				{
					m_bIsOpened = false;
					bIsDraggingContainer = false;
					return;
				}
			}

			// See if the user should be dragging
			if (FGUI::INPUT.IsCursorInArea(arDraggableArea))
			{
				if (FGUI::INPUT.IsKeyHeld(MOUSE_1))
				{
					bIsDraggingContainer = true;
				}
			}

			// if the user started dragging the container
			if (bIsDraggingContainer && m_draggable)
			{
				FGUI::POINT ptCursorPosDelta = FGUI::INPUT.GetCursorPosDelta();

				// move container
				m_ptPosition.m_iX += ptCursorPosDelta.m_iX;
				m_ptPosition.m_iY += ptCursorPosDelta.m_iY;

				if (GetFlags(WIDGET_FLAG::LIMIT))
				{
					m_ptPosition.m_iX = std::clamp(m_ptPosition.m_iX, 0, (FGUI::RENDER.GetScreenSize().m_iWidth - m_dmSize.m_iWidth));
					m_ptPosition.m_iY = std::clamp(m_ptPosition.m_iY, 0, (FGUI::RENDER.GetScreenSize().m_iHeight - m_dmSize.m_iHeight));
				}

				// change cursor
				SetCursor(CURSOR_STYLE::MOVE);
			}

			if (FGUI::INPUT.IsKeyReleased(MOUSE_1))
			{
				bIsDraggingContainer = false;
			}
		}
		else // if the container is behaving like a groupbox
		{
			if (m_bScrollBarState)
			{
				static bool bIsDraggingThumb = false;

				FGUI::AREA arWidgetRegion = { GetAbsolutePosition().m_iX, GetAbsolutePosition().m_iY, m_dmSize.m_iWidth, m_dmSize.m_iHeight };
				FGUI::AREA arScrollBarRegion = { (arWidgetRegion.m_iLeft + arWidgetRegion.m_iRight) - 15, arWidgetRegion.m_iTop, 15, arWidgetRegion.m_iBottom };

				if (FGUI::INPUT.IsCursorInArea(arScrollBarRegion))
				{
					// change cursor
					SetCursor(CURSOR_STYLE::HAND);

					if (FGUI::INPUT.IsKeyPressed(MOUSE_1))
					{
						bIsDraggingThumb = true;
					}
				}

				if (bIsDraggingThumb)
				{
					FGUI::POINT ptCursorPosDelta = FGUI::INPUT.GetCursorPosDelta();

					static constexpr int iLinesToScroll = 2; // NOTE: feel free to change this

					if (FGUI::INPUT.IsKeyHeld(MOUSE_1))
					{
						m_iWidgetScrollOffset += (ptCursorPosDelta.m_iY * iLinesToScroll);
					}
					else
					{
						bIsDraggingThumb = false;
					}

					// disable scrolling if a widget is being focused
					if (GetFocusedWidget())
					{
						bIsDraggingThumb = false;
					}
				}

				// clamp scrolling
				m_iWidgetScrollOffset = std::clamp(m_iWidgetScrollOffset, 0, std::max(0, ((m_prgpWidgets.back()->GetPosition().m_iY + (m_prgpWidgets.back()->GetSize().m_iHeight + 15))) - m_dmSize.m_iHeight));
			}
		}

		// this will tell the container to skip focused widgets (so it can be drawned after all other widgets)
		bool bSkipWidget = false;

		// this will hold the current skipped widget
		std::shared_ptr<FGUI::CWidgets> pWidgetToSkip = nullptr;

		// handle skipped widgets first
		if (m_bIsFocusingOnWidget)
		{
			// check if the skipped widget can be used
			if (m_pFocusedWidget && m_pFocusedWidget->IsUnlocked())
			{
				// tell the container to skip this widget
				bSkipWidget = true;

				// assign the widget that will be skipped
				pWidgetToSkip = m_pFocusedWidget;

				FGUI::AREA arSkippedWidgetRegion = { pWidgetToSkip->GetAbsolutePosition().m_iX, pWidgetToSkip->GetAbsolutePosition().m_iY, pWidgetToSkip->GetSize().m_iWidth, pWidgetToSkip->GetSize().m_iHeight };

				if (m_bScrollBarState)
				{
					if ((pWidgetToSkip->GetAbsolutePosition().m_iY + pWidgetToSkip->GetSize().m_iHeight) <= (GetAbsolutePosition().m_iY + GetSize().m_iHeight) && (pWidgetToSkip->GetAbsolutePosition().m_iY >= GetAbsolutePosition().m_iY))
					{
						pWidgetToSkip->Update();

						// check if the skipped widget can be clicked
						if (GetFocusedWidget()->GetFlags(WIDGET_FLAG::CLICKABLE) && FGUI::INPUT.IsCursorInArea(arSkippedWidgetRegion) && FGUI::INPUT.IsKeyPressed(MOUSE_1) && bSkipWidget)
						{
							pWidgetToSkip->Input();

							// loose unfocus
							SetFocusedWidget(nullptr);

							// reset focused widget state
							pWidgetToSkip.reset();
						}
					}
				}
				else
				{
					pWidgetToSkip->Update();

					// check if the skipped widget can be clicked
					if (GetFocusedWidget()->GetFlags(WIDGET_FLAG::CLICKABLE) && FGUI::INPUT.IsCursorInArea(arSkippedWidgetRegion) && FGUI::INPUT.IsKeyPressed(MOUSE_1) && bSkipWidget)
					{
						pWidgetToSkip->Input();

						// unfocus widget
						SetFocusedWidget(nullptr);

						// reset focused widget state
						pWidgetToSkip.reset();
					}
				}

				// change cursor
				if (pWidgetToSkip && FGUI::INPUT.IsCursorInArea(arSkippedWidgetRegion) && pWidgetToSkip->GetType() != static_cast<int>(WIDGET_TYPE::TABPANEL) && bSkipWidget)
				{
					if (pWidgetToSkip->GetType() == static_cast<int>(WIDGET_TYPE::CONTAINER))
					{
						SetCursor(CURSOR_STYLE::ARROW);
					}
					else if (pWidgetToSkip->GetType() == static_cast<int>(WIDGET_TYPE::TEXTBOX))
					{
						SetCursor(CURSOR_STYLE::IBEAM);
					}
					else
					{
						SetCursor(CURSOR_STYLE::HAND);
					}
				}
			}
		}

		// iterate over the rest of the widgets
		for (std::shared_ptr<FGUI::CWidgets> pWidgets : m_prgpWidgets)
		{
			if (pWidgets->IsUnlocked())
			{
				// if a widget is currently being skipped
				if (bSkipWidget)
				{
					// we don't want to handle skipped widgets here
					if (pWidgetToSkip == pWidgets)
					{
						continue;
					}
				}

				FGUI::AREA arWidgetRegion;

				if (m_bScrollBarState)
				{
					if ((pWidgets->GetAbsolutePosition().m_iY + pWidgets->GetSize().m_iHeight) <= (GetAbsolutePosition().m_iY + GetSize().m_iHeight) && (pWidgets->GetAbsolutePosition().m_iY >= GetAbsolutePosition().m_iY))
					{
						pWidgets->Update();

						// if the widget is a CheckBox allow input if the user clicks on its title
						if (pWidgets->GetType() == static_cast<int>(WIDGET_TYPE::CHECKBOX))
						{
							// get the text size of the CheckBox title
							FGUI::DIMENSION dmCheckBoxTitleTextSize = FGUI::RENDER.GetTextSize(pWidgets->GetFont(), pWidgets->GetTitle());

							arWidgetRegion = { pWidgets->GetAbsolutePosition().m_iX, pWidgets->GetAbsolutePosition().m_iY, (pWidgets->GetSize().m_iWidth + dmCheckBoxTitleTextSize.m_iWidth) + 5, pWidgets->GetSize().m_iHeight };
						}
						else
						{
							arWidgetRegion = { pWidgets->GetAbsolutePosition().m_iX, pWidgets->GetAbsolutePosition().m_iY, pWidgets->GetSize().m_iWidth, pWidgets->GetSize().m_iHeight };
						}

						// check if the widget can be clicked
						if (pWidgets->GetFlags(WIDGET_FLAG::CLICKABLE) && FGUI::INPUT.IsCursorInArea(arWidgetRegion) && FGUI::INPUT.IsKeyPressed(MOUSE_1) && !bSkipWidget)
						{
							pWidgets->Input();

							if (pWidgets->GetFlags(WIDGET_FLAG::FOCUSABLE))
							{
								SetFocusedWidget(pWidgets);
							}
							else
							{
								SetFocusedWidget(nullptr);
							}
						}
					}
				}
				else
				{
					pWidgets->Update();

					// if the widget is a CheckBox allow input if the user clicks on its title
					if (pWidgets->GetType() == static_cast<int>(WIDGET_TYPE::CHECKBOX))
					{
						// get the text size of the CheckBox title
						FGUI::DIMENSION dmCheckBoxTitleTextSize = FGUI::RENDER.GetTextSize(pWidgets->GetFont(), pWidgets->GetTitle());

						arWidgetRegion = { pWidgets->GetAbsolutePosition().m_iX, pWidgets->GetAbsolutePosition().m_iY, (pWidgets->GetSize().m_iWidth + dmCheckBoxTitleTextSize.m_iWidth) + 5, pWidgets->GetSize().m_iHeight };
					}
					else
					{
						arWidgetRegion = { pWidgets->GetAbsolutePosition().m_iX, pWidgets->GetAbsolutePosition().m_iY, pWidgets->GetSize().m_iWidth, pWidgets->GetSize().m_iHeight };
					}

					// check if the widget can be clicked
					if (pWidgets->GetFlags(WIDGET_FLAG::CLICKABLE) && FGUI::INPUT.IsCursorInArea(arWidgetRegion) && FGUI::INPUT.IsKeyPressed(MOUSE_1) && !bSkipWidget)
					{
						pWidgets->Input();

						if (pWidgets->GetFlags(WIDGET_FLAG::FOCUSABLE))
						{
							SetFocusedWidget(pWidgets);
						}
						else
						{
							SetFocusedWidget(nullptr);
						}
					}
				}

				// change cursor
				if (pWidgets && FGUI::INPUT.IsCursorInArea(arWidgetRegion) && pWidgets->GetType() != static_cast<int>(WIDGET_TYPE::TABPANEL) && !bSkipWidget)
				{
					if (pWidgets->GetType() == static_cast<int>(WIDGET_TYPE::CONTAINER))
					{
						SetCursor(CURSOR_STYLE::ARROW);
					}
					else if (pWidgets->GetType() == static_cast<int>(WIDGET_TYPE::TEXTBOX))
					{
						SetCursor(CURSOR_STYLE::IBEAM);
					}
					else
					{
						SetCursor(CURSOR_STYLE::HAND);
					}
				}
			}
		}
	}

	void CContainer::Input()
	{
	}

	void CContainer::Save(nlohmann::json& module)
	{
		if (m_prgpWidgets.empty())
		{
			return;
		}

		for (std::shared_ptr<FGUI::CWidgets> pWidgets : m_prgpWidgets)
		{
			if (pWidgets->GetType() == static_cast<int>(WIDGET_TYPE::CONTAINER))
			{
				pWidgets->Save(module);
			}
			else if (pWidgets->GetFlags(WIDGET_FLAG::SAVABLE)) // check if the widget can be saved
			{
				pWidgets->Save(module);
			}
		}
	}

	void CContainer::Load(nlohmann::json& module)
	{
		if (m_prgpWidgets.empty())
		{
			return;
		}

		for (std::shared_ptr<FGUI::CWidgets> pWidgets : m_prgpWidgets)
		{
			if (pWidgets->GetType() == static_cast<int>(WIDGET_TYPE::CONTAINER))
			{
				pWidgets->Load(module);
			}
			else if (pWidgets->GetFlags(WIDGET_FLAG::SAVABLE)) // check if the widget can be loaded
			{
				pWidgets->Load(module);
			}
		}
	}

	void CContainer::Tooltip()
	{
		for (std::shared_ptr<FGUI::CWidgets> pWidgets : m_prgpWidgets)
		{
			// check if widgets are unlocked
			if (pWidgets && pWidgets->IsUnlocked() && pWidgets->GetFlags(WIDGET_FLAG::DRAWABLE))
			{
				// avoid drawing tooltips when a widget is being focused
				if (!std::reinterpret_pointer_cast<FGUI::CContainer>(pWidgets->GetParentWidget())->GetFocusedWidget())
				{
					FGUI::AREA arWidgetRegion = { pWidgets->GetAbsolutePosition().m_iX, pWidgets->GetAbsolutePosition().m_iY, pWidgets->GetSize().m_iWidth, pWidgets->GetSize().m_iHeight };

					if (FGUI::INPUT.IsCursorInArea(arWidgetRegion))
					{
						pWidgets->Tooltip();
					}
				}
			}
		}
	}

	void CContainer::Cursor()
	{
		FGUI::POINT ptCursorPos = FGUI::INPUT.GetCursorPos();

		switch (m_nCursorStyle)
		{
			case static_cast<int>(CURSOR_STYLE::ARROW) :
			{
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 1, ptCursorPos.m_iY, 1, 17, { 1, 1, 1 });

				for (std::size_t i = 0; i < 11; i++)
				{
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 2 + i, ptCursorPos.m_iY + 1 + i, 1, 1, { 1, 1, 1 });
				}

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 8, ptCursorPos.m_iY + 12, 5, 1, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 8, ptCursorPos.m_iY + 13, 1, 1, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 9, ptCursorPos.m_iY + 14, 1, 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 10, ptCursorPos.m_iY + 16, 1, 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 8, ptCursorPos.m_iY + 18, 2, 1, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 7, ptCursorPos.m_iY + 16, 1, 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 6, ptCursorPos.m_iY + 14, 1, 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 5, ptCursorPos.m_iY + 13, 1, 1, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 4, ptCursorPos.m_iY + 14, 1, 1, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 3, ptCursorPos.m_iY + 15, 1, 1, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 2, ptCursorPos.m_iY + 16, 1, 1, { 1, 1, 1 });

				for (std::size_t i = 0; i < 4; i++)
				{
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 2 + i, ptCursorPos.m_iY + 2 + i, 1, 14 - (i * 2), { 245, 245, 254 });
				}

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 6, ptCursorPos.m_iY + 6, 1, 8, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 7, ptCursorPos.m_iY + 7, 1, 9, { 245, 245, 254 });

				for (std::size_t i = 0; i < 4; i++)
				{
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 8 + i, ptCursorPos.m_iY + 8 + i, 1, 4 - i, { 245, 245, 254 });
				}

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 8, ptCursorPos.m_iY + 14, 1, 4, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 9, ptCursorPos.m_iY + 16, 1, 2, { 245, 245, 254 });

				break;
			}
			case static_cast<int>(CURSOR_STYLE::HAND) :
			{
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 6, ptCursorPos.m_iY + 5 + 5 + -5, 7, 9, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5, ptCursorPos.m_iY + 5 + 7 + -5, 1, 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 1, ptCursorPos.m_iY + 5 + 6 + -5, 1, 1, { 1, 1, 1 });

				for (std::size_t i = 0; i < 3; ++i)
				{
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 2 + i, ptCursorPos.m_iY + 5 + 6 + i + -5, 1, 1, { 1, 1, 1 });
				}

				for (std::size_t i = 0; i < 4; ++i)
				{
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 1 + i, ptCursorPos.m_iY + 5 + 9 + i + -5, 1, 1, { 1, 1, 1 });
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 1 + i, ptCursorPos.m_iY + 5 + 7 + i + -5, 1, 1, { 245, 245, 254 });
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 2 + i, ptCursorPos.m_iY + 5 + 7 + i + -5, 1, 1, { 245, 245, 254 });
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 1 + i, ptCursorPos.m_iY + 5 + 8 + i + -5, 1, 1, { 245, 245, 254 });
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 3 + i, ptCursorPos.m_iY + 5 + 11 + i + -5, 1, 1, { 245, 245, 254 });
				}

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 4, ptCursorPos.m_iY + 5 + 1 + -5, 1, 7, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 5, ptCursorPos.m_iY + 5 + 0 + -5, 2, 1, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 5, ptCursorPos.m_iY + 5 + 15 + -5, 5, 1, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 7, ptCursorPos.m_iY + 5 + 1 + -5, 1, 6, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 8, ptCursorPos.m_iY + 5 + 4 + -5, 4, 1, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 10, ptCursorPos.m_iY + 5 + 5 + -5, 1, 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 12, ptCursorPos.m_iY + 5 + 5 + -5, 1, 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 13, ptCursorPos.m_iY + 5 + 5 + -5, 1, 1, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 14, ptCursorPos.m_iY + 5 + 6 + -5, 1, 5, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 13, ptCursorPos.m_iY + 5 + 11 + -5, 1, 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 12, ptCursorPos.m_iY + 5 + 13 + -5, 1, 3, { 1, 1, 1 });

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 5, ptCursorPos.m_iY + 5 + 1 + -5, 2, 13, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 7, ptCursorPos.m_iY + 5 + 14 + -5, 3, 1, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 13, ptCursorPos.m_iY + 5 + 6 + -5, 1, 5, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 11, ptCursorPos.m_iY + 5 + 14 + -5, 1, 1, { 245, 245, 254 });

				for (std::size_t i = 0; i < 2; ++i)
				{
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 10 + i, ptCursorPos.m_iY + 5 + 14 + i + -5, 1, 1, { 1, 1, 1 });
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 4 + i, ptCursorPos.m_iY + 5 + 13 + i + -5, 1, 1, { 1, 1, 1 });
				}

				break;
			}
			case static_cast<int>(CURSOR_STYLE::PIPETTE) :
			{
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX, ptCursorPos.m_iY + 14 + -15, 1, 1, { 1, 1, 1 });

				for (std::size_t i = 0; i < 2; ++i)
				{
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX + i, ptCursorPos.m_iY + 13 - i + -15, 1, 1, { 1, 1, 1 });
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 1 + i, ptCursorPos.m_iY + 14 - i + -15, 1, 1, { 1, 1, 1 });
				}

				for (std::size_t i = 0; i < 6; ++i)
				{
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 1 + i, ptCursorPos.m_iY + 11 - i + -15, 1, 1, { 1, 1, 1 });
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 3 + i, ptCursorPos.m_iY + 13 - i + -15, 1, 1, { 1, 1, 1 });

					FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 2 + i, ptCursorPos.m_iY + 11 - i + -15, 1, 1, { 245, 245, 254 });
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 3 + i, ptCursorPos.m_iY + 12 - i + -15, 1, 1, { 245, 245, 254 });
					FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 2 + i, ptCursorPos.m_iY + 12 - i + -15, 1, 1, { 245, 245, 254 });
				}

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 8, ptCursorPos.m_iY + 6 + -15, 1, 1, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 7, ptCursorPos.m_iY + 3 + -15, 6, 3, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 9, ptCursorPos.m_iY + 2 + -15, 3, 6, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 11, ptCursorPos.m_iY + -15, 3, 5, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX + 10, ptCursorPos.m_iY + 1 + -15, 5, 3, { 1, 1, 1 });

				break;
			}
			case static_cast<int>(CURSOR_STYLE::MOVE) :
			{
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 - 1, ptCursorPos.m_iY - 1 - 1, 5 + 2, 1 + 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 4 - 1, ptCursorPos.m_iY - 2 - 1, 3 + 2, 1 + 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 3 - 1, ptCursorPos.m_iY - 3 - 1, 1 + 2, 1 + 2, { 1, 1, 1 });

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5, ptCursorPos.m_iY - 1, 5, 1, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 4, ptCursorPos.m_iY - 2, 3, 1, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 3, ptCursorPos.m_iY - 3, 1, 1, { 245, 245, 254 });

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 3 - 7 - 1, ptCursorPos.m_iY + 11 - 7 - 1, 1 + 2, 5 + 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 4 - 7 - 1, ptCursorPos.m_iY + 12 - 7 - 1, 1 + 2, 3 + 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 - 7 - 1, ptCursorPos.m_iY + 13 - 7 - 1, 1 + 2, 1 + 2, { 1, 1, 1 });

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 3 - 7, ptCursorPos.m_iY + 11 - 7, 1, 5, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 4 - 7, ptCursorPos.m_iY + 12 - 7, 1, 3, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 - 7, ptCursorPos.m_iY + 13 - 7, 1, 1, { 245, 245, 254 });

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 - 1, ptCursorPos.m_iY + 12 - 1, 5 + 2, 1 + 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 4 - 1, ptCursorPos.m_iY + 13 - 1, 3 + 2, 1 + 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 3 - 1, ptCursorPos.m_iY + 14 - 1, 1 + 2, 1 + 2, { 1, 1, 1 });

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5, ptCursorPos.m_iY + 12, 5, 1, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 4, ptCursorPos.m_iY + 13, 3, 1, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 3, ptCursorPos.m_iY + 14, 1, 1, { 245, 245, 254 });

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 9 - 1, ptCursorPos.m_iY + 11 - 7 - 1, 1 + 2, 5 + 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 4 + 9 - 1, ptCursorPos.m_iY + 12 - 7 - 1, 1 + 2, 3 + 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 3 + 9 - 1, ptCursorPos.m_iY + 13 - 7 - 1, 1 + 2, 1 + 2, { 1, 1, 1 });

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 5 + 9, ptCursorPos.m_iY + 11 - 7, 1, 5, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 4 + 9, ptCursorPos.m_iY + 12 - 7, 1, 3, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 3 + 9, ptCursorPos.m_iY + 13 - 7, 1, 1, { 245, 245, 254 });

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 3 - (12 / 2) - 1, ptCursorPos.m_iY + (12 / 2) - 1, 13 + 2, 1 + 2, { 1, 1, 1 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 3 - 1, ptCursorPos.m_iY - 1, 1 + 2, 12 + 2, { 1, 1, 1 });

				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 3 - (12 / 2), ptCursorPos.m_iY + (12 / 2), 13, 1, { 245, 245, 254 });
				FGUI::RENDER.Rectangle(ptCursorPos.m_iX - 3, ptCursorPos.m_iY, 1, 12, { 245, 245, 254 });

				break;
			}
			default:
				return;
		}
	}

	void CContainer::SetDraggable(bool draggable) {
		m_draggable = draggable;
	}

} // namespace FGUI