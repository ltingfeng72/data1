# 图书管理系统 - 基于角色的访问控制 (RBAC)

这是一个使用 C++ 和 ODBC 实现的图书管理系统，支持基于角色的访问控制（Role-Based Access Control）。

## 项目结构

```
data1/
├── database/
│   └── schema.sql          # 数据库模式文件
├── src/
│   └── main.cpp            # C++ 主程序
└── README.md               # 本文件
```

## 功能特性

### 三种用户角色

1. **管理员 (Admin)**
   - 添加图书
   - 删除图书
   - 修改图书信息

2. **操作员 (Operator)**
   - 借书登记
   - 还书登记

3. **读者 (Reader)**
   - 查询图书信息
   - 查看个人借阅记录

## 数据库设计

### 数据库表结构

系统包含以下五个表，符合第三范式：

1. **users** - 用户表（用于 RBAC）
   - `userId` (INT, 主键, 自增)
   - `username` (VARCHAR, 唯一)
   - `password` (VARCHAR)
   - `role` (ENUM: 'admin', 'operator', 'reader')
   - `createdAt` (DATETIME)

2. **bookrack** - 书架表
   - `bookrackId` (INT, 主键)
   - `roomId` (INT)

3. **books** - 图书表
   - `bookId` (INT, 主键, 自增)
   - `bookName` (VARCHAR)
   - `publicationDate` (DATETIME)
   - `publisher` (VARCHAR)
   - `bookrackId` (INT, 外键)
   - `roomId` (INT)

4. **reader** - 读者表
   - `borrowBookId` (INT, 主键)
   - `name` (VARCHAR)
   - `age` (INT)
   - `sex` (VARCHAR)
   - `address` (VARCHAR)

5. **borrow** - 借阅表
   - `browBookId` (INT, 外键)
   - `bookId` (INT, 外键)
   - `borrowDate` (DATETIME)
   - `returnDate` (DATETIME)
   - 主键：(browBookId, bookId, borrowDate)

### 数据库创建步骤

#### 1. 安装 MySQL（如果尚未安装）

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install mysql-server
```

**CentOS/RHEL:**
```bash
sudo yum install mysql-server
```

**Windows:**
下载并安装 [MySQL Community Server](https://dev.mysql.com/downloads/mysql/)

#### 2. 创建数据库

```bash
# 登录 MySQL
mysql -u root -p

# 创建数据库
CREATE DATABASE library CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

# 使用数据库
USE library;

# 执行 schema.sql 文件
SOURCE /path/to/data1/database/schema.sql;

# 或者在命令行直接执行
mysql -u root -p library < database/schema.sql
```

#### 3. 验证数据库创建

```sql
USE library;
SHOW TABLES;
SELECT * FROM users;
```

## ODBC 配置

### Linux (unixODBC)

#### 1. 安装 unixODBC 和 MySQL ODBC 驱动

```bash
# Ubuntu/Debian
sudo apt-get install unixodbc unixodbc-dev
sudo apt-get install libmyodbc

# CentOS/RHEL
sudo yum install unixODBC unixODBC-devel
sudo yum install mysql-connector-odbc
```

#### 2. 配置 DSN

编辑 `/etc/odbc.ini` 或 `~/.odbc.ini`：

```ini
[LibraryDB]
Description = Library Management System Database
Driver = MySQL
Server = localhost
Database = library
Port = 3306
User = root
Password = your_password
```

编辑 `/etc/odbcinst.ini` 或 `~/.odbcinst.ini`：

```ini
[MySQL]
Description = MySQL ODBC Driver
Driver = /usr/lib/x86_64-linux-gnu/odbc/libmyodbc8w.so
Setup = /usr/lib/x86_64-linux-gnu/odbc/libodbcmyS.so
FileUsage = 1
```

#### 3. 测试 ODBC 连接

```bash
# 列出可用的数据源
odbcinst -q -s

# 测试连接
isql LibraryDB root your_password
```

### Windows

#### 1. 安装 MySQL ODBC 驱动

下载并安装 [MySQL Connector/ODBC](https://dev.mysql.com/downloads/connector/odbc/)

#### 2. 配置 DSN

1. 打开"ODBC 数据源管理器"（在控制面板或搜索"ODBC"）
2. 选择"系统 DSN"或"用户 DSN"选项卡
3. 点击"添加"
4. 选择"MySQL ODBC 8.0 Unicode Driver"
5. 配置如下：
   - **Data Source Name**: LibraryDB
   - **TCP/IP Server**: localhost
   - **Port**: 3306
   - **User**: root
   - **Password**: your_password
   - **Database**: library
6. 点击"Test"测试连接
7. 点击"OK"保存

## 编译和运行

### Linux

```bash
# 编译
cd src
g++ -o library_system main.cpp -lodbc

