#include "AppsScreen.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPainter>
#include <QLinearGradient>
#include <QGraphicsDropShadowEffect>
#include <functional>

AppsScreen::AppsScreen(QWidget *parent) : QWidget(parent) { setupUI(); }

void AppsScreen::setupUI()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(30, 20, 30, 20);
    root->setSpacing(18);

    // Header
    auto *hdr = new QLabel("⚡  APPS", this);
    hdr->setStyleSheet("color:#555; font-size:10px; letter-spacing:4px;");
    root->addWidget(hdr);

    // Scroll area
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background:transparent; border:none; }");

    auto *container = new QWidget();
    container->setStyleSheet("background:transparent;");
    auto *grid = new QGridLayout(container);
    grid->setSpacing(16);
    grid->setContentsMargins(4, 4, 4, 4);

    struct App {
        QString emoji, name, category;
        QColor  color;
    };

    QList<App> apps = {
        // Row 0
        {"🗺️",  "Maps",         "Navigation",  QColor(0,  212,255)},
        {"🎵", "Music",         "Media",       QColor(170, 0, 255)},
        {"📻", "Radio",         "Media",       QColor(255,100,  0)},
        {"📱", "Phone",         "Communication",QColor(0, 255,136)},
        // Row 1
        {"📲", "Messages",      "Communication",QColor(0, 180,255)},
        {"📧", "Email",         "Communication",QColor( 80,180,255)},
        {"🌤",  "Weather",       "Info",         QColor( 30,140,255)},
        {"📰", "News",          "Info",         QColor(200, 60, 60)},
        // Row 2
        {"⚙️",  "Settings",      "System",       QColor(120,120,160)},
        {"🔊", "Sound",         "System",       QColor(255,170,  0)},
        {"🔋", "Energy",        "System",       QColor( 0, 220, 80)},
        {"🅿",  "Parking",       "Services",     QColor(255,200,  0)},
        // Row 3
        {"⛽", "Fuel Finder",   "Services",     QColor(255,120,  0)},
        {"🏨", "Hotels",        "Travel",       QColor(140,  0,255)},
        {"☕", "Cafes",         "Food",         QColor(180, 90, 40)},
        {"🍕", "Restaurants",   "Food",         QColor(255, 60, 60)},
    };

    for (int i = 0; i < apps.size(); ++i) {
        const App &app = apps[i];

        auto *card = new QWidget(container);
        card->setFixedSize(148, 120);
        card->setCursor(Qt::PointingHandCursor);
        card->setStyleSheet(QString(R"(
            QWidget {
                background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
                    stop:0 rgba(%1,%2,%3,0.16), stop:1 rgba(%1,%2,%3,0.05));
                border-radius: 16px;
                border: 1px solid rgba(%1,%2,%3,0.28);
            }
            QWidget:hover {
                background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
                    stop:0 rgba(%1,%2,%3,0.28), stop:1 rgba(%1,%2,%3,0.10));
                border: 1px solid rgba(%1,%2,%3,0.50);
            }
        )").arg(app.color.red()).arg(app.color.green()).arg(app.color.blue()));

        auto *cvl = new QVBoxLayout(card);
        cvl->setContentsMargins(14, 14, 14, 12);
        cvl->setSpacing(6);

        auto *ico = new QLabel(app.emoji, card);
        ico->setStyleSheet("font-size:32px; background:transparent; border:none;");

        auto *nameLbl = new QLabel(app.name, card);
        nameLbl->setStyleSheet("color:white; font-size:12px; font-weight:bold;"
                               " background:transparent; border:none;");

        auto *catLbl = new QLabel(app.category, card);
        catLbl->setStyleSheet("color:#555; font-size:9px; letter-spacing:0.5px;"
                              " background:transparent; border:none;");

        cvl->addWidget(ico);
        cvl->addWidget(nameLbl);
        cvl->addWidget(catLbl);
        cvl->addStretch();

        // transparent click overlay
        auto *overlay = new QPushButton(card);
        overlay->setGeometry(0, 0, 148, 120);
        overlay->setStyleSheet("QPushButton { background:transparent; border:none; }");
        overlay->raise();

        grid->addWidget(card, i / 4, i % 4);
    }

    scroll->setWidget(container);
    root->addWidget(scroll, 1);

    // Bottom: Recently used strip
    auto *recentHdr = new QLabel("🕐  RECENTLY USED", this);
    recentHdr->setStyleSheet("color:#444; font-size:9px; letter-spacing:3px;");
    root->addWidget(recentHdr);

    auto *recentRow = new QHBoxLayout();
    recentRow->setSpacing(12);
    recentRow->setAlignment(Qt::AlignLeft);

    QList<App> recent = {apps[0], apps[1], apps[3], apps[6], apps[10]};
    for (auto &app : recent) {
        auto *chip = new QPushButton(app.emoji + "  " + app.name, this);
        chip->setFixedHeight(36);
        chip->setCursor(Qt::PointingHandCursor);
        chip->setStyleSheet(QString(R"(
            QPushButton {
                background: rgba(%1,%2,%3,0.10);
                border-radius: 18px;
                color: white; font-size: 12px;
                border: 1px solid rgba(%1,%2,%3,0.25);
                padding: 0 14px;
            }
            QPushButton:hover {
                background: rgba(%1,%2,%3,0.22);
            }
        )").arg(app.color.red()).arg(app.color.green()).arg(app.color.blue()));
        recentRow->addWidget(chip);
    }
    root->addLayout(recentRow);
}

void AppsScreen::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QLinearGradient bg(0, 0, width(), height());
    bg.setColorAt(0, QColor(10, 10, 22));
    bg.setColorAt(1, QColor(8,  8,  18));
    p.fillRect(rect(), bg);
}
