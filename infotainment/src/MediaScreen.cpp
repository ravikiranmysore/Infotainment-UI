#include "MediaScreen.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QGraphicsDropShadowEffect>

MediaScreen::MediaScreen(QWidget *parent) : QWidget(parent)
{
    tracks = {
        {"Neon Highway",      "Synthwave Collective", "Drive",      "3:42", "🎸", QColor(255,102,136)},
        {"Midnight Cruise",   "The Dark Knights",     "Night Rider","4:15", "🌙", QColor(100,68,255)},
        {"City Lights",       "Urban Flow",           "Metropolis", "3:28", "🌆", QColor(0,212,255)},
        {"Desert Storm",      "Analog Dreams",        "Horizons",   "5:01", "🏜️",  QColor(255,136,0)},
        {"Electric Soul",     "NightCar",             "Pulse",      "3:55", "⚡", QColor(0,255,136)},
        {"Turbo Charge",      "BPM Masters",          "Overdrive",  "4:22", "🔥", QColor(255,68,85)},
        {"Glass Horizon",     "Echo Chamber",         "Reflect",    "3:10", "🔮", QColor(180,0,255)},
        {"Solar Drift",       "Cosmos Rides",         "Nebula",     "4:48", "🌌", QColor(0,180,255)},
    };

    progressTimer = new QTimer(this);
    connect(progressTimer, &QTimer::timeout, this, &MediaScreen::tickProgress);

    setupUI();
    loadTrack(0);
}

