#ifndef APPLYFRIENDLIST_H
#define APPLYFRIENDLIST_H
#include <QListWidget>
#include <QObject>
class ApplyFriendList : public QListWidget {
    Q_OBJECT
  public:
    explicit ApplyFriendList(QWidget* parent = nullptr);
      ~ApplyFriendList();
    // QObject interface
  protected:
    virtual bool eventFilter(QObject* watched, QEvent* event) override;
  private slots:

  signals:
    void sig_show_search(bool);
};

#endif  // APPLYFRIENDLIST_H
