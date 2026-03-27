#pragma once
#include <QWidget>
#include <QLabel>
#include <QTimer>

class HomeScreen : public QWidget
{
    Q_OBJECT
public:
    explicit HomeScreen(QWidget *parent = nullptr);

signals:
    void openMedia();
    void openNav();
    void openClimate();
    void openPhone();

protected:
    void paintEvent(QPaintEvent *) override;

private:
    void setupUI();
    QWidget *makeQuickCard(const QString &icon, const QString &title,
                           const QString &subtitle, const QColor &accent,
                           std::function<void()> onClick);
    QWidget *makeWeatherWidget();
    QWidget *makeNowPlayingWidget();
    QWidget *makeRecentWidget();

    QLabel *timeLbl;
    QLabel *dateLbl;
    QLabel *greetLbl;
    QTimer *clockTimer;
};
