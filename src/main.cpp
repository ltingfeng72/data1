/*
 * ============================================
 * 图书管理系统 - 基于角色的访问控制 (RBAC)
 * Library Management System with Role-Based Access Control
 * ============================================
 * 
 * 编译说明：
 * 1. 在 Linux 上编译：
 *    g++ -o library_system main.cpp -lodbc
 * 
 * 2. 在 Windows 上编译：
 *    cl main.cpp odbc32.lib
 * 
 * ODBC 配置说明：
 * 1. Linux (unixODBC):
 *    - 安装：sudo apt-get install unixodbc unixodbc-dev
 *    - 配置 DSN：编辑 /etc/odbc.ini 或 ~/.odbc.ini
 *    - 示例配置：
 *      [LibraryDB]
 *      Driver = MySQL
 *      Server = localhost
 *      Database = library
 *      Port = 3306
 * 
 * 2. Windows (ODBC Data Source Administrator):
 *    - 打开"ODBC数据源管理器"
 *    - 添加系统DSN或用户DSN
 *    - 选择MySQL ODBC驱动
 *    - 配置数据源名称为"LibraryDB"
 */

#include <iostream>
#include <string>
#include <cstring>
#include <sql.h>
#include <sqlext.h>

using namespace std;

// ODBC 句柄
SQLHENV hEnv = SQL_NULL_HENV;     // 环境句柄
SQLHDBC hDbc = SQL_NULL_HDBC;     // 连接句柄
SQLHSTMT hStmt = SQL_NULL_HSTMT;  // 语句句柄

// 用户角色枚举
enum UserRole {
    ROLE_ADMIN,      // 管理员：增删改图书
    ROLE_OPERATOR,   // 操作员：借书还书
    ROLE_READER,     // 读者：查询图书、查看借阅记录
    ROLE_UNKNOWN
};

// 当前登录用户信息
struct CurrentUser {
    string username;
    UserRole role;
    int userId;
} currentUser;

// ============================================
// ODBC 辅助函数
// ============================================

/**
 * 初始化 ODBC 环境
 */
bool initODBC() {
    SQLRETURN ret;
    
    // 分配环境句柄
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        cout << "错误：无法分配环境句柄" << endl;
        return false;
    }
    
    // 设置 ODBC 版本
    ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        cout << "错误：无法设置ODBC版本" << endl;
        return false;
    }
    
    // 分配连接句柄
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        cout << "错误：无法分配连接句柄" << endl;
        return false;
    }
    
    return true;
}

/**
 * 连接到数据库
 * 请根据实际情况修改DSN、用户名和密码
 */
bool connectDatabase() {
    SQLRETURN ret;
    
    // 方法1：使用DSN连接（推荐）
    // DSN需要在ODBC数据源管理器中预先配置
    const char* dsn = "LibraryDB";
    const char* user = "root";
    const char* password = "password";
    
    ret = SQLConnect(hDbc, 
                     (SQLCHAR*)dsn, SQL_NTS,
                     (SQLCHAR*)user, SQL_NTS,
                     (SQLCHAR*)password, SQL_NTS);
    
    // 方法2：使用连接字符串（备选）
    // const char* connStr = "Driver={MySQL ODBC 8.0 Driver};Server=localhost;Database=library;User=root;Password=password;";
    // ret = SQLDriverConnect(hDbc, NULL, (SQLCHAR*)connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLCHAR sqlState[6], message[SQL_MAX_MESSAGE_LENGTH];
        SQLINTEGER nativeError;
        SQLSMALLINT textLength;
        
        SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, 1, sqlState, &nativeError, message, sizeof(message), &textLength);
        cout << "数据库连接失败！" << endl;
        cout << "SQLSTATE: " << sqlState << endl;
        cout << "错误信息: " << message << endl;
        return false;
    }
    
    cout << "数据库连接成功！" << endl;
    return true;
}

/**
 * 执行SQL查询并返回是否成功
 */
