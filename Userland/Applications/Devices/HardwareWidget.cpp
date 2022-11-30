/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "HardwareWidget.h"
#include "HardwareModel.h"
#include <Applications/Devices/HardwareWidgetGML.h>
#include <LibGUI/SortingProxyModel.h>
#include <LibGUI/TreeView.h>

void HardwareWidget::reset_default_values()
{
}

void HardwareWidget::apply_settings()
{
}

HardwareWidget::HardwareWidget()
{
    load_from_gml(hardware_widget_gml);

    auto hardware_model = HardwareModel::create();
    m_device_table_view = *find_descendant_of_type_named<GUI::TreeView>("device_table");
    m_device_table_view->set_model(GUI::SortingProxyModel::create(hardware_model).release_value_but_fixme_should_propagate_errors());

    for (auto column = 0; column < HardwareModel::Column::__Count; ++column) {
        m_device_table_view->set_column_visible(column, hardware_model->is_default_column(column));
    }

    m_device_table_view->set_key_column_and_sort_order(HardwareModel::Column::Name, GUI::SortOrder::Ascending);
    hardware_model->update();
}
