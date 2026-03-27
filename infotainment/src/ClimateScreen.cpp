#include "ClimateScreen.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QLinearGradient>
#include <QConicalGradient>
#include <QRadialGradient>
#include <QWheelEvent>
#include <cmath>

// ===================== TempDial =====================
TempDial::TempDial(QWidget *parent, double initTemp, const QColor &accent)
    : QWidget(parent), m_temp(initTemp), m_accent(accent)
{
    setFixedSize(180, 180);
    setCursor(Qt::SizeVerCursor);
}

void TempDial::setTemperature(double t)
{
    m_temp = qBound(16.0, t, 30.0);
    emit temperatureChanged(m_temp);
    update();
}

void TempDial::wheelEvent(QWheelEvent *e)
{
    setTemperature(m_temp + (e->angleDelta().y() > 0 ? 0.5 : -0.5));
}

void TempDial::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QPointF center(width()/2.0, height()/2.0);
    double  radius = 78.0;

    // BG circle
    QRadialGradient bg(center, radius);
    bg.setColorAt(0, QColor(20,22,40));
    bg.setColorAt(1, QColor(12,12,24));
    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    p.drawEllipse(center, radius, radius);

    // Arc track
    QRectF arcRect(center.x()-radius+8, center.y()-radius+8, (radius-8)*2, (radius-8)*2);
    double startDeg = 210.0, spanDeg = -240.0;

    // Background track
    QPen trackPen(QColor(30,32,55), 10);
    trackPen.setCapStyle(Qt::RoundCap);
    p.setPen(trackPen);
    p.setBrush(Qt::NoBrush);
    p.drawArc(arcRect, (int)(startDeg*16), (int)(spanDeg*16));

    // Value arc
    double pct = (m_temp - 16.0) / (30.0 - 16.0);
    QPen valPen(m_accent, 10);
    valPen.setCapStyle(Qt::RoundCap);
    p.setPen(valPen);
    p.drawArc(arcRect, (int)(startDeg*16), (int)(spanDeg*pct*16));

    // Glow on value arc tip
    double tipDeg = startDeg + spanDeg * pct;
    double tipRad = qDegreesToRadians(tipDeg);
    double r2 = radius - 12;
    QPointF tip(center.x() + r2*std::cos(tipRad), center.y() - r2*std::sin(tipRad));
    QRadialGradient tipGlow(tip, 14);
    tipGlow.setColorAt(0, QColor(m_accent.red(),m_accent.green(),m_accent.blue(),160));
    tipGlow.setColorAt(1, Qt::transparent);
    p.setPen(Qt::NoPen);
    p.setBrush(tipGlow);
    p.drawEllipse(tip, 14.0, 14.0);

    // Temp text
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 26, QFont::Bold));
    p.drawText(QRectF(center.x()-40, center.y()-20, 80, 36), Qt::AlignCenter,
               QString::number(m_temp,'f',1));
    p.setPen(QColor(100,140,180));
    p.setFont(QFont("Arial", 11));
    p.drawText(QRectF(center.x()-20, center.y()+18, 40, 18), Qt::AlignCenter, "°C");
}

// ===================== ClimateScreen =====================
ClimateScreen::ClimateScreen(QWidget *parent) : QWidget(parent)
{
    setupUI();
    airAnimTimer = new QTimer(this);
    connect(airAnimTimer, &QTimer::timeout, this, [this]{
        airTick++;
        // animate EQ-style airflow bars
        QStringList frames = {"▁▃▅▇▅▃▁","▂▅▇▅▃▁▂","▃▇▅▃▁▂▃","▅▇▃▁▂▃▅","▇▅▁▂▃▅▇","▅▁▂▃▅▇▅"};
        if (airFlowLbl)
            airFlowLbl->setText("❄  " + frames[airTick % frames.size()] + "  ❄");
    });
    airAnimTimer->start(180);
}