bool executeSQL(const char* sql) {
    SQLRETURN ret;
    
    // 分配语句句柄
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        cout << "错误：无法分配语句句柄" << endl;
        return false;
    }
    
    // 执行SQL语句
    ret = SQLExecDirect(hStmt, (SQLCHAR*)sql, SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLCHAR sqlState[6], message[SQL_MAX_MESSAGE_LENGTH];
        SQLINTEGER nativeError;
        SQLSMALLINT textLength;
        
        SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, message, sizeof(message), &textLength);
        cout << "SQL执行失败！" << endl;
        cout << "SQL: " << sql << endl;
        cout << "SQLSTATE: " << sqlState << endl;
        cout << "错误信息: " << message << endl;
        
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return false;
    }
    
    return true;
}

/**
 * 清理 ODBC 资源
 */
void cleanupODBC() {
    if (hStmt != SQL_NULL_HSTMT) {
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }
    if (hDbc != SQL_NULL_HDBC) {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    }
    if (hEnv != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }
}

// ============================================
// 用户认证和授权
// ============================================

/**
 * 用户登录认证
 */
bool login(const string& username, const string& password) {
    char sql[256];
    sprintf(sql, "SELECT userId, role FROM users WHERE username='%s' AND password='%s'", 
            username.c_str(), password.c_str());
    
    if (!executeSQL(sql)) {
        return false;
    }
    
    SQLRETURN ret;
    SQLINTEGER userId;
    SQLCHAR role[20];
    SQLLEN indicator;
    
    // 绑定列
    SQLBindCol(hStmt, 1, SQL_C_LONG, &userId, 0, &indicator);
    SQLBindCol(hStmt, 2, SQL_C_CHAR, role, sizeof(role), &indicator);
    
    // 获取结果
    ret = SQLFetch(hStmt);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        currentUser.username = username;
        currentUser.userId = userId;
        
        string roleStr = (char*)role;
        if (roleStr == "admin") {
            currentUser.role = ROLE_ADMIN;
        } else if (roleStr == "operator") {
            currentUser.role = ROLE_OPERATOR;
        } else if (roleStr == "reader") {
            currentUser.role = ROLE_READER;
        } else {
            currentUser.role = ROLE_UNKNOWN;
        }
        
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        cout << "登录成功！欢迎 " << username << endl;
        return true;
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    cout << "登录失败：用户名或密码错误" << endl;
    return false;
}

/**
 * 检查权限
 */
bool checkPermission(UserRole requiredRole) {
    if (currentUser.role != requiredRole) {
        cout << "权限不足：此操作需要不同的角色权限" << endl;
        return false;
    }
    return true;
}

// ============================================
// 管理员功能：图书管理
// ============================================

/**
 * 添加图书
 */
