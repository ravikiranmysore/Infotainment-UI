#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>

class AppsScreen : public QWidget
{
    Q_OBJECT
public:
    explicit AppsScreen(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    void setupUI();
};
