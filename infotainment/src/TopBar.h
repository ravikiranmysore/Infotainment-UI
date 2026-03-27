#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QPropertyAnimation>

class TopBar : public QWidget
{
    Q_OBJECT
public:
    explicit TopBar(QWidget *parent = nullptr);
    void setTime(const QString &t);
    void setPageTitle(const QString &t);
    void pulse();  // animate notification bell

signals:
    void closeClicked();
    void homeClicked();

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QLabel *timeLbl;
    QLabel *titleLbl;
    QLabel *signalLbl;
    QLabel *battLbl;
    QLabel *tempLbl;
    QPushButton *closeBtn;
    QPushButton *homeBtn;

    QTimer *tickTimer;
    bool   wifiOn   = true;
    bool   btOn     = true;
};
