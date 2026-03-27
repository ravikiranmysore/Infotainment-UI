#pragma once
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>

class NotificationOverlay : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)
public:
    explicit NotificationOverlay(QWidget *parent = nullptr);
    void show(const QString &icon, const QString &title, const QString &msg);

    float opacity() const { return m_opacity; }
    void  setOpacity(float o);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QLabel *iconLbl;
    QLabel *titleLbl;
    QLabel *msgLbl;
    QTimer *hideTimer;
    QPropertyAnimation *fadeAnim;
    float m_opacity = 0.f;
};
