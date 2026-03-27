#include "HomeScreen.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QGraphicsDropShadowEffect>
#include <QDateTime>
#include <QProgressBar>
#include <functional>
#include <QPushButton>

HomeScreen::HomeScreen(QWidget *parent) : QWidget(parent)
{
    setupUI();
    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, [this]{
        timeLbl->setText(QDateTime::currentDateTime().toString("hh:mm"));
        dateLbl->setText(QDateTime::currentDateTime().toString("dddd, MMMM d"));
    });
    clockTimer->start(1000);
}

QWidget *HomeScreen::makeQuickCard(const QString &icon, const QString &title,
                                    const QString &subtitle, const QColor &accent,
                                    std::function<void()> onClick)
{
    auto *card = new QWidget(this);
    card->setCursor(Qt::PointingHandCursor);
    card->setFixedSize(168, 130);
    card->setStyleSheet(QString(R"(
        QWidget {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
                stop:0 rgba(%1,%2,%3,0.18), stop:1 rgba(%1,%2,%3,0.06));
            border-radius: 16px;
            border: 1px solid rgba(%1,%2,%3,0.30);
        }
        QWidget:hover {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
                stop:0 rgba(%1,%2,%3,0.30), stop:1 rgba(%1,%2,%3,0.12));
            border: 1px solid rgba(%1,%2,%3,0.55);
        }
    )").arg(accent.red()).arg(accent.green()).arg(accent.blue()));

    auto *vl = new QVBoxLayout(card);
    vl->setContentsMargins(16, 14, 16, 14);
    vl->setSpacing(6);

    auto *icoLbl = new QLabel(icon, card);
    icoLbl->setStyleSheet("font-size:30px; background:transparent; border:none;");

    auto *titleLbl2 = new QLabel(title, card);
    titleLbl2->setStyleSheet(QString("color:white; font-size:13px; font-weight:bold;"
                                      "background:transparent; border:none;"));

    auto *subLbl = new QLabel(subtitle, card);
    subLbl->setStyleSheet("color:#888; font-size:10px; background:transparent; border:none;");
    subLbl->setWordWrap(true);

    vl->addWidget(icoLbl);
    vl->addWidget(titleLbl2);
    vl->addWidget(subLbl);
    vl->addStretch();

    // Click via mouse press event using event filter trick with a lambda-based button overlay
    auto *btn = new QWidget(card);
    btn->setGeometry(0, 0, card->width(), card->height());
    btn->setAttribute(Qt::WA_TransparentForMouseEvents);

    // Install click via QPushButton trick
    card->installEventFilter(this); // fallback

    // Use QPushButton as transparent overlay
    auto *overlayBtn = new QPushButton(card);
    overlayBtn->setGeometry(0, 0, 168, 130);
    overlayBtn->setStyleSheet("QPushButton { background:transparent; border:none; }");
    overlayBtn->setCursor(Qt::PointingHandCursor);
    overlayBtn->raise();
    connect(overlayBtn, &QPushButton::clicked, this, onClick);

    return card;
}

