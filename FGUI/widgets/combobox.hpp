//
// FGUI - feature rich graphical user interface
//

#ifndef FGUI_COMBOBOX_HH
#define FGUI_COMBOBOX_HH

// includes
#include <string>
#include <functional>
#include <utility>
#include <vector>
#include "theme.h"

// library includes
#include "widgets.hpp"

namespace FGUI
{

	class CComboBox : public FGUI::CWidgets
	{
	public:
		CComboBox();

		// @brief: set the combobox state (toggle dropdown list on/off)
		// @args: bool onoff = dropdown state
		void SetState(bool onoff);

		// @brief: get the combobox state (dropdown list enabled or not)
		bool GetState();

		// @brief: set the combobox selected index
		// @args: std::size_t index = selected index (starting with 0)
		void SetIndex(std::size_t index);

		// @brief: set the combobox selected index
		// @args: std::size_t index = selected index (starting with 0)
		void SetIndex(const std::string title);

		// @brief: get the combobox selected index (entry)
		std::size_t GetIndex();

		// @brief: sets a custom value to a specific entry on the dropdown
		// @args: std::size_t index = entry index, unsigned int value = custom value
		void SetValue(std::size_t index, unsigned int value);

		// @brief: get the combobox selected entry custom value
		std::size_t GetValue();

		// @brief: adds a new entry inside the combobox
		// @args: std::string name = entry title, unsigned int value = entry custom value
		void AddEntry(std::string name, unsigned int value = 0);

		// @brief: adds a function callback for the combobox (it will call the function whenever the user selects something on the combobox)
		// @args: std::function<void()> callback = function instance
		void AddCallback(std::function<void()> callback);

		// @brief: populate widget geometry (draw widget)
		// @args: FGUI::WIDGET_STATUS status = widget status (HOVERED, etc)
		void Geometry(FGUI::WIDGET_STATUS status) override;

		// @brief: handle update operations on the widget
		void Update() override;

		// @brief: handle input inside the widget
		void Input() override;

		// @brief: save the widget state
		// @args: nlohmann::json module = json module
		void Save(nlohmann::json& module) override;

		// @brief: load the widget state
		// @args: nlohmann::json module = json module
		void Load(nlohmann::json& module) override;

		// @brief: handle widget tooltips
		void Tooltip() override;

		void BindMap(const std::map<std::string, std::string> &map, std::string *ref) {
			m_boundString = ref;
			
			for (const auto &[k,v] : map) {
				m_Keys.push_back(k);

				AddEntry(v);
			}
		}

		void BindVec(const std::vector<std::string>& map, int* ref) {
			m_boundInt = ref;

			for (const auto& k : map) {
				m_Keys.push_back(k);

				AddEntry(k);
			}
		}

		void BindIndex(int* ref) {
			m_boundInt = ref;
		}

		void SetHeight(int height) {
			m_iCustomHeight = height;
		}


	private:
		std::vector<std::string> m_Keys{};
		std::string* m_boundString = nullptr;
		int* m_boundInt = nullptr;
		
		bool m_bIsOpened;
		int m_iEntrySpacing;
		int m_iCustomHeight;
		int m_iScrollThumbPosition;
		bool m_bIsDraggingThumb;
		std::size_t m_ullSelectedEntry;
		std::pair<std::vector<std::string>, std::vector<int>> m_prgpEntries;
		std::function<void()> m_fnctCallback;
	};
} // namespace FGUI

#endif // FGUI_COMBOBOX_HH