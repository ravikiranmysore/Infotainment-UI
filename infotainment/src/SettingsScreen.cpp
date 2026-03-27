#include "SettingsScreen.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QLinearGradient>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QScrollArea>

// ── Helpers ──────────────────────────────────────────────────────────────────
static QWidget *makeSeparator(QWidget *parent)
{
    auto *sep = new QWidget(parent);
    sep->setFixedHeight(1);
    sep->setStyleSheet("background: rgba(255,255,255,0.07);");
    return sep;
}

static QWidget *makeToggleRow(QWidget *parent, const QString &icon,
                               const QString &label, const QString &sub,
                               bool checked = false)
{
    auto *row = new QWidget(parent);
    row->setStyleSheet(R"(
        QWidget { background:transparent; }
        QWidget:hover { background: rgba(255,255,255,0.03); border-radius:8px; }
    )");
    auto *hl = new QHBoxLayout(row);
    hl->setContentsMargins(8, 10, 8, 10);
    hl->setSpacing(14);

    auto *icoLbl = new QLabel(icon, row);
    icoLbl->setStyleSheet("font-size:20px; min-width:28px;");
    icoLbl->setAlignment(Qt::AlignCenter);

    auto *textW = new QWidget(row);
    auto *tvl   = new QVBoxLayout(textW);
    tvl->setSpacing(2); tvl->setContentsMargins(0,0,0,0);
    auto *lbl   = new QLabel(label, textW);
    lbl->setStyleSheet("color:white; font-size:13px;");
    auto *sub2  = new QLabel(sub, textW);
    sub2->setStyleSheet("color:#555; font-size:10px;");
    tvl->addWidget(lbl); tvl->addWidget(sub2);

    // Toggle switch (simulated with QPushButton)
    auto *toggle = new QPushButton(row);
    toggle->setCheckable(true);
    toggle->setChecked(checked);
    toggle->setFixedSize(48, 26);
    toggle->setCursor(Qt::PointingHandCursor);
    auto updateToggle = [toggle]() {
        bool on = toggle->isChecked();
        toggle->setStyleSheet(QString(R"(
            QPushButton { background:%1; border-radius:13px; border:none; }
        )").arg(on ? "#00d4ff" : "#2a2a44"));
    };
    updateToggle();
    QObject::connect(toggle, &QPushButton::toggled, [updateToggle](bool){ updateToggle(); });

    hl->addWidget(icoLbl);
    hl->addWidget(textW, 1);
    hl->addWidget(toggle);
    return row;
}

static QWidget *makeSliderRow(QWidget *parent, const QString &icon,
                               const QString &label, int value,
                               const QString &accentColor = "#00d4ff")
{
    auto *row = new QWidget(parent);
    row->setStyleSheet("QWidget { background:transparent; }");
    auto *hl = new QHBoxLayout(row);
    hl->setContentsMargins(8, 8, 8, 8);
    hl->setSpacing(14);

    auto *icoLbl = new QLabel(icon, row);
    icoLbl->setStyleSheet("font-size:20px; min-width:28px;");
    icoLbl->setAlignment(Qt::AlignCenter);

    auto *lbl = new QLabel(label, row);
    lbl->setStyleSheet("color:white; font-size:13px; min-width:100px;");

    auto *slider = new QSlider(Qt::Horizontal, row);
    slider->setRange(0, 100);
    slider->setValue(value);
    slider->setStyleSheet(QString(R"(
        QSlider::groove:horizontal { height:4px; background:#2a2a44; border-radius:2px; }
        QSlider::handle:horizontal { width:16px; height:16px; margin:-6px 0;
            background:white; border-radius:8px; }
        QSlider::sub-page:horizontal { background:%1; border-radius:2px; }
    )").arg(accentColor));

    auto *valLbl = new QLabel(QString::number(value) + "%", row);
    valLbl->setStyleSheet("color:#555; font-size:11px; min-width:36px;");
    valLbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    QObject::connect(slider, &QSlider::valueChanged, [valLbl](int v){
        valLbl->setText(QString::number(v) + "%");
    });

    hl->addWidget(icoLbl);
    hl->addWidget(lbl);
    hl->addWidget(slider, 1);
    hl->addWidget(valLbl);
    return row;
}

static QWidget *makeSelectRow(QWidget *parent, const QString &icon,
                               const QString &label, const QStringList &opts,
                               int currentIdx = 0)
{
    auto *row = new QWidget(parent);
    row->setStyleSheet("QWidget { background:transparent; }");
    auto *hl = new QHBoxLayout(row);
    hl->setContentsMargins(8, 8, 8, 8);
    hl->setSpacing(14);

    auto *icoLbl = new QLabel(icon, row);
    icoLbl->setStyleSheet("font-size:20px; min-width:28px;");
    icoLbl->setAlignment(Qt::AlignCenter);

    auto *lbl = new QLabel(label, row);
    lbl->setStyleSheet("color:white; font-size:13px;");

    auto *combo = new QComboBox(row);
    combo->addItems(opts);
    combo->setCurrentIndex(currentIdx);
    combo->setFixedWidth(160);
    combo->setStyleSheet(R"(
        QComboBox { background:rgba(255,255,255,0.07); border-radius:8px;
                    color:white; font-size:12px; padding:4px 12px;
                    border:1px solid #2a2a44; }
        QComboBox::drop-down { border:none; width:20px; }
        QComboBox QAbstractItemView { background:#1a1a30; color:white;
                                       border:1px solid #2a2a44; selection-background-color:#00d4ff44; }
    )");

    hl->addWidget(icoLbl);
    hl->addWidget(lbl, 1);
    hl->addWidget(combo);
    return row;
}

static QWidget *buildSection(QWidget *parent, const QString &title,
                              const QList<QWidget*> &rows)
{
    auto *section = new QWidget(parent);
    auto *vl = new QVBoxLayout(section);
    vl->setContentsMargins(0, 0, 0, 12);
    vl->setSpacing(0);

    auto *titleLbl = new QLabel(title, section);
    titleLbl->setStyleSheet("color:#444; font-size:9px; letter-spacing:3px; padding: 4px 8px 6px 8px;");
    vl->addWidget(titleLbl);

    auto *card = new QWidget(section);
    card->setStyleSheet("background:rgba(255,255,255,0.03); border-radius:14px;");
    auto *cvl = new QVBoxLayout(card);
    cvl->setContentsMargins(8, 6, 8, 6);
    cvl->setSpacing(0);

    for (int i = 0; i < rows.size(); ++i) {
        cvl->addWidget(rows[i]);
        if (i < rows.size()-1) cvl->addWidget(makeSeparator(card));
    }
    vl->addWidget(card);
    return section;
}

// ── Panels ───────────────────────────────────────────────────────────────────
QWidget *SettingsScreen::buildDisplayPanel()
{
    auto *w = new QWidget();
    auto *vl = new QVBoxLayout(w);
    vl->setContentsMargins(0,0,0,0);
    vl->setSpacing(0);
    vl->setAlignment(Qt::AlignTop);

    vl->addWidget(buildSection(w, "SCREEN", {
        makeSliderRow(w, "☀️", "Brightness",   78),
        makeSliderRow(w, "🌙", "Night Mode",    30, "#aa66ff"),
        makeToggleRow(w, "🌗", "Auto Brightness","Adjust based on ambient light", true),
    }));
    vl->addWidget(buildSection(w, "APPEARANCE", {
        makeSelectRow(w, "🎨", "Theme",     {"Dark Blue","Midnight Black","Arctic White"}, 0),
        makeSelectRow(w, "🔤", "Font Size", {"Small","Medium","Large"}, 1),
        makeToggleRow(w, "✨", "Animations","Enable UI transitions", true),
        makeToggleRow(w, "🌐", "Map 3D Mode","Use 3D perspective for maps", true),
    }));
    vl->addStretch();
    return w;
}

QWidget *SettingsScreen::buildSoundPanel()
{
    auto *w = new QWidget();
    auto *vl = new QVBoxLayout(w);
    vl->setContentsMargins(0,0,0,0); vl->setSpacing(0); vl->setAlignment(Qt::AlignTop);

    vl->addWidget(buildSection(w, "VOLUME", {
        makeSliderRow(w, "🔊", "Master Volume", 72),
        makeSliderRow(w, "🎵", "Media",         80, "#aa66ff"),
        makeSliderRow(w, "📞", "Phone",         90, "#00ff88"),
        makeSliderRow(w, "🧭", "Navigation",    65, "#ffcc00"),
        makeSliderRow(w, "🔔", "Notifications", 50),
    }));
    vl->addWidget(buildSection(w, "AUDIO", {
        makeSelectRow(w, "🎛️", "Sound Profile",  {"Balanced","Bass Boost","Treble Boost","Flat"}, 0),
        makeSelectRow(w, "🔈", "Speaker Layout", {"All Zones","Front","Rear","Driver Only"}, 0),
        makeToggleRow(w, "🎙", "Microphone",     "Hands-free phone & voice commands", true),
        makeToggleRow(w, "🔇", "Speed-based Vol","Increase volume at higher speeds", true),
    }));
    vl->addStretch();
    return w;
}

QWidget *SettingsScreen::buildConnectivityPanel()
{
    auto *w = new QWidget();
    auto *vl = new QVBoxLayout(w);
    vl->setContentsMargins(0,0,0,0); vl->setSpacing(0); vl->setAlignment(Qt::AlignTop);

    vl->addWidget(buildSection(w, "BLUETOOTH", {
        makeToggleRow(w, "🔵", "Bluetooth",     "iPhone 15 Pro connected", true),
        makeToggleRow(w, "📲", "Auto-Connect",  "Connect on startup", true),
        makeToggleRow(w, "📡", "Audio Streaming","Stream audio via BT", true),
    }));
    vl->addWidget(buildSection(w, "WIRELESS", {
        makeToggleRow(w, "📶", "Wi-Fi Hotspot",  "Car hotspot enabled", false),
        makeToggleRow(w, "🌐", "Online Services","Maps, weather, traffic", true),
        makeToggleRow(w, "📍", "Location",       "GPS & network location", true),
    }));
    vl->addWidget(buildSection(w, "APPLE / ANDROID", {
        makeToggleRow(w, "🍎", "Apple CarPlay", "Connect via USB or wireless", true),
        makeToggleRow(w, "🤖", "Android Auto", "Connect via USB", false),
    }));
    vl->addStretch();
    return w;
}

QWidget *SettingsScreen::buildVehiclePanel()
{
    auto *w = new QWidget();
    auto *vl = new QVBoxLayout(w);
    vl->setContentsMargins(0,0,0,0); vl->setSpacing(0); vl->setAlignment(Qt::AlignTop);

    vl->addWidget(buildSection(w, "DRIVING", {
        makeSelectRow(w, "🚗", "Drive Mode",    {"Eco","Comfort","Sport","Sport+"}, 1),
        makeSelectRow(w, "🅿️",  "Park Assist",   {"Off","Sensors","Camera","360°"}, 2),
        makeToggleRow(w, "🛑", "Auto Hold",     "Hold brakes when stopped", true),
        makeToggleRow(w, "💺", "Seat Memory",   "Auto-adjust seat on unlock", true),
    }));
    vl->addWidget(buildSection(w, "SAFETY", {
        makeToggleRow(w, "⚠️",  "Lane Departure","Warn when drifting", true),
        makeToggleRow(w, "🚘", "Blind Spot",    "Monitor blind spot zones", true),
        makeToggleRow(w, "🚦", "Speed Warning", "Alert above speed limit", false),
        makeToggleRow(w, "🔦", "Auto Lights",   "Switch lights automatically", true),
    }));
    vl->addStretch();
    return w;
}

QWidget *SettingsScreen::buildAboutPanel()
{
    auto *w = new QWidget();
    auto *vl = new QVBoxLayout(w);
    vl->setContentsMargins(20, 10, 20, 10); vl->setSpacing(12);
    vl->setAlignment(Qt::AlignTop);

    // Car identity card
    auto *carCard = new QWidget(w);
    carCard->setStyleSheet("background:rgba(0,212,255,0.07); border-radius:14px;"
                            "border:1px solid rgba(0,212,255,0.18);");
    auto *ccl = new QVBoxLayout(carCard);
    ccl->setContentsMargins(20, 16, 20, 16); ccl->setSpacing(6);

    auto addInfo = [&](const QString &lbl, const QString &val) {
        auto *row = new QWidget(carCard);
        auto *hl  = new QHBoxLayout(row);
        hl->setContentsMargins(0,0,0,0);
        auto *l = new QLabel(lbl, row);
        l->setStyleSheet("color:#555; font-size:11px;");
        auto *v = new QLabel(val, row);
        v->setStyleSheet("color:white; font-size:11px; font-weight:bold;");
        v->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        hl->addWidget(l); hl->addStretch(); hl->addWidget(v);
        ccl->addWidget(row);
    };
    addInfo("Vehicle",          "BMW i5 eDrive40");
    addInfo("VIN",              "WBA12AB34CD567890");
    addInfo("HMI Version",      "InfoDrive 4.2.1");
    addInfo("Map Data",         "Europe 2025.Q1");
    addInfo("Last Update",      "March 2025");
    addInfo("Odometer",         "45,231 km");
    vl->addWidget(carCard);

    // Update button
    auto *updateBtn = new QPushButton("⬆  Check for Updates", w);
    updateBtn->setFixedHeight(44);
    updateBtn->setCursor(Qt::PointingHandCursor);
    updateBtn->setStyleSheet(R"(
        QPushButton { background:rgba(0,212,255,0.12); border-radius:12px;
                      color:#00d4ff; font-size:13px; font-weight:bold;
                      border:1px solid rgba(0,212,255,0.28); }
        QPushButton:hover { background:rgba(0,212,255,0.22); }
    )");
    vl->addWidget(updateBtn);

    auto *resetBtn = new QPushButton("🔄  Factory Reset", w);
    resetBtn->setFixedHeight(44);
    resetBtn->setCursor(Qt::PointingHandCursor);
    resetBtn->setStyleSheet(R"(
        QPushButton { background:rgba(255,50,80,0.08); border-radius:12px;
                      color:#ff5577; font-size:13px;
                      border:1px solid rgba(255,50,80,0.20); }
        QPushButton:hover { background:rgba(255,50,80,0.18); }
    )");
    vl->addWidget(resetBtn);
    vl->addStretch();
    return w;
}

// ── Main setup ────────────────────────────────────────────────────────────────
void SettingsScreen::setupUI()
{
    auto *hl = new QHBoxLayout(this);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(0);

    // Left menu
    auto *menu = new QWidget(this);
    menu->setFixedWidth(200);
    menu->setStyleSheet("background:rgba(0,0,0,0.20); border-right:1px solid rgba(255,255,255,0.05);");
    auto *mvl = new QVBoxLayout(menu);
    mvl->setContentsMargins(10, 16, 10, 16);
    mvl->setSpacing(4);

    struct MenuItem { QString icon, label; };
    QList<MenuItem> items = {
        {"🖥️",  "Display"},
        {"🔊", "Sound"},
        {"📡", "Connectivity"},
        {"🚗", "Vehicle"},
        {"ℹ️",  "About"},
    };

    contentStack = new QStackedWidget(this);

    QList<QWidget*> panels = {
        buildDisplayPanel(),
        buildSoundPanel(),
        buildConnectivityPanel(),
        buildVehiclePanel(),
        buildAboutPanel(),
    };
    for (auto *p : panels) {
        auto *scroll = new QScrollArea();
        scroll->setWidget(p);
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setStyleSheet("QScrollArea { background:transparent; border:none; }");
        contentStack->addWidget(scroll);
    }

    for (int i = 0; i < items.size(); ++i) {
        auto &item = items[i];
        auto *btn  = new QPushButton(this);
        btn->setCheckable(true);
        btn->setChecked(i == 0);
        btn->setFixedHeight(52);
        btn->setCursor(Qt::PointingHandCursor);

        auto *bl = new QHBoxLayout(btn);
        bl->setContentsMargins(12, 0, 12, 0);
        bl->setSpacing(10);
        auto *icoL = new QLabel(item.icon, btn);
        icoL->setStyleSheet("font-size:18px; background:transparent; border:none;");
        auto *lblL = new QLabel(item.label, btn);
        lblL->setStyleSheet("font-size:13px; background:transparent; border:none; color:#aaa;");
        bl->addWidget(icoL);
        bl->addWidget(lblL);
        bl->addStretch();

        btn->setStyleSheet(R"(
            QPushButton { background:transparent; border:none; border-radius:10px;
                          text-align:left; }
            QPushButton:hover { background:rgba(255,255,255,0.05); }
            QPushButton:checked { background:rgba(0,212,255,0.12);
                                  border-left:3px solid #00d4ff; }
        )");

        int idx = i;
        connect(btn, &QPushButton::clicked, this, [this, idx](){
            for (auto *b : menuBtns) b->setChecked(false);
            menuBtns[idx]->setChecked(true);
            contentStack->setCurrentIndex(idx);
        });
        mvl->addWidget(btn);
        menuBtns.append(btn);
    }
    mvl->addStretch();

    hl->addWidget(menu);

    // Content area
    auto *contentWrap = new QWidget(this);
    auto *cwl = new QVBoxLayout(contentWrap);
    cwl->setContentsMargins(24, 18, 24, 18);
    cwl->setSpacing(0);
    cwl->addWidget(contentStack);

    hl->addWidget(contentWrap, 1);
}

SettingsScreen::SettingsScreen(QWidget *parent) : QWidget(parent) { setupUI(); }

void SettingsScreen::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QLinearGradient bg(0, 0, 0, height());
    bg.setColorAt(0, QColor(10, 10, 22));
    bg.setColorAt(1, QColor(8,  8,  18));
    p.fillRect(rect(), bg);
}
