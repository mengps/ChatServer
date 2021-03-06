﻿#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QtGlobal>
#include <QDataStream>

typedef quint32 msg_flag_t;
typedef quint8  msg_t;
typedef quint32 msg_size_t;
typedef quint8  msg_option_t;

//用于测试
#define server_ip      "10.220.3.25"
#define server_port    43800

//消息头的标志
#define MSG_FLAG 0xF8AD951A

//消息类型
//系统类型  0x10 ~ 0x40
#define MSG_IS_SYSTEM(x) (x <= 0x40)
#define MT_CHECK       0x10        //验证
#define MT_HEARTBEAT   0x11        //心跳
#define MT_USERINFO    0x12        //用户信息
#define MT_STATECHANGE 0x13        //状态变化
#define MT_SEARCH      0x14        //获取信息
#define MT_ADDFRIEND   0x15        //添加好友
#define MT_REGISTER    0x16        //注册账户
#define MT_UNKNOW      0x30        //未知

#define CHECK_SUCCESS  "SUCCESS"   //验证成功
#define CHECK_FAILURE  "FAILURE"   //验证失败

#define CLIENT_ID      "CLIENT"    //客户端默认ID
#define SERVER_ID      "SERVER"    //服务器ID
#define HEARTBEAT      "HEARTBEAT" //心跳数据
#define USERINFO       "USERINFO"  //用户信息

#define ADDFRIEND      "ADDFRIEND" //添加好友
//添加成功将发送其信息
#define ADD_SUCCESS    "SUCCESS"   //添加成功
#define ADD_FAILURE    "FAILURE"   //添加失败

//注册成功/失败消息
#define REG_SUCCESS    "SUCCESS"   //注册成功
#define REG_FAILURE    "FAILURE"   //注册失败

//用户类型 0x41 ~ 0xFF
#define MSG_IS_USER(x) (!(MSG_IS_SYSTEM(x)))
#define MT_SHAKE       0x41        //窗口抖动
#define MT_TEXT        0x42        //普通文本
#define MT_IMAGE       0x43        //图像
#define MT_FILE        0x44        //文件

//选项类型
#define MO_NULL        0x10       //无类型
#define MO_UPLOAD      0x11       //上传
#define MO_DOWNLOAD    0x12       //下载

/*
 *  \ 消息标志flag \\ 消息类型type \\ 消息大小size \\ 选项类型option \\ 发送者ID \\ 接收者ID \\ MD5验证 \
 *                                           { 消息头 }
 *                                 \ 数据data \ ... \ 数据data \
 *                                           { 数据 }
*/

struct MessageHeader
{
    MessageHeader()
        : flag(0), type(0), size(0), option(0),
          sender(QByteArray()), receiver(QByteArray()), md5(QByteArray()) {}
    MessageHeader(msg_flag_t f, msg_t t, msg_size_t s, msg_option_t o,
                  const QByteArray &sr, const QByteArray &rr, const QByteArray &m)
        : flag(f), type(t), size(s), option(o), sender(sr), receiver(rr), md5(m) {}

    bool isEmpty() const
    {
        return flag     == 0 ||
               type     == 0 ||
               size     == 0 ||
               option   == 0 ||
               sender.isEmpty()   ||
               receiver.isEmpty() ||
               md5.isEmpty();
    }

    int getSize() const
    {
        return int(sizeof(flag)) +
               int(sizeof(type)) +
               int(sizeof(size)) +
               int(sizeof(option)) +
               sender.size() +
               receiver.size() +
               md5.size() +
               3 * 4;       //有三个QByteArray，每个会在前面加4字节大小
    }

    msg_flag_t   flag;
    msg_t        type;
    msg_size_t   size;
    msg_option_t option;
    QByteArray   sender;    //Latin-1
    QByteArray   receiver;  //Latin-1
    QByteArray   md5;       //md5(base64 data)
};

struct Message
{
    Message() : header(MessageHeader()), data(QByteArray()) {}
    Message(const MessageHeader &h, const QByteArray &d) : header(h), data(d) {}
    bool isEmpty() const { return header.isEmpty() || data.isEmpty(); }

    MessageHeader header;
    QByteArray data;        //Local8Bit
};

msg_flag_t   inline get_flag(MessageHeader &header) { return header.flag; }
msg_flag_t   inline get_flag(Message &message) { return get_flag(message.header); }
msg_t        inline get_type(MessageHeader &header) { return header.type; }
msg_t        inline get_type(Message &message) { return get_type(message.header); }
msg_size_t   inline get_size(MessageHeader &header) { return header.size; }
msg_size_t   inline get_size(Message &message) { return get_size(message.header); }
msg_option_t inline get_option(MessageHeader &header) { return header.option; }
msg_option_t inline get_option(Message &message) { return get_option(message.header); }
QByteArray   inline get_sender(MessageHeader &header) { return header.sender; }
QByteArray   inline get_sender(Message &message) { return get_sender(message.header); }
QByteArray   inline get_receiver(MessageHeader &header) { return header.receiver; }
QByteArray   inline get_receiver(Message &message) { return get_receiver(message.header); }
QByteArray   inline get_md5(MessageHeader &header) { return header.md5; }
QByteArray   inline get_md5(Message &message) { return get_md5(message.header); }

QDebug operator<<(QDebug debug, const MessageHeader &header);
QDebug operator<<(QDebug debug, const Message &message);
QDataStream& operator<<(QDataStream &out, const MessageHeader &header);
QDataStream& operator>>(QDataStream &in, MessageHeader &header);
QDataStream& operator<<(QDataStream &out, const Message &message);
QDataStream& operator>>(QDataStream &in, Message &message);

#endif // PROTOCOL_H
