#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include <QLabel>
#include <QList>
#include <QPushButton>

class TopBar;
class SideBar;
class HomeScreen;
class MediaScreen;
class NavigationScreen;
class ClimateScreen;
class PhoneScreen;
class AppsScreen;
class SettingsScreen;
class NotificationOverlay;

class HMIWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit HMIWindow(QWidget *parent = nullptr);

public slots:
    void navigateTo(int idx);
    void showNotification(const QString &title, const QString &msg, const QString &icon);

private:
    void setupUI();
    void setupConnections();
    void updateClock();

    QWidget          *root;
    TopBar           *topBar;
    SideBar          *sideBar;
    QStackedWidget   *stack;
    NotificationOverlay *notifOverlay;

    HomeScreen       *homeScreen;
    MediaScreen      *mediaScreen;
    NavigationScreen *navScreen;
    ClimateScreen    *climateScreen;
    PhoneScreen      *phoneScreen;
    AppsScreen       *appsScreen;
    SettingsScreen   *settingsScreen;

    QTimer           *clockTimer;
    int               currentIdx = 0;
};
