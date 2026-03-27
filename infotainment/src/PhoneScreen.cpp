#include "PhoneScreen.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QLinearGradient>

PhoneScreen::PhoneScreen(QWidget *parent) : QWidget(parent) { setupUI(); }

void PhoneScreen::setupUI()
{
    auto *hl = new QHBoxLayout(this);
    hl->setContentsMargins(30,20,30,20);
    hl->setSpacing(24);

    // ===== Contacts =====
    auto *leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(260);
    auto *ll = new QVBoxLayout(leftPanel);
    ll->setSpacing(8);

    auto *cHdr = new QLabel("👥  CONTACTS", leftPanel);
    cHdr->setStyleSheet("color:#555; font-size:10px; letter-spacing:3px;");
    ll->addWidget(cHdr);

    auto *contactList = new QListWidget(leftPanel);
    contactList->setStyleSheet(R"(
        QListWidget{background:transparent;border:none;outline:none;}
        QListWidget::item{color:#ddd;font-size:12px;padding:10px 12px;
                          border-radius:8px;margin:2px 0;}
        QListWidget::item:hover{background:rgba(255,255,255,0.06);}
        QListWidget::item:selected{background:rgba(0,212,255,0.12);color:white;}
    )");
    struct Contact { QString emo, name, num; };
    QList<Contact> contacts = {
        {"👩","Anna Müller",  "+49 89 123 4567"},
        {"👨","Ben Schmidt",  "+49 171 234 5678"},
        {"🔧","Car Service",  "+49 800 123 0000"},
        {"👩‍💼","Elena Koch",   "+49 89 987 6543"},
        {"🧑","Felix Braun",  "+49 160 111 2233"},
        {"🏠","Home",         "+49 89 555 0000"},
        {"🏢","Office",       "+49 89 222 3344"},
        {"👩‍🎤","Lena Wagner",  "+49 176 445 6677"},
    };
    for (auto &c : contacts) {
        auto *item = new QListWidgetItem(c.emo + "  " + c.name, contactList);
        item->setData(Qt::UserRole, c.num);
    }
    connect(contactList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item){
        dialNum = item->data(Qt::UserRole).toString();
        dialLbl->setText(dialNum);
    });
    ll->addWidget(contactList, 1);
    hl->addWidget(leftPanel);

    // ===== Dialpad =====
    auto *midPanel = new QWidget(this);
    auto *mvl = new QVBoxLayout(midPanel);
    mvl->setAlignment(Qt::AlignCenter);
    mvl->setSpacing(14);

    // BT info
    auto *btCard = new QWidget(midPanel);
    btCard->setStyleSheet("background:rgba(0,100,255,0.09);border-radius:10px;border:1px solid rgba(0,100,255,0.22);");
    auto *btL = new QHBoxLayout(btCard);
    btL->setContentsMargins(12,8,12,8);
    auto *btIco = new QLabel("🔵", btCard);
    btIco->setStyleSheet("font-size:20px;");
    auto *btInfo = new QWidget(btCard);
    auto *biL    = new QVBoxLayout(btInfo);
    biL->setSpacing(1); biL->setContentsMargins(0,0,0,0);
    auto *biTitle = new QLabel("Bluetooth Connected", btInfo);
    biTitle->setStyleSheet("color:#5599ff;font-size:12px;font-weight:bold;");
    auto *biSub   = new QLabel("iPhone 15 Pro", btInfo);
    biSub->setStyleSheet("color:#888;font-size:11px;");
    biL->addWidget(biTitle); biL->addWidget(biSub);
    btL->addWidget(btIco); btL->addWidget(btInfo);
    mvl->addWidget(btCard);

    // Dial display
    dialLbl = new QLabel("", midPanel);
    dialLbl->setFixedHeight(56);
    dialLbl->setStyleSheet(R"(
        QLabel{color:white;font-size:24px;font-weight:bold;
               background:rgba(255,255,255,0.04);border-radius:12px;
               padding:0 20px;letter-spacing:3px;}
    )");
    dialLbl->setAlignment(Qt::AlignCenter);
    mvl->addWidget(dialLbl);

    // Grid
    auto *grid = new QGridLayout();
    grid->setSpacing(10);
    QStringList keys = {"1","2","3","4","5","6","7","8","9","*","0","#"};
    for (int i = 0; i < 12; ++i) {
        auto *b = new QPushButton(keys[i], midPanel);
        b->setFixedSize(70,54);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(R"(
            QPushButton{background:rgba(255,255,255,0.07);border-radius:10px;
                        color:white;font-size:18px;font-weight:bold;border:none;}
            QPushButton:hover{background:rgba(0,212,255,0.15);}
            QPushButton:pressed{background:rgba(0,212,255,0.30);}
        )");
        QString k = keys[i];
        connect(b, &QPushButton::clicked, this, [this,k]{ dialLbl->setText(dialNum += k); });
        grid->addWidget(b, i/3, i%3);
    }
    mvl->addLayout(grid);

    // Call + backspace row
    auto *cbRow = new QHBoxLayout();
    auto *bkBtn = new QPushButton("⌫", midPanel);
    bkBtn->setFixedSize(70,54);
    bkBtn->setCursor(Qt::PointingHandCursor);
    bkBtn->setStyleSheet(R"(
        QPushButton{background:rgba(255,255,255,0.06);border-radius:10px;
                    color:#aaa;font-size:18px;border:none;}
        QPushButton:hover{background:rgba(255,255,255,0.12);}
    )");
    connect(bkBtn, &QPushButton::clicked, this, [this]{
        if (!dialNum.isEmpty()) { dialNum.chop(1); dialLbl->setText(dialNum); }
    });

    callBtn = new QPushButton("📞  Call", midPanel);
    callBtn->setFixedHeight(54);
    callBtn->setCursor(Qt::PointingHandCursor);
    callBtn->setStyleSheet(R"(
        QPushButton{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,
                        stop:0 #00cc44,stop:1 #00ff66);
                    border-radius:10px;color:#0a1a0a;font-size:16px;
                    font-weight:bold;border:none;}
        QPushButton:hover{background:#00ff88;}
        QPushButton:pressed{background:#00aa33;}
    )");

    callTimerLbl = new QLabel("", midPanel);
    callTimerLbl->setStyleSheet("color:#00ff88;font-size:13px;");
    callTimerLbl->setAlignment(Qt::AlignCenter);

    callTimer = new QTimer(this);
    connect(callTimer, &QTimer::timeout, this, [this]{
        callSec++;
        int m = callSec/60, s = callSec%60;
        callTimerLbl->setText(QString("🟢  %1:%2").arg(m).arg(s,2,10,QChar('0')));
    });

    connect(callBtn, &QPushButton::clicked, this, [this]{
        if (dialNum.isEmpty()) return;
        inCall = !inCall;
        callBtn->setText(inCall ? "📵  End Call" : "📞  Call");
        callBtn->setStyleSheet(inCall ?
            R"(QPushButton{background:#ff3355;border-radius:10px;color:white;
                           font-size:16px;font-weight:bold;border:none;}
               QPushButton:hover{background:#ff5577;})" :
            R"(QPushButton{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,
                               stop:0 #00cc44,stop:1 #00ff66);
                           border-radius:10px;color:#0a1a0a;font-size:16px;
                           font-weight:bold;border:none;}
               QPushButton:hover{background:#00ff88;})");
        if (inCall) { callSec = 0; callTimer->start(1000); }
        else        { callTimer->stop(); callTimerLbl->setText(""); }
    });

    cbRow->addWidget(bkBtn);
    cbRow->addWidget(callBtn, 1);
    mvl->addLayout(cbRow);
    mvl->addWidget(callTimerLbl);

    hl->addWidget(midPanel, 1);

    // ===== Recent calls =====
    auto *rightPanel = new QWidget(this);
    rightPanel->setFixedWidth(220);
    auto *rl = new QVBoxLayout(rightPanel);
    rl->setSpacing(8);

    auto *rHdr = new QLabel("🕐  RECENT CALLS", rightPanel);
    rHdr->setStyleSheet("color:#555;font-size:10px;letter-spacing:3px;");
    rl->addWidget(rHdr);

    struct Recent { QString icon, name, time; };
    QList<Recent> recents = {
        {"📞","Anna Müller",  "Today 11:24"},
        {"📞","Ben Schmidt",  "Today 09:15"},
        {"📵","Unknown",      "Yesterday"},
        {"📞","Home",         "Yesterday"},
        {"📞","Office",       "Mon 08:30"},
        {"📞","Lena Wagner",  "Sun 19:12"},
    };
    for (auto &r : recents) {
        auto *card = new QWidget(rightPanel);
        card->setStyleSheet("background:rgba(255,255,255,0.03);border-radius:8px;");
        card->setCursor(Qt::PointingHandCursor);
        auto *cl2 = new QHBoxLayout(card);
        cl2->setContentsMargins(10,7,10,7); cl2->setSpacing(8);
        auto *icoL = new QLabel(r.icon, card);
        icoL->setStyleSheet("font-size:16px;");
        auto *inf = new QWidget(card);
        auto *infl = new QVBoxLayout(inf);
        infl->setSpacing(1); infl->setContentsMargins(0,0,0,0);
        auto *nL = new QLabel(r.name, inf);
        nL->setStyleSheet("color:#ddd;font-size:12px;font-weight:bold;");
        auto *tL = new QLabel(r.time, inf);
        tL->setStyleSheet("color:#555;font-size:10px;");
        infl->addWidget(nL); infl->addWidget(tL);
        cl2->addWidget(icoL); cl2->addWidget(inf,1);
        rl->addWidget(card);
    }
    rl->addStretch();
    hl->addWidget(rightPanel);
}

void PhoneScreen::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QLinearGradient bg(0,0,0,height());
    bg.setColorAt(0,QColor(10,10,22));
    bg.setColorAt(1,QColor(8,8,18));
    p.fillRect(rect(),bg);
}
