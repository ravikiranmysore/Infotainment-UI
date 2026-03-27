#pragma once
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QList>
#include <QPointF>

class NavigationScreen : public QWidget
{
    Q_OBJECT
public:
    explicit NavigationScreen(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;

private slots:
    void animateTick();

private:
    void setupUI();
    void drawGrid(QPainter &p, int mw, int mh);
    void drawRoads(QPainter &p, int mw, int mh);
    void drawBuildings(QPainter &p, int mw, int mh);
    void drawRoute(QPainter &p, int mw, int mh);
    void drawCar(QPainter &p, int mw, int mh);
    void drawPOIs(QPainter &p, int mw, int mh);
    void drawOverlayHUD(QPainter &p, int mw, int mh);

    QTimer  *animTimer;
    double   carT    = 0.0;
    int      tick    = 0;
    int      sidebarW = 260;

    QLabel  *etaLbl;
    QLabel  *distLbl;
    QLabel  *turnLbl;
    QLabel  *streetLbl;
    QLabel  *speedLimitLbl;
    QLineEdit *searchEdit;

    QList<QPointF> routePts;
};
