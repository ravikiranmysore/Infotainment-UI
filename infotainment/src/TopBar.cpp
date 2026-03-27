#include "TopBar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QLinearGradient>
#include <QDateTime>

TopBar::TopBar(QWidget *parent) : QWidget(parent)
{
    setFixedHeight(48);
    setObjectName("TopBar");

    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(16, 0, 16, 0);
    lay->setSpacing(0);

    // Close button
    closeBtn = new QPushButton("✕", this);
    closeBtn->setFixedSize(32, 32);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(R"(
        QPushButton { background:#ff3355; border-radius:16px; color:white;
                      font-size:13px; font-weight:bold; border:none; }
        QPushButton:hover { background:#ff5577; }
    )");
    connect(closeBtn, &QPushButton::clicked, this, &TopBar::closeClicked);

    homeBtn = new QPushButton("⌂", this);
    homeBtn->setFixedSize(32, 32);
    homeBtn->setCursor(Qt::PointingHandCursor);
    homeBtn->setStyleSheet(R"(
        QPushButton { background:rgba(255,255,255,0.08); border-radius:16px;
                      color:#aaa; font-size:16px; border:none; }
        QPushButton:hover { background:rgba(255,255,255,0.16); color:white; }
    )");
    connect(homeBtn, &QPushButton::clicked, this, &TopBar::homeClicked);

    // Status indicators
    tempLbl   = new QLabel("18 °C  ☁", this);
    signalLbl = new QLabel("📶  🔵  📡", this);
    battLbl   = new QLabel("🔋 87%", this);

    QString baseStyle = "color:#aaa; font-size:11px; background:transparent;";
    tempLbl->setStyleSheet(baseStyle);
    signalLbl->setStyleSheet(baseStyle);
    battLbl->setStyleSheet(baseStyle + "color:#00ff88;");

    // Center title
    titleLbl = new QLabel("InfoDrive", this);
    titleLbl->setStyleSheet("color:white; font-size:15px; font-weight:600; background:transparent; letter-spacing:1px;");
    titleLbl->setAlignment(Qt::AlignCenter);

    // Clock
    timeLbl = new QLabel(QDateTime::currentDateTime().toString("hh:mm"), this);
    timeLbl->setStyleSheet("color:white; font-size:15px; font-weight:bold; background:transparent; min-width:46px;");
    timeLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    lay->addWidget(closeBtn);
    lay->addSpacing(8);
    lay->addWidget(homeBtn);
    lay->addSpacing(16);
    lay->addWidget(tempLbl);
    lay->addStretch();
    lay->addWidget(titleLbl);
    lay->addStretch();
    lay->addWidget(signalLbl);
    lay->addSpacing(12);
    lay->addWidget(battLbl);
    lay->addSpacing(12);
    lay->addWidget(timeLbl);
}

void TopBar::setTime(const QString &t)   { timeLbl->setText(t); }
void TopBar::setPageTitle(const QString &t) { titleLbl->setText(t); }
void TopBar::pulse() { /* could animate bell here */ }

void TopBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QLinearGradient g(0, 0, 0, height());
    g.setColorAt(0, QColor(14, 14, 30));
    g.setColorAt(1, QColor(10, 10, 22));
    p.fillRect(rect(), g);
    // bottom border
    p.setPen(QColor(40, 40, 70));
    p.drawLine(0, height()-1, width(), height()-1);
}
