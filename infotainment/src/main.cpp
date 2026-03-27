#include <QApplication>
#include "HMIWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("InfoDrive HMI");

    app.setStyleSheet(R"(
        * { font-family: 'Segoe UI', 'SF Pro Display', Arial, sans-serif; }
        QScrollBar:vertical   { width:4px; background:transparent; }
        QScrollBar::handle:vertical { background:#ffffff22; border-radius:2px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }
        QScrollBar:horizontal { height:4px; background:transparent; }
        QScrollBar::handle:horizontal { background:#ffffff22; border-radius:2px; }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width:0; }
        QToolTip { background:#1e1e38; color:white; border:1px solid #3a3a5c;
                   border-radius:6px; padding:4px 8px; font-size:11px; }
    )");

    HMIWindow w;
    w.show();
    return app.exec();
}
