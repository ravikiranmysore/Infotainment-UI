#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QPropertyAnimation>
#include <QTimer>

class TempDial : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
public:
    explicit TempDial(QWidget *parent, double initTemp, const QColor &accent);
    double temperature() const { return m_temp; }
    void   setTemperature(double t);

signals:
    void temperatureChanged(double t);

protected:
    void paintEvent(QPaintEvent *) override;
    void wheelEvent(QWheelEvent *) override;

private:
    double m_temp;
    QColor m_accent;
};

class ClimateScreen : public QWidget
{
    Q_OBJECT
public:
    explicit ClimateScreen(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    void setupUI();

    TempDial   *driverDial;
    TempDial   *passDial;
    QLabel     *driverTempLbl;
    QLabel     *passTempLbl;
    int         fanSpeed = 3;
    QTimer     *airAnimTimer;
    int         airTick  = 0;
    QLabel     *airFlowLbl;
};
