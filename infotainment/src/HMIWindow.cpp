#include "HMIWindow.h"
#include "TopBar.h"
#include "SideBar.h"
#include "HomeScreen.h"
#include "MediaScreen.h"
#include "NavigationScreen.h"
#include "ClimateScreen.h"
#include "PhoneScreen.h"
#include "AppsScreen.h"
#include "SettingsScreen.h"
#include "NotificationOverlay.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QDateTime>

// stylesheet for sidebar nav buttons
static const char *SIDEBAR_STYLE = R"(
    QPushButton#sideBtn {
        background: transparent;
        border: none;
        border-radius: 10px;
        color: #aaa;
    }
    QPushButton#sideBtn:hover {
        background: rgba(255,255,255,0.06);
    }
    QPushButton#sideBtn:checked {
        background: rgba(0,212,255,0.12);
    }
    QPushButton#sideBtn:checked QLabel {
        color: #00d4ff;
    }
)";

HMIWindow::HMIWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(1280, 720);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

    QRect sg = QApplication::primaryScreen()->availableGeometry();
    move((sg.width()-1280)/2, (sg.height()-720)/2);

    setupUI();
    setupConnections();

    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &HMIWindow::updateClock);
    clockTimer->start(1000);
    updateClock();

    // First notification after 2s
    QTimer::singleShot(2000, this, [this]{
        showNotification("📱 Bluetooth", "iPhone 15 Pro connected", "🔵");
    });
    QTimer::singleShot(6000, this, [this]{
        showNotification("🗺️ Navigation", "Traffic ahead — 12 min delay", "⚠️");
    });
    QTimer::singleShot(11000, this, [this]{
        showNotification("🎵 Now Playing", "Neon Highway — Synthwave Collective", "🎵");
    });
}

void HMIWindow::setupUI()
{
    root = new QWidget(this);
    setCentralWidget(root);
    root->setStyleSheet("background:#0c0c1e;");

    auto *vl = new QVBoxLayout(root);
    vl->setSpacing(0);
    vl->setContentsMargins(0,0,0,0);

    topBar = new TopBar(root);
    vl->addWidget(topBar);

    auto *hl = new QHBoxLayout();
    hl->setSpacing(0);
    hl->setContentsMargins(0,0,0,0);

    sideBar = new SideBar(root);
    sideBar->setStyleSheet(SIDEBAR_STYLE);
    hl->addWidget(sideBar);

    stack = new QStackedWidget(root);
    homeScreen    = new HomeScreen(stack);
    mediaScreen   = new MediaScreen(stack);
    navScreen     = new NavigationScreen(stack);
    climateScreen = new ClimateScreen(stack);
    phoneScreen   = new PhoneScreen(stack);
    appsScreen    = new AppsScreen(stack);
    settingsScreen= new SettingsScreen(stack);

    stack->addWidget(homeScreen);      // 0
    stack->addWidget(mediaScreen);     // 1
    stack->addWidget(navScreen);       // 2
    stack->addWidget(climateScreen);   // 3
    stack->addWidget(phoneScreen);     // 4
    stack->addWidget(appsScreen);      // 5
    stack->addWidget(settingsScreen);  // 6

    hl->addWidget(stack, 1);
    vl->addLayout(hl, 1);

    // Notification overlay (floats over everything)
    notifOverlay = new NotificationOverlay(root);
    notifOverlay->setGeometry(root->width()-360, 56, 340, 80);
    notifOverlay->hide();
}

void HMIWindow::setupConnections()
{
    connect(sideBar,  &SideBar::pageRequested, this, &HMIWindow::navigateTo);
    connect(topBar,   &TopBar::closeClicked,   qApp, &QApplication::quit);
    connect(topBar,   &TopBar::homeClicked,    this, [this]{ navigateTo(0); });

    // Media screen "open" shortcut from home
    connect(homeScreen, &HomeScreen::openMedia, this, [this]{ navigateTo(1); });
    connect(homeScreen, &HomeScreen::openNav,   this, [this]{ navigateTo(2); });
    connect(homeScreen, &HomeScreen::openClimate,this,[this]{ navigateTo(3); });
    connect(homeScreen, &HomeScreen::openPhone, this, [this]{ navigateTo(4); });
}

void HMIWindow::navigateTo(int idx)
{
    currentIdx = idx;
    stack->setCurrentIndex(idx);
    sideBar->setActive(idx);

    static const QStringList titles = {
        "Home", "Media", "Navigation", "Climate", "Phone", "Apps", "Settings"
    };
    if (idx < titles.size())
        topBar->setPageTitle(titles[idx]);
}

void HMIWindow::showNotification(const QString &title,
                                  const QString &msg,
                                  const QString &icon)
{
    notifOverlay->show(icon, title, msg);
    topBar->pulse();
}

void HMIWindow::updateClock()
{
    topBar->setTime(QDateTime::currentDateTime().toString("hh:mm"));
}