QWidget *HomeScreen::makeWeatherWidget()
{
    auto *w = new QWidget(this);
    w->setStyleSheet(R"(
        QWidget { background:rgba(0,150,255,0.10); border-radius:16px;
                  border:1px solid rgba(0,150,255,0.22); }
    )");
    auto *hl = new QHBoxLayout(w);
    hl->setContentsMargins(18, 14, 18, 14);
    hl->setSpacing(16);

    auto *leftW = new QWidget(w);
    auto *leftL = new QVBoxLayout(leftW);
    leftL->setSpacing(3);
    leftL->setContentsMargins(0,0,0,0);

    auto *cityLbl = new QLabel("📍 Munich, Bavaria", leftW);
    cityLbl->setStyleSheet("color:#888; font-size:11px; background:transparent; border:none;");

    auto *tempBigLbl = new QLabel("18°C", leftW);
    tempBigLbl->setStyleSheet("color:white; font-size:38px; font-weight:bold; background:transparent; border:none;");

    auto *condLbl = new QLabel("🌤  Partly Cloudy", leftW);
    condLbl->setStyleSheet("color:#aac4ee; font-size:12px; background:transparent; border:none;");

    leftL->addWidget(cityLbl);
    leftL->addWidget(tempBigLbl);
    leftL->addWidget(condLbl);
    hl->addWidget(leftW);
    hl->addStretch();

    // Forecast mini
    auto *forecastW = new QWidget(w);
    auto *fhl = new QHBoxLayout(forecastW);
    fhl->setSpacing(14);
    fhl->setContentsMargins(0,0,0,0);

    struct Day { QString d, ic, t; };
    QList<Day> days = {{"Mon","🌤","17°"},{"Tue","🌧","13°"},{"Wed","☀️","22°"},{"Thu","⛅","19°"}};
    for (auto &day : days) {
        auto *col = new QWidget(forecastW);
        auto *cl  = new QVBoxLayout(col);
        cl->setSpacing(3); cl->setContentsMargins(0,0,0,0);
        cl->setAlignment(Qt::AlignCenter);
        auto *dl = new QLabel(day.d, col);
        dl->setStyleSheet("color:#666; font-size:9px; background:transparent; border:none;");
        dl->setAlignment(Qt::AlignCenter);
        auto *il = new QLabel(day.ic, col);
        il->setStyleSheet("font-size:16px; background:transparent; border:none;");
        il->setAlignment(Qt::AlignCenter);
        auto *tl = new QLabel(day.t, col);
        tl->setStyleSheet("color:#ccc; font-size:11px; font-weight:bold; background:transparent; border:none;");
        tl->setAlignment(Qt::AlignCenter);
        cl->addWidget(dl); cl->addWidget(il); cl->addWidget(tl);
        fhl->addWidget(col);
    }
    hl->addWidget(forecastW);
    return w;
}

