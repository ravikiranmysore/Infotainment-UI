#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QListWidget>
#include <QTimer>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QVBoxLayout>

class MediaScreen : public QWidget
{
    Q_OBJECT
public:
    explicit MediaScreen(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;

private slots:
    void playPause();
    void nextTrack();
    void prevTrack();
    void tickProgress();
    void switchSource(int idx);

private:
    void setupUI();
    void loadTrack(int idx);
    void buildPlaylistPanel(QWidget *parent, QVBoxLayout *vl);
    void buildPlayerPanel(QWidget *parent, QHBoxLayout *hl);

    struct Track { QString title, artist, album, dur, emoji; QColor color; };
    QList<Track> tracks;
    int   currentTrack = 0;
    bool  playing = false;
    int   progressSec = 0, totalSec = 0;

    // Player widgets
    QLabel      *coverLbl;
    QLabel      *titleLbl;
    QLabel      *artistLbl;
    QLabel      *albumLbl;
    QPushButton *playBtn;
    QSlider     *seekSlider;
    QLabel      *timeLbl;
    QLabel      *durLbl;
    QListWidget *playlist;
    QLabel      *sourceLbl;

    QTimer             *progressTimer;
    QPropertyAnimation *coverPulse;

    // Source tabs
    QStackedWidget *sourceStack;
    QList<QPushButton*> srcBtns;
};
