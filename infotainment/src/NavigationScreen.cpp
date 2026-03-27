#include "NavigationScreen.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <cmath>

NavigationScreen::NavigationScreen(QWidget *parent) : QWidget(parent)
{
    routePts = {
        {0.08,0.78},{0.14,0.65},{0.20,0.55},{0.28,0.47},{0.35,0.42},
        {0.42,0.40},{0.50,0.38},{0.57,0.34},{0.63,0.28},{0.70,0.24},
        {0.76,0.28},{0.82,0.22},{0.88,0.16}
    };
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &NavigationScreen::animateTick);
    animTimer->start(40);
    setupUI();
}

void NavigationScreen::setupUI()
{
    // Sidebar on right
    auto *hl = new QHBoxLayout(this);
    hl->setContentsMargins(0,0,0,0);
    hl->setSpacing(0);
    hl->addStretch(1); // map area (painted directly)

    auto *sidebar = new QWidget(this);
    sidebar->setFixedWidth(sidebarW);
    sidebar->setStyleSheet("background:rgba(8,8,20,0.93); border-left:1px solid #1a1a32;");
    auto *svl = new QVBoxLayout(sidebar);
    svl->setContentsMargins(14,14,14,14);
    svl->setSpacing(10);

    // Search
    searchEdit = new QLineEdit(sidebar);
    searchEdit->setPlaceholderText("🔍  Search destination...");
    searchEdit->setFixedHeight(40);
    searchEdit->setStyleSheet(R"(
        QLineEdit { background:rgba(255,255,255,0.07); border-radius:10px;
                    color:white; font-size:12px; padding:0 14px;
                    border:1px solid #2a2a44; }
        QLineEdit:focus { border:1px solid #00d4ff; }
    )");
    svl->addWidget(searchEdit);

    // Active route card
    auto *routeCard = new QWidget(sidebar);
    routeCard->setStyleSheet("background:rgba(0,212,255,0.08); border-radius:12px; border:1px solid rgba(0,212,255,0.2);");
    auto *rcl = new QVBoxLayout(routeCard);
    rcl->setContentsMargins(12,12,12,12); rcl->setSpacing(5);
    auto *destHdr = new QLabel("📍  DESTINATION", routeCard);
    destHdr->setStyleSheet("color:#555; font-size:9px; letter-spacing:2px;");
    auto *destName = new QLabel("BMW Welt, Munich", routeCard);
    destName->setStyleSheet("color:white; font-size:14px; font-weight:bold;");
    auto *sep = new QWidget(routeCard);
    sep->setFixedHeight(1); sep->setStyleSheet("background:rgba(0,212,255,0.18);");
    etaLbl  = new QLabel("🕐  ETA: 12 min", routeCard);
    etaLbl->setStyleSheet("color:#00d4ff; font-size:13px; font-weight:bold;");
    distLbl = new QLabel("📏  8.3 km remaining", routeCard);
    distLbl->setStyleSheet("color:#aaa; font-size:11px;");
    rcl->addWidget(destHdr);
    rcl->addWidget(destName);
    rcl->addWidget(sep);
    rcl->addWidget(etaLbl);
    rcl->addWidget(distLbl);
    svl->addWidget(routeCard);

    // Next turn card
    auto *turnCard = new QWidget(sidebar);
    turnCard->setStyleSheet("background:rgba(255,200,0,0.07); border-radius:12px; border:1px solid rgba(255,200,0,0.20);");
    auto *tcl = new QHBoxLayout(turnCard);
    tcl->setContentsMargins(12,10,12,10); tcl->setSpacing(12);
    auto *arrowLbl = new QLabel("↱", turnCard);
    arrowLbl->setStyleSheet("color:#ffcc00; font-size:40px; font-weight:bold;");
    auto *tInfo = new QWidget(turnCard);
    auto *til   = new QVBoxLayout(tInfo);
    til->setSpacing(2); til->setContentsMargins(0,0,0,0);
    turnLbl   = new QLabel("Turn right in", tInfo);
    turnLbl->setStyleSheet("color:#888; font-size:10px;");
    streetLbl = new QLabel("Olympiapark Str.", tInfo);
    streetLbl->setStyleSheet("color:white; font-size:13px; font-weight:bold;");
    auto *dist2Turn = new QLabel("in 320 m", tInfo);
    dist2Turn->setStyleSheet("color:#ffcc00; font-size:12px;");
    til->addWidget(turnLbl); til->addWidget(streetLbl); til->addWidget(dist2Turn);
    tcl->addWidget(arrowLbl); tcl->addWidget(tInfo, 1);
    svl->addWidget(turnCard);

    // Speed limit sign
    auto *slRow = new QHBoxLayout();
    auto *slSign = new QWidget(sidebar);
    slSign->setFixedSize(60, 60);
    slSign->setStyleSheet("background:white; border-radius:30px; border:4px solid #dd0000;");
    auto *slVl = new QVBoxLayout(slSign);
    slVl->setContentsMargins(0,0,0,0);
    speedLimitLbl = new QLabel("80", slSign);
    speedLimitLbl->setStyleSheet("color:#cc0000; font-size:20px; font-weight:bold; background:transparent; border:none;");
    speedLimitLbl->setAlignment(Qt::AlignCenter);
    slVl->addWidget(speedLimitLbl);
    auto *slInfo = new QWidget(sidebar);
    auto *sil    = new QVBoxLayout(slInfo);
    sil->setSpacing(2); sil->setContentsMargins(0,0,0,0);
    auto *slTitle = new QLabel("Speed Limit", slInfo);
    slTitle->setStyleSheet("color:#666; font-size:10px;");
    auto *slVal   = new QLabel("80 km/h zone", slInfo);
    slVal->setStyleSheet("color:white; font-size:13px; font-weight:bold;");
    sil->addWidget(slTitle); sil->addWidget(slVal);
    slRow->addWidget(slSign);
    slRow->addWidget(slInfo);
    slRow->addStretch();
    svl->addLayout(slRow);

    svl->addStretch();

    // Action buttons
    auto *altBtn = new QPushButton("🔄  Alternative Route", sidebar);
    altBtn->setFixedHeight(38);
    altBtn->setCursor(Qt::PointingHandCursor);
    altBtn->setStyleSheet(R"(
        QPushButton{background:rgba(255,255,255,0.05);border-radius:10px;
                    color:#aaa;font-size:12px;border:none;}
        QPushButton:hover{background:rgba(255,255,255,0.10);color:white;}
    )");
    auto *stopBtn = new QPushButton("⏹  Stop Navigation", sidebar);
    stopBtn->setFixedHeight(38);
    stopBtn->setCursor(Qt::PointingHandCursor);
    stopBtn->setStyleSheet(R"(
        QPushButton{background:rgba(255,50,80,0.10);border-radius:10px;
                    color:#ff5577;font-size:12px;border:1px solid rgba(255,50,80,0.22);}
        QPushButton:hover{background:rgba(255,50,80,0.22);}
    )");
    svl->addWidget(altBtn);
    svl->addWidget(stopBtn);

    hl->addWidget(sidebar);
}

void NavigationScreen::animateTick()
{
    tick++;
    carT = std::fmod(carT + 0.003, 1.0);
    update();
}

void NavigationScreen::resizeEvent(QResizeEvent *e) { QWidget::resizeEvent(e); update(); }

// ---- Drawing helpers ----
void NavigationScreen::drawGrid(QPainter &p, int mw, int mh)
{
    p.setPen(QPen(QColor(20, 22, 38), 1));
    for (int x = 0; x < mw; x += 36) p.drawLine(x,0,x,mh);
    for (int y = 0; y < mh; y += 36) p.drawLine(0,y,mw,y);
}

void NavigationScreen::drawRoads(QPainter &p, int mw, int mh)
{
    // Major roads
    QPen major(QColor(28,32,54), 16);
    major.setCapStyle(Qt::RoundCap);
    p.setPen(major);
    for (int y = 55; y < mh; y += 90) p.drawLine(0,y,mw,y);
    for (int x = 72; x < mw; x += 110) p.drawLine(x,0,x,mh);

    // Road markings
    QPen dash(QColor(38,45,72), 2, Qt::DashLine);
    dash.setDashPattern({8,16});
    p.setPen(dash);
    for (int y = 55; y < mh; y += 90) p.drawLine(0,y,mw,y);
    for (int x = 72; x < mw; x += 110) p.drawLine(x,0,x,mh);
}

void NavigationScreen::drawBuildings(QPainter &p, int mw, int mh)
{
    p.setPen(QPen(QColor(26,28,48),1));
    struct B { int x,y,w,h; };
    QList<B> blds = {
        {8,8,55,65},{72,8,50,45},{140,20,65,55},{240,8,55,50},
        {8,80,45,60},{140,90,60,55},{260,85,70,50},
        {8,190,60,50},{120,200,55,60},{220,185,65,55},
        {8,290,70,45},{130,295,60,50},{250,280,75,55},
        {8,390,55,60},{150,385,65,55},{270,375,60,65},
    };
    p.setBrush(QColor(18, 20, 36));
    for (auto &b : blds)
        p.drawRoundedRect(b.x, b.y, b.w, b.h, 3, 3);

    // Parks
    p.setBrush(QColor(14, 32, 18));
    p.setPen(QPen(QColor(18,40,22),1));
    p.drawRoundedRect(300, 50, 110, 80, 8, 8);
    p.drawRoundedRect(440, 200, 90, 70, 8, 8);
    p.drawRoundedRect(340, 320, 80, 60, 8, 8);
}

void NavigationScreen::drawRoute(QPainter &p, int mw, int mh)
{
    if (routePts.size() < 2) return;

    QList<QPointF> pts;
    for (auto &rp : routePts) pts << QPointF(rp.x()*mw, rp.y()*mh);

    QPainterPath path;
    path.moveTo(pts[0]);
    for (int i = 1; i+1 < pts.size(); ++i)
        path.quadTo(pts[i], (pts[i]+pts[i+1])/2.0);
    path.lineTo(pts.last());

    // Outer glow
    QPen glow(QColor(0,212,255,35)); glow.setWidth(22);
    glow.setCapStyle(Qt::RoundCap); glow.setJoinStyle(Qt::RoundJoin);
    p.setPen(glow); p.setBrush(Qt::NoBrush);
    p.drawPath(path);

    // Mid glow
    QPen glow2(QColor(0,212,255,70)); glow2.setWidth(12);
    glow2.setCapStyle(Qt::RoundCap); glow2.setJoinStyle(Qt::RoundJoin);
    p.setPen(glow2); p.drawPath(path);

    // Route line
    QPen line(QColor("#00d4ff")); line.setWidth(5);
    line.setCapStyle(Qt::RoundCap); line.setJoinStyle(Qt::RoundJoin);
    p.setPen(line); p.drawPath(path);

    // Destination pin
    QPointF dest = pts.last();
    QRadialGradient dg(dest, 18);
    dg.setColorAt(0, QColor(0,212,255,160)); dg.setColorAt(1,Qt::transparent);
    p.setBrush(dg); p.setPen(Qt::NoPen);
    p.drawEllipse(dest, 18.0, 18.0);
    p.setBrush(QColor("#00d4ff")); p.drawEllipse(dest, 6.0, 6.0);

    p.setFont(QFont("Arial",18));
    p.drawText(QPointF(dest.x()-10, dest.y()-14), "📍");
}

void NavigationScreen::drawCar(QPainter &p, int mw, int mh)
{
    int n = routePts.size();
    double prog = carT * (n-1);
    int idx = qMin((int)prog, n-2);
    double frac = prog - idx;

    QPointF a(routePts[idx].x()*mw,   routePts[idx].y()*mh);
    QPointF b(routePts[idx+1].x()*mw, routePts[idx+1].y()*mh);
    QPointF pos = a + (b-a)*frac;

    double angle = std::atan2(b.y()-a.y(), b.x()-a.x()) * 180.0/M_PI;

    // Glow
    QRadialGradient cg(pos, 24);
    cg.setColorAt(0, QColor(0,212,255,90)); cg.setColorAt(1,Qt::transparent);
    p.setBrush(cg); p.setPen(Qt::NoPen);
    p.drawEllipse(pos, 24.0, 24.0);

    // Car arrow
    p.save();
    p.translate(pos);
    p.rotate(angle);
    p.setBrush(QColor("#00d4ff")); p.setPen(Qt::NoPen);
    QPolygonF car; car << QPointF(16,0) << QPointF(-10,8) << QPointF(-5,0) << QPointF(-10,-8);
    p.drawPolygon(car);
    p.restore();
}

void NavigationScreen::drawPOIs(QPainter &p, int mw, int mh)
{
    struct POI { double nx,ny; QString ic; };
    QList<POI> pois = {
        {0.28,0.60,"⛽"},{0.50,0.52,"🏨"},{0.38,0.28,"🅿"},
        {0.65,0.50,"☕"},{0.18,0.38,"🍕"},{0.72,0.65,"🛒"},
    };
    p.setFont(QFont("Arial",15));
    for (auto &poi : pois)
        p.drawText(QRectF(poi.nx*mw-12, poi.ny*mh-12, 24, 24), Qt::AlignCenter, poi.ic);
}

void NavigationScreen::drawOverlayHUD(QPainter &p, int mw, int mh)
{
    Q_UNUSED(mh);
    // Zoom controls
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(10,10,22,200));
    p.drawRoundedRect(mw-46, 10, 36, 80, 10, 10);
    p.setPen(QColor(100,100,160));
    p.setFont(QFont("Arial",16,QFont::Bold));
    p.drawText(QRect(mw-46,10,36,40), Qt::AlignCenter, "+");
    p.drawText(QRect(mw-46,50,36,40), Qt::AlignCenter, "−");

    // Compass
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(10,10,22,200));
    p.drawEllipse(mw-46, 100, 36, 36);
    p.setPen(QColor("#ff3355"));
    p.setFont(QFont("Arial",10,QFont::Bold));
    p.drawText(QRect(mw-46,100,36,36), Qt::AlignCenter, "N");

    // Scale bar
    p.setPen(QPen(Qt::white, 2));
    p.drawLine(10, mh-20, 60, mh-20);
    p.drawLine(10, mh-24, 10, mh-16);
    p.drawLine(60, mh-24, 60, mh-16);
    p.setPen(Qt::white);
    p.setFont(QFont("Arial",9));
    p.drawText(QRect(10, mh-38, 50, 16), Qt::AlignCenter, "500 m");
}

void NavigationScreen::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    p.setRenderHint(QPainter::SmoothPixmapTransform,true);

    int mw = width() - sidebarW;
    int mh = height();

    // Map background
    p.fillRect(0, 0, mw, mh, QColor(12, 14, 26));

    drawGrid(p, mw, mh);
    drawRoads(p, mw, mh);
    drawBuildings(p, mw, mh);
    drawRoute(p, mw, mh);
    drawPOIs(p, mw, mh);
    drawCar(p, mw, mh);
    drawOverlayHUD(p, mw, mh);
}