void ClimateScreen::setupUI()
{
    auto *mainHL = new QHBoxLayout(this);
    mainHL->setContentsMargins(40, 24, 40, 24);
    mainHL->setSpacing(20);

    // ---------- Zone builder ----------
    auto makeZone = [&](const QString &label, double initTemp,
                         const QColor &accent, QLabel *&tempLbl,
                         TempDial *&dial) {
        auto *panel = new QWidget(this);
        panel->setStyleSheet("background:rgba(255,255,255,0.025); border-radius:18px;");
        auto *vl = new QVBoxLayout(panel);
        vl->setContentsMargins(20,20,20,20);
        vl->setSpacing(10);
        vl->setAlignment(Qt::AlignCenter);

        auto *zoneLbl = new QLabel(label, panel);
        zoneLbl->setStyleSheet("color:#555; font-size:11px; letter-spacing:2px;");
        zoneLbl->setAlignment(Qt::AlignCenter);

        dial = new TempDial(panel, initTemp, accent);
        tempLbl = new QLabel(QString::number(initTemp,'f',1) + " °C", panel);
        tempLbl->setStyleSheet(QString("color:%1; font-size:15px; font-weight:bold;").arg(accent.name()));
        tempLbl->setAlignment(Qt::AlignCenter);

        connect(dial, &TempDial::temperatureChanged, this, [tempLbl](double t){
            tempLbl->setText(QString::number(t,'f',1) + " °C");
        });

        // − + buttons
        auto *btnRow = new QHBoxLayout();
        btnRow->setSpacing(12);
        btnRow->setAlignment(Qt::AlignCenter);
        for (auto [lbl2, delta] : QList<std::pair<QString,double>>{{"-",-0.5},{"+",0.5}}) {
            auto *b = new QPushButton(lbl2, panel);
            b->setFixedSize(44,44);
            b->setCursor(Qt::PointingHandCursor);
            b->setStyleSheet(QString(R"(
                QPushButton{background:rgba(%1,%2,%3,0.12);border-radius:22px;
                            color:%4;font-size:22px;font-weight:bold;border:none;}
                QPushButton:hover{background:rgba(%1,%2,%3,0.25);}
            )").arg(accent.red()).arg(accent.green()).arg(accent.blue()).arg(accent.name()));
            double d = delta;
            connect(b, &QPushButton::clicked, dial, [dial,d]{ dial->setTemperature(dial->temperature()+d); });
            btnRow->addWidget(b);
        }

        vl->addWidget(zoneLbl);
        vl->addWidget(dial, 0, Qt::AlignCenter);
        vl->addWidget(tempLbl);
        vl->addLayout(btnRow);
        return panel;
    };

    mainHL->addWidget(makeZone("👤  DRIVER", 22.0, QColor("#00d4ff"), driverTempLbl, driverDial));

    // ---------- Center controls ----------
    auto *center = new QWidget(this);
    center->setFixedWidth(320);
    auto *cvl = new QVBoxLayout(center);
    cvl->setSpacing(12);
    cvl->setAlignment(Qt::AlignVCenter);

    // A/C toggle
    auto *acBtn = new QPushButton("❄   AUTO A/C", center);
    acBtn->setCheckable(true); acBtn->setChecked(true);
    acBtn->setFixedHeight(52);
    acBtn->setCursor(Qt::PointingHandCursor);
    acBtn->setStyleSheet(R"(
        QPushButton{background:rgba(0,212,255,0.12);border-radius:12px;
                    color:#00d4ff;font-size:15px;font-weight:bold;
                    border:1px solid rgba(0,212,255,0.30);}
        QPushButton:checked{background:rgba(0,212,255,0.25);border:1px solid #00d4ff;}
        QPushButton:hover{background:rgba(0,212,255,0.22);}
    )");
    cvl->addWidget(acBtn);

    // Fan speed
    auto *fanHdr = new QLabel("FAN SPEED", center);
    fanHdr->setStyleSheet("color:#555; font-size:10px; letter-spacing:2px;");
    fanHdr->setAlignment(Qt::AlignCenter);
    cvl->addWidget(fanHdr);

    auto *fanRow = new QHBoxLayout();
    fanRow->setSpacing(8);
    QList<QPushButton*> fanBtns;
    for (int i = 1; i <= 5; ++i) {
        auto *fb = new QPushButton(QString::number(i), center);
        fb->setFixedSize(48,48);
        fb->setCheckable(true);
        fb->setChecked(i == fanSpeed);
        fb->setCursor(Qt::PointingHandCursor);
        fb->setStyleSheet(R"(
            QPushButton{background:rgba(255,255,255,0.06);border-radius:24px;
                        color:#aaa;font-size:14px;border:none;}
            QPushButton:checked{background:#00d4ff;color:#0a0a18;font-weight:bold;}
            QPushButton:hover{background:rgba(0,212,255,0.20);}
        )");
        int spd = i;
        connect(fb, &QPushButton::clicked, this, [this, spd, &fanBtns](bool){
            fanSpeed = spd;
            for (int j = 0; j < fanBtns.size(); ++j)
                fanBtns[j]->setChecked(j+1 == spd);
        });
        fanRow->addWidget(fb);
        fanBtns.append(fb);
    }
    cvl->addLayout(fanRow);

    // Airflow modes
    auto *flowHdr = new QLabel("AIRFLOW MODE", center);
    flowHdr->setStyleSheet("color:#555; font-size:10px; letter-spacing:2px;");
    flowHdr->setAlignment(Qt::AlignCenter);
    cvl->addWidget(flowHdr);

    auto *flowRow = new QHBoxLayout();
    flowRow->setSpacing(8);
    struct FlowMode { QString icon, tip; };
    QList<FlowMode> modes = {{"💨","Face"},{"🦵","Feet"},{"💨↕","Both"},{"🌀","Windshield"},{"♻","Recirculate"}};
    QList<QPushButton*> flowBtns;
    for (int i = 0; i < modes.size(); ++i) {
        auto &m = modes[i];
        auto *b  = new QPushButton(m.icon, center);
        b->setFixedSize(52,52); b->setCheckable(true); b->setChecked(i==0);
        b->setToolTip(m.tip); b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(R"(
            QPushButton{background:rgba(255,255,255,0.05);border-radius:26px;
                        font-size:18px;border:none;}
            QPushButton:checked{background:rgba(0,212,255,0.18);
                                border:1px solid rgba(0,212,255,0.45);}
            QPushButton:hover{background:rgba(0,212,255,0.10);}
        )");
        int idx = i;
        connect(b, &QPushButton::clicked, this, [idx, &flowBtns](bool){
            for (int j = 0; j < flowBtns.size(); ++j)
                flowBtns[j]->setChecked(j==idx);
        });
        flowRow->addWidget(b);
        flowBtns.append(b);
    }
    cvl->addLayout(flowRow);

    // Animated airflow
    airFlowLbl = new QLabel("❄  ▁▃▅▇▅▃▁  ❄", center);
    airFlowLbl->setStyleSheet("color:#00d4ff; font-size:15px; letter-spacing:4px;");
    airFlowLbl->setAlignment(Qt::AlignCenter);
    cvl->addWidget(airFlowLbl);

    // Sync + Heated seats
    auto *extraRow = new QHBoxLayout();
    for (auto &[ic, lbl] : QList<std::pair<QString,QString>>{
         {"🔄","Sync Zones"}, {"🪑","Heated Seats"}}) {
        auto *b = new QPushButton(ic + "  " + lbl, center);
        b->setCheckable(true); b->setFixedHeight(40);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(R"(
            QPushButton{background:rgba(255,255,255,0.05);border-radius:10px;
                        color:#888;font-size:11px;border:none;}
            QPushButton:checked{background:rgba(255,170,0,0.15);color:#ffaa00;
                                border:1px solid rgba(255,170,0,0.30);}
            QPushButton:hover{background:rgba(255,255,255,0.08);color:#bbb;}
        )");
        extraRow->addWidget(b);
    }
    cvl->addLayout(extraRow);

    mainHL->addWidget(center);
    mainHL->addWidget(makeZone("👤  PASSENGER", 21.0, QColor("#ff8844"), passTempLbl, passDial));
}

void ClimateScreen::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QLinearGradient bg(0,0,0,height());
    bg.setColorAt(0, QColor(8,16,28));
    bg.setColorAt(1, QColor(6,10,20));
    p.fillRect(rect(), bg);

    // Ambient blue glow center
    QRadialGradient amb(width()/2, height()/2, 300);
    amb.setColorAt(0, QColor(0,150,255,15));
    amb.setColorAt(1, Qt::transparent);
    p.fillRect(rect(), amb);
}
