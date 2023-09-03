/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>

template <typename TestClass>
void runTests(int argc, char* argv[], int* status)
{
    ::QTest::Internal::callInitMain<TestClass>();
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    QTEST_DISABLE_KEYPAD_NAVIGATION TestClass tc;
    QTEST_SET_MAIN_SOURCE_PATH
    *status |= QTest::qExec(&tc, argc, argv);
}
