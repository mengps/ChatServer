#include "database.h"

#include <QDir>
#include <QSqlQuery>
#include <QSqlError>

extern QDebug operator<<(QDebug debug, const UserInfo &info)
{
    QDebugStateSaver saver(debug);
    debug << "username: " << info.username  << "password: " << info.password  << "nickname: " << info.nickname
          << "headImage: " << info.headImage << "background" << info.background << "gende: " << info.gender
          << "birthday: " << info.birthday << "signature: " << info.signature  << "level: " << info.level;
    return debug;
}

Database::Database(const QString &connectionName, QObject *parent)
    : QObject(parent)
    , m_connectionName(connectionName)
{
    QMutexLocker locker(&m_mutex);
    m_database = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    m_database.setDatabaseName("users/users.db");
    m_database.setHostName("localhost");
    m_database.setUserName("MChatServer");
    m_database.setPassword("123456");
    m_database.open();
}

Database::~Database()
{
    closeDatabase();
    //QSqlDatabase::removeDatabase(m_connectionName);
}

bool Database::openDatabase()
{
    if (!m_database.isOpen())
    {
        QMutexLocker locker(&m_mutex);
        return m_database.open();
    }

    return true;
}

void Database::closeDatabase()
{
    if (m_database.isOpen())
        m_database.close();
}

bool Database::createUser(const UserInfo &info)
{
    if (!tableExists())
        return false;

    QString user_dir = "users/" + info.username;
    if (!QFile::exists(user_dir))               //如果该用户不存在,则创建一系列文件夹
    {
        QDir dir;
        dir.mkpath(user_dir);
        dir.mkpath(user_dir + "/headImage");
        dir.mkpath(user_dir + "/messageImage");
        dir.mkpath(user_dir + "/messageText");
    }

    QString insert = "INSERT INTO info VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?);";
    QSqlQuery query(m_database);
    query.prepare(insert);
    query.addBindValue(info.username);
    query.addBindValue(info.password);
    query.addBindValue(info.nickname);
    query.addBindValue(info.headImage);
    query.addBindValue(info.background);
    query.addBindValue(info.gender);
    query.addBindValue(info.birthday);
    query.addBindValue(info.signature);
    query.addBindValue(info.level);

    if (query.exec())
    {
        return true;
    }
    else
    {
        qDebug() << __func__ << query.lastError().text();
        closeDatabase();
        return false;
    }
}

UserInfo Database::getUserInfo(const QString &username)
{
    if (!tableExists())
        return UserInfo();

    QSqlQuery query(m_database);
    if (!query.exec("SELECT * FROM info WHERE user_username = '" + username + "';"))
         qDebug() << __func__ <<  query.lastError().text();
    query.next();

    UserInfo info;
    if (query.isValid())
    {
        info.username = query.value(0).toString();
        info.password = query.value(1).toString();
        info.nickname = query.value(2).toString();
        info.headImage = query.value(3).toString();
        info.background = query.value(4).toString();
        info.gender = query.value(5).toString();
        info.birthday = query.value(6).toString();
        info.signature = query.value(7).toString();
        info.level = query.value(8).toInt();
    }
    else qDebug() << __func__ << "未找到" + username + "的信息。";

    return info;
}

bool Database::addFriend(const QString &username, const QString &friendname)
{
    /*if (username > friendname)
        qSwap(username, friendname);*/
    QString insert = "INSERT INTO users VALUES(?, ?, ?, ?, ?, ?);";
    QSqlQuery query(m_database);
    query.prepare(insert);
    query.addBindValue(username);
    query.addBindValue(friendname);
    query.addBindValue("我的好友");
    query.addBindValue("我的好友");
    query.addBindValue(0);
    query.addBindValue(0);

    if (query.exec())
    {
        return true;
    }
    else
    {
        qDebug() << __func__ << query.lastError().text();;
        closeDatabase();
        return false;
    }
}

void Database::setUserInfo(const UserInfo &info)
{
    QString query_update = "UPDATE info "
                           "SET user_password = '" + info.password +
                           "', user_nickname = '" + info.nickname +
                           "', user_headImage = '" + info.headImage +
                           "', user_background = '" + info.background +
                           "', user_gender = '" + info.gender +
                           "', user_birthday = '" + info.birthday +
                           "', user_signature = '" + info.signature +
                           "', user_level = " + QString::number(info.level) +
                           " WHERE user_username = '" + info.username + "';";
    QSqlQuery query(m_database);
    if (query.exec(query_update))
    {
        qDebug() << "user info 更新成功";
    }
    else
    {
        qDebug() << __func__ <<  query.lastError().text();
        closeDatabase();
    }
}

