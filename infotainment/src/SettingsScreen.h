#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QStackedWidget>

class SettingsScreen : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsScreen(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    void setupUI();
    QWidget *buildDisplayPanel();
    QWidget *buildSoundPanel();
    QWidget *buildConnectivityPanel();
    QWidget *buildVehiclePanel();
    QWidget *buildAboutPanel();

    QStackedWidget   *contentStack;
    QList<QPushButton*> menuBtns;
};
