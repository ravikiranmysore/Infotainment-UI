#pragma once
#include <QWidget>
#include <QList>
#include <QPushButton>
#include <QLabel>

class SideBar : public QWidget
{
    Q_OBJECT
public:
    explicit SideBar(QWidget *parent = nullptr);
    void setActive(int idx);

signals:
    void pageRequested(int idx);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    struct NavItem { QString emoji; QString label; };
    QList<NavItem>   items;
    QList<QPushButton*> btns;
    int activeIdx = 0;
};