QStringList Database::getUserFriends(const QString &username)
{
    QString query_friends = "SELECT user_friend AS user_friend "
                            "FROM users "
                            "WHERE user_username = '" + username + "' "
                            "UNION ALL "
                            "SELECT user_username AS user_friend "
                            "FROM users "
                            "WHERE user_friend = '" + username + "'; ";

    QSqlQuery query(m_database);
    QStringList friends;
    if (query.exec(query_friends))
    {
        while (query.next())
        {
            if (query.isValid())
                friends << query.value(0).toString();
        }
    }
    else
    {
        qDebug() << __func__ <<  query.lastError().text();
        closeDatabase();
    }

    return friends;
}

QMap<QString, QList<FriendInfo> > Database::getUserFriendsInfo(const QString &username)
{
    QString query_friends = "SELECT user_group AS user_group, user_friend AS user_friend, user_unread AS unreadMessage "
                            "FROM users "
                            "WHERE user_username = '" + username + "' "
                            "UNION ALL "
                            "SELECT friend_group AS user_group, user_username AS user_friend, friend_unread AS unreadMessage "
                            "FROM users "
                            "WHERE user_friend = '" + username + "';";
    QSqlQuery query(m_database);
    QMap<QString, QList<FriendInfo> > friendsInfo;
    if (query.exec(query_friends))
    {
        while (query.next())
        {
            if (query.isValid())
            {
                QString user_group = query.value(0).toString();
                QString user_friend = query.value(1).toString();
                int user_unread = query.value(2).toInt();
                FriendInfo info = { user_friend, user_unread };
                friendsInfo[user_group].append(info);
            }
        }
    }
    else
    {
        qDebug() << __func__ <<  query.lastError().text();
        closeDatabase();
    }

    return friendsInfo;
}

bool Database::addUnreadMessage(const QString &sender, const QString &receiver)
{
    QString query_unread = "SELECT user_unread AS unreadMessage "
                           "FROM users "
                           "WHERE user_username = '" + receiver + "' "
                           "UNION ALL "
                           "SELECT friend_unread AS unreadMessage "
                           "FROM users "
                           "WHERE user_friend = '" + receiver + "';";

    QSqlQuery query(m_database);

    if (query.exec(query_unread))
    {
        query.next();
        int unreadMessage = query.value(0).toInt();
        QString query_update1 = "UPDATE users "
                                "SET user_unread = " + QString::number(unreadMessage + 1) +
                                " WHERE user_username = '" + receiver + "' AND user_friend = '" + sender + "';";

        QString query_update2 = "UPDATE users "
                                "SET friend_unread = " + QString::number(unreadMessage + 1) +
                                " WHERE user_username = '" + sender + "' AND user_friend = '" + receiver + "';";
        if (query.exec(query_update1) && query.exec(query_update2))
        {
            return true;
        }
        else
        {
            qDebug() << __func__ << query.lastError().text();
            closeDatabase();
            return false;
        }
    }
    else
    {
        qDebug() << __func__ << query.lastError().text();
        closeDatabase();
        return false;
    }
}

bool Database::tableExists()
{
    if (!openDatabase())
        return false;

    QString query_create_users = "CREATE TABLE IF NOT EXISTS users"
                                 "("
                                 "    user_username varchar(10) NOT NULL,"
                                 "    user_friend   varchar(10) NOT NULL,"
                                 "    user_group    varchar(16) DEFAULT '我的好友',"
                                 "    friend_group  varchar(16) DEFAULT '我的好友',"
                                 "    user_unread   int         DEFAULT 0,"
                                 "    friend_unread int         DEFAULT 0"
                                 ");";
    QString query_create_info = "CREATE TABLE IF NOT EXISTS info"
                                "("
                                "    user_username   varchar(10) NOT NULL PRIMARY KEY,"
                                "    user_password   varchar(32) NOT NULL,"
                                "    user_nickname   varchar(32) DEFAULT 'USER',"
                                "    user_headImage  varchar(32) DEFAULT 'qrc:/image/winIcon.png',"
                                "    user_background varchar(32) DEFAULT 'qrc:/image/Background/7.jpg',"
                                "    user_gender     varchar(2)  DEFAULT '男',"
                                "    user_birthday   text        DEFAULT '2019-01-01',"
                                "    user_signature  varchar(64) DEFAULT NULL,"
                                "    user_level      int         DEFAULT 1"
                                ");";
    QSqlQuery query(m_database);
    if (query.exec(query_create_users))
    {
        if (query.exec(query_create_info))
        {
            return true;
        }
        else
        {
            qDebug() << __func__ << query.lastError().text();
            closeDatabase();
            return false;
        }
    }
    else
    {
        qDebug() << __func__ << query.lastError().text();
        closeDatabase();
        return false;
    }
}
