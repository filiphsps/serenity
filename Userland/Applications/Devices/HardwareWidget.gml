@GUI::Frame {
    fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [8]
    }

    @GUI::TreeView {
        name: "device_table"
        column_headers_visible: true
        should_fill_selected_rows: true
        selection_behavior: "SelectItems"
    }
}