void MediaScreen::setupUI()
{
    auto *mainHL = new QHBoxLayout(this);
    mainHL->setContentsMargins(0, 0, 0, 0);
    mainHL->setSpacing(0);

    // === Left player panel ===
    auto *playerPanel = new QWidget(this);
    playerPanel->setFixedWidth(480);
    playerPanel->setStyleSheet("background: rgba(0,0,0,0.25);");
    auto *pvl = new QVBoxLayout(playerPanel);
    pvl->setContentsMargins(30, 24, 30, 24);
    pvl->setSpacing(18);

    // Source selector
    auto *sourceRow = new QHBoxLayout();
    sourceRow->setSpacing(8);
    QStringList sources = {"🎵 Library", "📻 Radio", "🎙 Podcast", "📲 Bluetooth"};
    for (int i = 0; i < sources.size(); ++i) {
        auto *btn = new QPushButton(sources[i], playerPanel);
        btn->setCheckable(true);
        btn->setChecked(i == 0);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(32);
        btn->setStyleSheet(R"(
            QPushButton { background:rgba(255,255,255,0.06); border-radius:8px;
                          color:#888; font-size:11px; border:none; padding:0 10px; }
            QPushButton:checked { background:rgba(0,212,255,0.18); color:#00d4ff;
                                  border:1px solid rgba(0,212,255,0.35); }
            QPushButton:hover { background:rgba(255,255,255,0.10); color:#ccc; }
        )");
        int idx = i;
        connect(btn, &QPushButton::clicked, this, [this, idx](){ switchSource(idx); });
        srcBtns.append(btn);
        sourceRow->addWidget(btn);
    }
    pvl->addLayout(sourceRow);

    // Cover art
    coverLbl = new QLabel(playerPanel);
    coverLbl->setFixedSize(200, 200);
    coverLbl->setAlignment(Qt::AlignCenter);
    coverLbl->setStyleSheet("font-size:72px; border-radius:20px; border:none;");

    auto *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(50);
    shadow->setOffset(0, 10);
    shadow->setColor(QColor(255, 100, 136, 140));
    coverLbl->setGraphicsEffect(shadow);

    coverPulse = new QPropertyAnimation(coverLbl, "minimumSize", this);
    coverPulse->setDuration(800);
    coverPulse->setStartValue(QSize(200, 200));
    coverPulse->setEndValue(QSize(194, 194));
    coverPulse->setEasingCurve(QEasingCurve::InOutSine);
    coverPulse->setLoopCount(-1);

    pvl->addWidget(coverLbl, 0, Qt::AlignCenter);

    // Track info
    titleLbl = new QLabel(playerPanel);
    titleLbl->setStyleSheet("color:white; font-size:22px; font-weight:bold;");
    titleLbl->setAlignment(Qt::AlignCenter);

    artistLbl = new QLabel(playerPanel);
    artistLbl->setStyleSheet("color:#aaa; font-size:13px;");
    artistLbl->setAlignment(Qt::AlignCenter);

    albumLbl = new QLabel(playerPanel);
    albumLbl->setStyleSheet("color:#555; font-size:11px; letter-spacing:1px;");
    albumLbl->setAlignment(Qt::AlignCenter);

    pvl->addWidget(titleLbl);
    pvl->addWidget(artistLbl);
    pvl->addWidget(albumLbl);

    // Seek row
    auto *seekRow = new QHBoxLayout();
    seekRow->setSpacing(8);
    timeLbl = new QLabel("0:00", playerPanel);
    timeLbl->setStyleSheet("color:#666; font-size:11px;");
    durLbl  = new QLabel("0:00", playerPanel);
    durLbl->setStyleSheet("color:#666; font-size:11px;");

    seekSlider = new QSlider(Qt::Horizontal, playerPanel);
    seekSlider->setRange(0, 1000);
    seekSlider->setStyleSheet(R"(
        QSlider::groove:horizontal { height:4px; background:#2a2a44; border-radius:2px; }
        QSlider::handle:horizontal { width:16px; height:16px; margin:-6px 0;
            background:white; border-radius:8px; }
        QSlider::sub-page:horizontal { background:#00d4ff; border-radius:2px; }
    )");
    seekRow->addWidget(timeLbl);
    seekRow->addWidget(seekSlider, 1);
    seekRow->addWidget(durLbl);
    pvl->addLayout(seekRow);

    // Controls
    auto *ctrlRow = new QHBoxLayout();
    ctrlRow->setSpacing(14);
    ctrlRow->setAlignment(Qt::AlignCenter);

    auto mkBtn = [&](const QString &ic, int sz, bool accent = false) {
        auto *b = new QPushButton(ic, playerPanel);
        b->setFixedSize(sz, sz);
        b->setCursor(Qt::PointingHandCursor);
        if (accent) {
            b->setStyleSheet(QString(R"(
                QPushButton{background:#00d4ff;border-radius:%1px;color:#0a0a18;
                            font-size:%2px;font-weight:bold;border:none;}
                QPushButton:hover{background:#33ddff;}
                QPushButton:pressed{background:#0099cc;}
            )").arg(sz/2).arg(sz/3));
        } else {
            b->setStyleSheet(QString(R"(
                QPushButton{background:rgba(255,255,255,0.07);border-radius:%1px;
                            color:white;font-size:%2px;border:none;}
                QPushButton:hover{background:rgba(0,212,255,0.18);}
                QPushButton:pressed{background:rgba(0,212,255,0.30);}
            )").arg(sz/2).arg(sz/3));
        }
        return b;
    };

    auto *shuffleBtn = mkBtn("🔀", 38);
    auto *prevBtn    = mkBtn("⏮", 46);
    playBtn          = mkBtn("▶", 60, true);
    auto *nextBtn    = mkBtn("⏭", 46);
    auto *repeatBtn  = mkBtn("🔁", 38);

    connect(prevBtn,  &QPushButton::clicked, this, &MediaScreen::prevTrack);
    connect(playBtn,  &QPushButton::clicked, this, &MediaScreen::playPause);
    connect(nextBtn,  &QPushButton::clicked, this, &MediaScreen::nextTrack);

    ctrlRow->addWidget(shuffleBtn);
    ctrlRow->addWidget(prevBtn);
    ctrlRow->addWidget(playBtn);
    ctrlRow->addWidget(nextBtn);
    ctrlRow->addWidget(repeatBtn);
    pvl->addLayout(ctrlRow);

    // Volume
    auto *volRow = new QHBoxLayout();
    volRow->setSpacing(10);
    auto *volIco = new QLabel("🔊", playerPanel);
    volIco->setStyleSheet("font-size:16px;");
    auto *volSlider = new QSlider(Qt::Horizontal, playerPanel);
    volSlider->setRange(0,100); volSlider->setValue(72);
    volSlider->setStyleSheet(seekSlider->styleSheet());
    auto *volPct = new QLabel("72%", playerPanel);
    volPct->setStyleSheet("color:#666; font-size:11px; min-width:32px;");
    connect(volSlider, &QSlider::valueChanged, this, [volPct](int v){
        volPct->setText(QString("%1%").arg(v));
    });
    volRow->addWidget(volIco);
    volRow->addWidget(volSlider, 1);
    volRow->addWidget(volPct);
    pvl->addLayout(volRow);

    mainHL->addWidget(playerPanel);

    // Divider
    auto *div = new QWidget(this);
    div->setFixedWidth(1);
    div->setStyleSheet("background: rgba(255,255,255,0.07);");
    mainHL->addWidget(div);

    // === Right: Playlist + Equalizer ===
    auto *rightPanel = new QWidget(this);
    auto *rvl = new QVBoxLayout(rightPanel);
    rvl->setContentsMargins(20, 20, 20, 20);
    rvl->setSpacing(10);

    auto *plTitle = new QLabel("📋  QUEUE", rightPanel);
    plTitle->setStyleSheet("color:#555; font-size:10px; letter-spacing:3px;");
    rvl->addWidget(plTitle);

    playlist = new QListWidget(rightPanel);
    playlist->setStyleSheet(R"(
        QListWidget { background:transparent; border:none; outline:none; }
        QListWidget::item { color:#ccc; font-size:12px; padding:10px 14px;
                            border-radius:10px; margin:2px 0; }
        QListWidget::item:hover { background:rgba(255,255,255,0.05); }
        QListWidget::item:selected { background:rgba(0,212,255,0.10);
                                     color:white;
                                     border-left:3px solid #00d4ff; }
    )");
    for (int i = 0; i < tracks.size(); ++i) {
        auto &t = tracks[i];
        playlist->addItem(QString("%1  %2  •  %3  %4")
                              .arg(t.emoji).arg(t.title).arg(t.artist).arg(t.dur));
    }
    connect(playlist, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item){
        loadTrack(playlist->row(item));
        if (!playing) playPause();
    });
    rvl->addWidget(playlist, 1);

    // EQ mini visualizer (static decorative bars)
    auto *eqWidget = new QWidget(rightPanel);
    eqWidget->setFixedHeight(48);
    eqWidget->setStyleSheet("background:rgba(255,255,255,0.02); border-radius:10px;");
    auto *eqHL = new QHBoxLayout(eqWidget);
    eqHL->setContentsMargins(12,6,12,6); eqHL->setSpacing(4);

    auto *eqLbl = new QLabel("EQUALIZER  ▊▆▇▅▋▇▆▊▅▇", eqWidget);
    eqLbl->setStyleSheet("color:#00d4ff; font-size:12px; letter-spacing:3px;");
    eqLbl->setAlignment(Qt::AlignCenter);
    eqHL->addWidget(eqLbl);
    rvl->addWidget(eqWidget);

    mainHL->addWidget(rightPanel, 1);
}

void MediaScreen::loadTrack(int idx)
{
    currentTrack = idx;
    const Track &t = tracks[idx];

    titleLbl->setText(t.title);
    artistLbl->setText(t.artist);
    albumLbl->setText(t.album.toUpper());
    durLbl->setText(t.dur);

    auto parts = t.dur.split(':');
    totalSec   = parts[0].toInt() * 60 + parts[1].toInt();
    progressSec = 0;
    seekSlider->setValue(0);
    timeLbl->setText("0:00");

    coverLbl->setStyleSheet(QString(R"(
        QLabel { font-size:72px; border-radius:20px; border:none;
                 background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                     stop:0 %1, stop:1 %2); }
    )").arg(t.color.name()).arg(t.color.darker(200).name()));
    coverLbl->setText(t.emoji);

    if (auto *fx = dynamic_cast<QGraphicsDropShadowEffect*>(coverLbl->graphicsEffect()))
        fx->setColor(QColor(t.color.red(), t.color.green(), t.color.blue(), 140));

    playlist->setCurrentRow(idx);
}

void MediaScreen::playPause()
{
    playing = !playing;
    playBtn->setText(playing ? "⏸" : "▶");
    if (playing) { progressTimer->start(1000); coverPulse->start(); }
    else         { progressTimer->stop();      coverPulse->stop();  }
}

void MediaScreen::nextTrack() { loadTrack((currentTrack + 1) % tracks.size()); }
void MediaScreen::prevTrack() { loadTrack((currentTrack - 1 + tracks.size()) % tracks.size()); }

void MediaScreen::tickProgress()
{
    if (++progressSec >= totalSec) { nextTrack(); return; }
    int m = progressSec/60, s = progressSec%60;
    timeLbl->setText(QString("%1:%2").arg(m).arg(s,2,10,QChar('0')));
    seekSlider->setValue(progressSec * 1000 / totalSec);
}

void MediaScreen::switchSource(int idx)
{
    for (int i = 0; i < srcBtns.size(); ++i)
        srcBtns[i]->setChecked(i == idx);
}

void MediaScreen::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QLinearGradient bg(0, 0, width(), height());
    bg.setColorAt(0, QColor(12, 8, 24));
    bg.setColorAt(1, QColor(8, 10, 20));
    p.fillRect(rect(), bg);
}
