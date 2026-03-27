#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTimer>

class PhoneScreen : public QWidget
{
    Q_OBJECT
public:
    explicit PhoneScreen(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    void setupUI();

    QLabel   *dialLbl;
    QString   dialNum;
    bool      inCall = false;
    QPushButton *callBtn;
    QTimer   *callTimer;
    int       callSec = 0;
    QLabel   *callTimerLbl;
};