QWidget *HomeScreen::makeNowPlayingWidget()
{
    auto *w = new QWidget(this);
    w->setStyleSheet(R"(
        QWidget { background:rgba(180,0,255,0.08); border-radius:16px;
                  border:1px solid rgba(180,0,255,0.22); }
    )");
    auto *hl = new QHBoxLayout(w);
    hl->setContentsMargins(14,12,14,12);
    hl->setSpacing(14);

    // Album art
    auto *art = new QLabel("🎵", w);
    art->setFixedSize(56, 56);
    art->setAlignment(Qt::AlignCenter);
    art->setStyleSheet(R"(
        QLabel { font-size:28px; background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                     stop:0 #ff6688, stop:1 #6644ff);
                 border-radius:10px; border:none; }
    )");

    auto *infoW = new QWidget(w);
    auto *vl    = new QVBoxLayout(infoW);
    vl->setSpacing(3); vl->setContentsMargins(0,0,0,0);

    auto *nowLbl = new QLabel("🎵  NOW PLAYING", infoW);
    nowLbl->setStyleSheet("color:#aa66ff; font-size:9px; letter-spacing:2px; background:transparent; border:none;");

    auto *trackLbl = new QLabel("Neon Highway", infoW);
    trackLbl->setStyleSheet("color:white; font-size:14px; font-weight:bold; background:transparent; border:none;");

    auto *artistLbl = new QLabel("Synthwave Collective", infoW);
    artistLbl->setStyleSheet("color:#888; font-size:11px; background:transparent; border:none;");

    auto *progress = new QProgressBar(infoW);
    progress->setRange(0, 100); progress->setValue(42);
    progress->setFixedHeight(3); progress->setTextVisible(false);
    progress->setStyleSheet(R"(
        QProgressBar { background:#2a1a3a; border-radius:2px; border:none; }
        QProgressBar::chunk { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,
            stop:0 #aa44ff, stop:1 #ff66cc); border-radius:2px; }
    )");

    vl->addWidget(nowLbl);
    vl->addWidget(trackLbl);
    vl->addWidget(artistLbl);
    vl->addWidget(progress);

    hl->addWidget(art);
    hl->addWidget(infoW, 1);

    // Controls
    auto *ctrlW = new QWidget(w);
    auto *cl    = new QHBoxLayout(ctrlW);
    cl->setSpacing(6); cl->setContentsMargins(0,0,0,0);
    cl->setAlignment(Qt::AlignCenter);

    for (auto &ic : QStringList{"⏮","⏸","⏭"}) {
        auto *b = new QPushButton(ic, ctrlW);
        b->setFixedSize(32,32);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(R"(
            QPushButton { background:rgba(255,255,255,0.08); border-radius:16px;
                          color:white; font-size:13px; border:none; }
            QPushButton:hover { background:rgba(180,0,255,0.25); }
        )");
        cl->addWidget(b);
    }
    hl->addWidget(ctrlW);
    return w;
}

QWidget *HomeScreen::makeRecentWidget()
{
    auto *w = new QWidget(this);
    w->setStyleSheet(R"(
        QWidget { background:rgba(0,255,136,0.06); border-radius:16px;
                  border:1px solid rgba(0,255,136,0.15); }
    )");
    auto *vl = new QVBoxLayout(w);
    vl->setContentsMargins(16,12,16,12); vl->setSpacing(8);

    auto *titleLbl2 = new QLabel("🗺️  RECENT DESTINATIONS", w);
    titleLbl2->setStyleSheet("color:#00ff88; font-size:10px; letter-spacing:2px; background:transparent; border:none;");
    vl->addWidget(titleLbl2);

    struct Dest { QString name, addr, time; };
    QList<Dest> dests = {
        {"BMW Welt",       "Am Olympiapark 2, Munich",    "Today"},
        {"REWE Market",    "Leopoldstraße 44, Munich",    "Yesterday"},
        {"Munich Airport", "Nordallee 25, Freising",      "Mon"},
    };
    for (auto &d : dests) {
        auto *row = new QWidget(w);
        row->setStyleSheet("background:rgba(255,255,255,0.03); border-radius:8px; border:none;");
        row->setCursor(Qt::PointingHandCursor);
        auto *rl = new QHBoxLayout(row);
        rl->setContentsMargins(10,7,10,7); rl->setSpacing(10);

        auto *pin = new QLabel("📍", row);
        pin->setStyleSheet("font-size:16px; background:transparent; border:none;");

        auto *info = new QWidget(row);
        auto *il   = new QVBoxLayout(info);
        il->setSpacing(1); il->setContentsMargins(0,0,0,0);
        auto *nL = new QLabel(d.name, info);
        nL->setStyleSheet("color:white; font-size:12px; font-weight:bold; background:transparent; border:none;");
        auto *aL = new QLabel(d.addr, info);
        aL->setStyleSheet("color:#666; font-size:10px; background:transparent; border:none;");
        il->addWidget(nL); il->addWidget(aL);

        auto *timeL = new QLabel(d.time, row);
        timeL->setStyleSheet("color:#444; font-size:10px; background:transparent; border:none;");

        rl->addWidget(pin);
        rl->addWidget(info, 1);
        rl->addWidget(timeL);
        vl->addWidget(row);
    }
    return w;
}

void HomeScreen::setupUI()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(24, 18, 24, 18);
    root->setSpacing(16);

    // ---- Greeting row ----
    auto *topRow = new QHBoxLayout();
    topRow->setSpacing(0);

    auto *greetCol = new QVBoxLayout();
    greetLbl = new QLabel("Good morning, Driver 👋", this);
    greetLbl->setStyleSheet("color:white; font-size:20px; font-weight:600;");

    // Update greeting by hour
    int h = QDateTime::currentDateTime().time().hour();
    QString greet = h < 12 ? "Good morning" : h < 18 ? "Good afternoon" : "Good evening";
    greetLbl->setText(greet + ", Driver 👋");

    dateLbl = new QLabel(QDateTime::currentDateTime().toString("dddd, MMMM d"), this);
    dateLbl->setStyleSheet("color:#555; font-size:12px; letter-spacing:0.5px;");

    timeLbl = new QLabel(QDateTime::currentDateTime().toString("hh:mm"), this);
    timeLbl->setStyleSheet("color:#00d4ff; font-size:42px; font-weight:bold; letter-spacing:2px;");
    timeLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    greetCol->addWidget(greetLbl);
    greetCol->addWidget(dateLbl);
    topRow->addLayout(greetCol);
    topRow->addStretch();
    topRow->addWidget(timeLbl);
    root->addLayout(topRow);

    // ---- Quick access cards ----
    auto *cardsRow = new QHBoxLayout();
    cardsRow->setSpacing(12);

    cardsRow->addWidget(makeQuickCard("🎵","Media","Neon Highway playing",
                                       QColor(170,0,255), [this]{ emit openMedia(); }));
    cardsRow->addWidget(makeQuickCard("🗺️","Navigate","BMW Welt • 8.3 km",
                                       QColor(0,212,255), [this]{ emit openNav(); }));
    cardsRow->addWidget(makeQuickCard("❄️","Climate","22°C • Auto",
                                       QColor(0,180,255), [this]{ emit openClimate(); }));
    cardsRow->addWidget(makeQuickCard("📱","Phone","iPhone connected",
                                       QColor(0,255,136), [this]{ emit openPhone(); }));

    // Fuel/Battery card
    auto *fuelCard = new QWidget(this);
    fuelCard->setFixedSize(168,130);
    fuelCard->setStyleSheet(R"(
        QWidget { background:rgba(255,170,0,0.10); border-radius:16px;
                  border:1px solid rgba(255,170,0,0.25); }
    )");
    auto *fvl = new QVBoxLayout(fuelCard);
    fvl->setContentsMargins(16,14,16,14); fvl->setSpacing(6);
    auto *fIco = new QLabel("⚡", fuelCard);
    fIco->setStyleSheet("font-size:28px; background:transparent; border:none;");
    auto *fTitle = new QLabel("Battery", fuelCard);
    fTitle->setStyleSheet("color:white; font-size:13px; font-weight:bold; background:transparent; border:none;");
    auto *fProg = new QProgressBar(fuelCard);
    fProg->setRange(0,100); fProg->setValue(87);
    fProg->setFixedHeight(6); fProg->setTextVisible(false);
    fProg->setStyleSheet(R"(
        QProgressBar{background:#2a1e00;border-radius:3px;border:none;}
        QProgressBar::chunk{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,
            stop:0 #ffaa00,stop:1 #ffee44);border-radius:3px;}
    )");
    auto *fSub = new QLabel("87%  •  312 km range", fuelCard);
    fSub->setStyleSheet("color:#ffaa44; font-size:10px; background:transparent; border:none;");
    fvl->addWidget(fIco); fvl->addWidget(fTitle); fvl->addWidget(fProg); fvl->addWidget(fSub);
    fvl->addStretch();
    cardsRow->addWidget(fuelCard);

    root->addLayout(cardsRow);

    // ---- Weather + Now Playing row ----
    auto *midRow = new QHBoxLayout();
    midRow->setSpacing(14);

    auto *weatherW = makeWeatherWidget();
    midRow->addWidget(weatherW, 3);

    auto *nowW = makeNowPlayingWidget();
    midRow->addWidget(nowW, 2);
    root->addLayout(midRow);

    // ---- Recent destinations ----
    root->addWidget(makeRecentWidget());
}

void HomeScreen::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QLinearGradient bg(0, 0, width(), height());
    bg.setColorAt(0, QColor(10, 10, 22));
    bg.setColorAt(1, QColor(8,  8,  18));
    p.fillRect(rect(), bg);

    // Subtle ambient glow top-left
    QRadialGradient glow(0, 0, 400);
    glow.setColorAt(0, QColor(0, 80, 180, 30));
    glow.setColorAt(1, Qt::transparent);
    p.fillRect(rect(), glow);
}
