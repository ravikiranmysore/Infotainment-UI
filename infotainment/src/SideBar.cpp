#include "SideBar.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QLinearGradient>

SideBar::SideBar(QWidget *parent) : QWidget(parent)
{
    setFixedWidth(80);
    setObjectName("SideBar");

    items = {
        {"🏠", "Home"},
        {"🎵", "Media"},
        {"🗺️",  "Maps"},
        {"❄️",  "Climate"},
        {"📱", "Phone"},
        {"⚡", "Apps"},
        {"⚙️",  "Settings"},
    };

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(6, 12, 6, 12);
    lay->setSpacing(4);

    for (int i = 0; i < items.size(); ++i) {
        auto &item = items[i];
        auto *btn  = new QPushButton(this);
        btn->setCheckable(true);
        btn->setChecked(i == 0);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(68);
        btn->setObjectName("sideBtn");

        // Build layout inside button
        auto *vl   = new QVBoxLayout(btn);
        vl->setContentsMargins(4, 6, 4, 6);
        vl->setSpacing(2);
        auto *ico  = new QLabel(item.emoji, btn);
        ico->setAlignment(Qt::AlignCenter);
        ico->setStyleSheet("font-size:22px; background:transparent; border:none;");
        auto *lbl  = new QLabel(item.label, btn);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("font-size:9px; color:#888; background:transparent; border:none; letter-spacing:0.5px;");
        vl->addWidget(ico);
        vl->addWidget(lbl);

        int idx = i;
        connect(btn, &QPushButton::clicked, this, [this, idx, lbl]() {
            setActive(idx);
            emit pageRequested(idx);
        });

        lay->addWidget(btn);
        btns.append(btn);
    }
    lay->addStretch();
}

void SideBar::setActive(int idx)
{
    activeIdx = idx;
    for (int i = 0; i < btns.size(); ++i)
        btns[i]->setChecked(i == idx);
}

void SideBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QLinearGradient g(0, 0, width(), 0);
    g.setColorAt(0, QColor(10, 10, 20));
    g.setColorAt(1, QColor(13, 13, 26));
    p.fillRect(rect(), g);
    // right border
    p.setPen(QColor(30, 30, 55));
    p.drawLine(width()-1, 0, width()-1, height());

    // active indicator bar
    if (activeIdx >= 0 && activeIdx < btns.size()) {
        auto *ab = btns[activeIdx];
        int y = ab->y() + 12;
        int h = ab->height() - 24;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#00d4ff"));
        p.drawRoundedRect(0, y, 3, h, 2, 2);
    }
}
