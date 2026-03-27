#include "NotificationOverlay.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>

NotificationOverlay::NotificationOverlay(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setFixedSize(330, 74);

    auto *hl = new QHBoxLayout(this);
    hl->setContentsMargins(14, 10, 14, 10);
    hl->setSpacing(12);

    iconLbl = new QLabel(this);
    iconLbl->setFixedSize(40, 40);
    iconLbl->setAlignment(Qt::AlignCenter);
    iconLbl->setStyleSheet("font-size:24px; background:transparent;");

    auto *textCol = new QWidget(this);
    auto *vl = new QVBoxLayout(textCol);
    vl->setContentsMargins(0,0,0,0);
    vl->setSpacing(2);

    titleLbl = new QLabel(this);
    titleLbl->setStyleSheet("color:white; font-size:12px; font-weight:bold; background:transparent;");

    msgLbl = new QLabel(this);
    msgLbl->setStyleSheet("color:#aaa; font-size:11px; background:transparent;");
    msgLbl->setWordWrap(true);

    vl->addWidget(titleLbl);
    vl->addWidget(msgLbl);

    hl->addWidget(iconLbl);
    hl->addWidget(textCol, 1);

    hideTimer = new QTimer(this);
    hideTimer->setSingleShot(true);
    connect(hideTimer, &QTimer::timeout, this, [this]{
        fadeAnim->setStartValue(1.f);
        fadeAnim->setEndValue(0.f);
        fadeAnim->start();
    });

    fadeAnim = new QPropertyAnimation(this, "opacity", this);
    fadeAnim->setDuration(400);
    connect(fadeAnim, &QPropertyAnimation::finished, this, [this]{
        if (m_opacity < 0.05f) QWidget::hide();
    });
}

void NotificationOverlay::show(const QString &icon,
                                const QString &title,
                                const QString &msg)
{
    iconLbl->setText(icon);
    titleLbl->setText(title);
    msgLbl->setText(msg);

    // Slide-in position: parent right side
    if (parentWidget()) {
        setGeometry(parentWidget()->width() - width() - 12,
                    52,
                    width(), height());
    }
    QWidget::show();
    raise();

    fadeAnim->stop();
    m_opacity = 1.f;
    update();

    hideTimer->start(3500);
}

void NotificationOverlay::setOpacity(float o)
{
    m_opacity = o;
    update();
    if (o < 0.01f) QWidget::hide();
}

void NotificationOverlay::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setOpacity(m_opacity);

    // Glass card
    p.setPen(QPen(QColor(60, 60, 100, 180), 1));
    p.setBrush(QColor(18, 18, 36, 230));
    p.drawRoundedRect(rect().adjusted(1,1,-1,-1), 12, 12);

    // Accent top border
    p.setPen(QPen(QColor("#00d4ff"), 2));
    p.drawLine(14, 1, width()-14, 1);
}
