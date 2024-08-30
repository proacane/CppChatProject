#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H
/*****************************************************************************
 * @file         messagetextedit.h
 * @brief
 * @description  自定义消息框
 * @author       ACA
 * @date         2024/08/30
 * @history
 *****************************************************************************/
#include <QApplication>
#include <QDrag>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QMimeData>
#include <QMimeType>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QTextEdit>
#include <QVector>

#include "global.h"

class MessageTextEdit : public QTextEdit {
    Q_OBJECT
  public:
    explicit MessageTextEdit(QWidget* parent = nullptr);

    ~MessageTextEdit();
    // 返回不同的消息类型
    QVector<MsgInfo> getMsgList();
    // 插入文件生成 url
    void insertFileFromUrl(const QStringList& urls);
  signals:
    void send();

  protected:
    // 文件拖动
    void dragEnterEvent(QDragEnterEvent* event);
    // 文件释放
    void dropEvent(QDropEvent* event);
    // 按下回车
    void keyPressEvent(QKeyEvent* e);

  private:
    // 插入图片
    void insertImages(const QString& url);
    // 插入文本
    void insertTextFile(const QString& url);
    // TODO 文件发送后续实现
    bool canInsertFromMimeData(const QMimeData* source) const;
    // TODO 图片发送后续实现
    void insertFromMimeData(const QMimeData* source);

  private:
    // 判断文件是否为图片
    bool isImage(QString url);
    // 插入消息
    void insertMsgList(QVector<MsgInfo>& list, QString flag, QString text, QPixmap pix);

    QStringList getUrl(QString text);
    // 获取文件图标及大小信息，并转化成图片
    QPixmap getFileIconPixmap(const QString& url);
    // 获取文件大小
    QString getFileSize(qint64 size);

  private slots:
    void textEditChanged();

  private:
    QVector<MsgInfo> mMsgList;
    QVector<MsgInfo> mGetMsgList;
};
#endif  // MESSAGETEXTEDIT_H