void addBook() {
    if (!checkPermission(ROLE_ADMIN)) return;
    
    string bookName, publisher, pubDate;
    int bookrackId, roomId;
    
    cout << "\n=== 添加图书 ===" << endl;
    cout << "图书名称: ";
    cin.ignore();
    getline(cin, bookName);
    cout << "出版日期 (YYYY-MM-DD): ";
    getline(cin, pubDate);
    cout << "出版社: ";
    getline(cin, publisher);
    cout << "书架ID: ";
    cin >> bookrackId;
    cout << "房间ID: ";
    cin >> roomId;
    
    char sql[512];
    sprintf(sql, "INSERT INTO books (bookName, publicationDate, publisher, bookrackId, roomId) VALUES ('%s', '%s', '%s', %d, %d)",
            bookName.c_str(), pubDate.c_str(), publisher.c_str(), bookrackId, roomId);
    
    if (executeSQL(sql)) {
        cout << "图书添加成功！" << endl;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

/**
 * 删除图书
 */
void deleteBook() {
    if (!checkPermission(ROLE_ADMIN)) return;
    
    int bookId;
    cout << "\n=== 删除图书 ===" << endl;
    cout << "请输入要删除的图书ID: ";
    cin >> bookId;
    
    char sql[256];
    sprintf(sql, "DELETE FROM books WHERE bookId=%d", bookId);
    
    if (executeSQL(sql)) {
        cout << "图书删除成功！" << endl;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

/**
 * 修改图书信息
 */
void updateBook() {
    if (!checkPermission(ROLE_ADMIN)) return;
    
    int bookId;
    string bookName, publisher;
    
    cout << "\n=== 修改图书信息 ===" << endl;
    cout << "请输入要修改的图书ID: ";
    cin >> bookId;
    cout << "新图书名称: ";
    cin.ignore();
    getline(cin, bookName);
    cout << "新出版社: ";
    getline(cin, publisher);
    
    char sql[512];
    sprintf(sql, "UPDATE books SET bookName='%s', publisher='%s' WHERE bookId=%d",
            bookName.c_str(), publisher.c_str(), bookId);
    
    if (executeSQL(sql)) {
        cout << "图书信息修改成功！" << endl;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

// ============================================
// 操作员功能：借还书管理
// ============================================

/**
 * 借书操作
 */
void borrowBook() {
    if (!checkPermission(ROLE_OPERATOR)) return;
    
    int browBookId, bookId;
    
    cout << "\n=== 借书操作 ===" << endl;
    cout << "读者ID (borrowBookId): ";
    cin >> browBookId;
    cout << "图书ID: ";
    cin >> bookId;
    
    char sql[512];
    sprintf(sql, "INSERT INTO borrow (browBookId, bookId, borrowDate, returnDate) VALUES (%d, %d, NOW(), NULL)",
            browBookId, bookId);
    
    if (executeSQL(sql)) {
        cout << "借书登记成功！" << endl;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

/**
 * 还书操作
 */
void returnBook() {
    if (!checkPermission(ROLE_OPERATOR)) return;
    
    int browBookId, bookId;
    
    cout << "\n=== 还书操作 ===" << endl;
    cout << "读者ID (borrowBookId): ";
    cin >> browBookId;
    cout << "图书ID: ";
    cin >> bookId;
    
    char sql[512];
    sprintf(sql, "UPDATE borrow SET returnDate=NOW() WHERE browBookId=%d AND bookId=%d AND returnDate IS NULL",
            browBookId, bookId);
    
    if (executeSQL(sql)) {
        cout << "还书登记成功！" << endl;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

// ============================================
// 读者功能：查询
// ============================================

/**
 * 查询图书信息
 */
void queryBooks() {
    if (!checkPermission(ROLE_READER)) return;
    
    string keyword;
    cout << "\n=== 查询图书 ===" << endl;
    cout << "输入图书名称关键字（留空查询所有）: ";
    cin.ignore();
    getline(cin, keyword);
    
    char sql[512];
    if (keyword.empty()) {
        sprintf(sql, "SELECT bookId, bookName, publisher, publicationDate, bookrackId, roomId FROM books");
    } else {
        sprintf(sql, "SELECT bookId, bookName, publisher, publicationDate, bookrackId, roomId FROM books WHERE bookName LIKE '%%%s%%'",
                keyword.c_str());
    }
    
    if (!executeSQL(sql)) {
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }
    
    SQLINTEGER bookId, bookrackId, roomId;
    SQLCHAR bookName[256], publisher[256], pubDate[64];
    SQLLEN indicator;
    
    SQLBindCol(hStmt, 1, SQL_C_LONG, &bookId, 0, &indicator);
    SQLBindCol(hStmt, 2, SQL_C_CHAR, bookName, sizeof(bookName), &indicator);
    SQLBindCol(hStmt, 3, SQL_C_CHAR, publisher, sizeof(publisher), &indicator);
    SQLBindCol(hStmt, 4, SQL_C_CHAR, pubDate, sizeof(pubDate), &indicator);
    SQLBindCol(hStmt, 5, SQL_C_LONG, &bookrackId, 0, &indicator);
    SQLBindCol(hStmt, 6, SQL_C_LONG, &roomId, 0, &indicator);
    
    cout << "\n图书列表：" << endl;
    cout << "------------------------------------------------------------" << endl;
    
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        cout << "ID: " << bookId << " | 书名: " << bookName 
             << " | 出版社: " << publisher << " | 日期: " << pubDate
             << " | 书架: " << bookrackId << " | 房间: " << roomId << endl;
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

/**
 * 查询借阅记录（读者只能查看自己的记录）
 */
void queryBorrowRecords() {
    if (!checkPermission(ROLE_READER)) return;
    
    int browBookId;
    cout << "\n=== 查询借阅记录 ===" << endl;
    cout << "请输入读者ID: ";
    cin >> browBookId;
    
    char sql[512];
    sprintf(sql, "SELECT b.bookId, bk.bookName, b.borrowDate, b.returnDate FROM borrow b "
                 "JOIN books bk ON b.bookId = bk.bookId WHERE b.browBookId=%d", browBookId);
    
    if (!executeSQL(sql)) {
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }
    
    SQLINTEGER bookId;
    SQLCHAR bookName[256], borrowDate[64], returnDate[64];
    SQLLEN indicator;
    
    SQLBindCol(hStmt, 1, SQL_C_LONG, &bookId, 0, &indicator);
    SQLBindCol(hStmt, 2, SQL_C_CHAR, bookName, sizeof(bookName), &indicator);
    SQLBindCol(hStmt, 3, SQL_C_CHAR, borrowDate, sizeof(borrowDate), &indicator);
    SQLBindCol(hStmt, 4, SQL_C_CHAR, returnDate, sizeof(returnDate), &indicator);
    
    cout << "\n借阅记录：" << endl;
    cout << "------------------------------------------------------------" << endl;
    
    SQLRETURN ret;
    while ((ret = SQLFetch(hStmt)) == SQL_SUCCESS) {
        cout << "图书ID: " << bookId << " | 书名: " << bookName 
             << " | 借书日期: " << borrowDate;
        if (indicator == SQL_NULL_DATA) {
            cout << " | 状态: 未归还" << endl;
        } else {
            cout << " | 还书日期: " << returnDate << endl;
        }
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

// ============================================
// 主菜单
// ============================================

void showMenu() {
    cout << "\n========================================" << endl;
    cout << "    图书管理系统 (RBAC)" << endl;
    cout << "========================================" << endl;
    cout << "当前用户: " << currentUser.username << " | 角色: ";
    
    switch (currentUser.role) {
        case ROLE_ADMIN:
            cout << "管理员" << endl;
            cout << "1. 添加图书" << endl;
            cout << "2. 删除图书" << endl;
            cout << "3. 修改图书信息" << endl;
            break;
        case ROLE_OPERATOR:
            cout << "操作员" << endl;
            cout << "1. 借书登记" << endl;
            cout << "2. 还书登记" << endl;
            break;
        case ROLE_READER:
            cout << "读者" << endl;
            cout << "1. 查询图书" << endl;
            cout << "2. 查看借阅记录" << endl;
            break;
        default:
            cout << "未知" << endl;
            break;
    }
    
    cout << "0. 退出系统" << endl;
    cout << "========================================" << endl;
    cout << "请选择操作: ";
}

// ============================================
// 主程序
// ============================================

int main() {
    // 初始化ODBC
    if (!initODBC()) {
        cout << "ODBC初始化失败！" << endl;
        return 1;
    }
    
    // 连接数据库
    if (!connectDatabase()) {
        cleanupODBC();
        return 1;
    }
    
    // 用户登录
    string username, password;
    cout << "========================================" << endl;
    cout << "      图书管理系统登录" << endl;
    cout << "========================================" << endl;
    cout << "用户名: ";
    cin >> username;
    cout << "密码: ";
    cin >> password;
    
    if (!login(username, password)) {
        cleanupODBC();
        return 1;
    }
    
    // 主循环
    int choice;
    bool running = true;
    
    while (running) {
        showMenu();
        cin >> choice;
        
        if (choice == 0) {
            running = false;
            cout << "感谢使用图书管理系统！再见！" << endl;
            continue;
        }
        
        switch (currentUser.role) {
            case ROLE_ADMIN:
                switch (choice) {
                    case 1: addBook(); break;
                    case 2: deleteBook(); break;
                    case 3: updateBook(); break;
                    default: cout << "无效的选择！" << endl; break;
                }
                break;
            case ROLE_OPERATOR:
                switch (choice) {
                    case 1: borrowBook(); break;
                    case 2: returnBook(); break;
                    default: cout << "无效的选择！" << endl; break;
                }
                break;
            case ROLE_READER:
                switch (choice) {
                    case 1: queryBooks(); break;
                    case 2: queryBorrowRecords(); break;
                    default: cout << "无效的选择！" << endl; break;
                }
                break;
            default:
                cout << "未知角色！" << endl;
                running = false;
                break;
        }
    }
    
    // 清理资源
    cleanupODBC();
    return 0;
}