# 运行
./library_system
```

### Windows

使用 Visual Studio：

```cmd
# 使用 cl 编译器
cl main.cpp odbc32.lib

# 运行
main.exe
```

或者使用 MinGW：

```cmd
g++ -o library_system.exe main.cpp -lodbc32

# 运行
library_system.exe
```

## 使用说明

### 1. 启动程序

运行编译后的程序，系统会提示输入用户名和密码。

### 2. 默认账户

数据库初始化后包含以下测试账户：

| 用户名    | 密码     | 角色      |
|----------|---------|-----------|
| admin    | admin123| 管理员    |
| operator | oper123 | 操作员    |
| reader1  | read123 | 读者      |

### 3. 功能使用

登录后，根据您的角色，系统会显示相应的菜单选项。按照菜单提示进行操作。

#### 管理员示例操作
```
1. 选择"1. 添加图书"
2. 输入图书信息（书名、出版日期、出版社、书架ID、房间ID）
3. 系统确认添加成功
```

#### 操作员示例操作
```
1. 选择"1. 借书登记"
2. 输入读者ID和图书ID
3. 系统记录借书信息
```

#### 读者示例操作
```
1. 选择"1. 查询图书"
2. 输入图书名称关键字（或留空查询所有）
3. 系统显示图书列表
```

## 安全注意事项

### 生产环境建议

1. **密码加密**
   - 当前版本使用明文存储密码，仅供学习使用
   - 生产环境应使用 bcrypt、SHA-256 等算法加密密码

2. **SQL 注入防护**
   - 当前版本使用字符串拼接，存在 SQL 注入风险
   - 应改用参数化查询（SQLBindParameter）

3. **连接安全**
   - 使用 SSL/TLS 加密数据库连接
   - 不要在代码中硬编码数据库凭据

4. **权限管理**
   - 为不同角色创建不同的数据库用户
   - 使用最小权限原则

示例改进（使用参数化查询）：
```cpp
// 使用 SQLPrepare 和 SQLBindParameter 代替 SQLExecDirect
SQLPrepare(hStmt, (SQLCHAR*)"SELECT * FROM users WHERE username=? AND password=?", SQL_NTS);
SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, username, 0, NULL);
SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, password, 0, NULL);
SQLExecute(hStmt);
```

## 技术栈

- **编程语言**: C++
- **数据库**: MySQL
- **数据库接口**: ODBC (Open Database Connectivity)
- **开发标准**: 第三范式数据库设计

## 故障排除

### 常见问题

1. **编译错误：找不到 sql.h 或 sqlext.h**
   - 确保已安装 unixODBC-dev（Linux）或配置了正确的包含路径（Windows）

2. **运行时错误：无法连接数据库**
   - 检查 MySQL 服务是否运行
   - 验证 ODBC DSN 配置是否正确
   - 确认数据库用户名和密码

3. **ODBC 驱动未找到**
   - 重新安装 MySQL ODBC 驱动
   - 检查 odbcinst.ini 配置

4. **权限不足**
   - 确保数据库用户具有足够的权限
   - 检查 MySQL 用户权限：`SHOW GRANTS FOR 'root'@'localhost';`

### 调试建议

1. 启用 ODBC 跟踪（Linux）：
```bash
export ODBCSYSINI=/etc
export ODBCINSTINI=/etc/odbcinst.ini
```

2. 查看 MySQL 日志：
```bash
sudo tail -f /var/log/mysql/error.log
```

## 扩展建议

1. 添加密码加密功能
2. 实现参数化查询防止 SQL 注入
3. 添加日志记录功能
4. 实现图书搜索的高级过滤（按出版社、日期范围等）
5. 添加借书逾期提醒功能
6. 实现数据导出功能（CSV、Excel）
7. 添加图形用户界面（使用 Qt 或其他 GUI 库）

## 许可证

本项目仅供学习和研究使用。

## 联系方式

如有问题或建议，请联系项目维护者。
