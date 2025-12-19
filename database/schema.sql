-- ============================================
-- Library Management System Database Schema
-- 符合第三范式的图书管理系统数据库设计
-- ============================================

-- 删除已存在的表（按依赖关系逆序）
DROP TABLE IF EXISTS borrow;
DROP TABLE IF EXISTS reader;
DROP TABLE IF EXISTS books;
DROP TABLE IF EXISTS bookrack;
DROP TABLE IF EXISTS users;

-- ============================================
-- 用户表 - 用于 RBAC (Role-Based Access Control)
-- ============================================
CREATE TABLE users (
    userId INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password VARCHAR(255) NOT NULL,
    role ENUM('admin', 'operator', 'reader') NOT NULL,
    createdAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    COMMENT '用户表，存储系统用户信息及其角色'
);

-- ============================================
-- 书架表
-- ============================================
CREATE TABLE bookrack (
    bookrackId INT PRIMARY KEY,
    roomId INT NOT NULL,
    COMMENT '书架表，存储书架和房间的对应关系'
);

-- ============================================
-- 图书表
-- ============================================
CREATE TABLE books (
    bookId INT AUTO_INCREMENT PRIMARY KEY,
    bookName VARCHAR(255) NOT NULL,
    publicationDate DATETIME,
    publisher VARCHAR(255),
    bookrackId INT,
    roomId INT,
    FOREIGN KEY (bookrackId) REFERENCES bookrack(bookrackId) ON DELETE SET NULL,
    COMMENT '图书表，存储图书基本信息'
);

-- ============================================
-- 读者表
-- ============================================
CREATE TABLE reader (
    borrowBookId INT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    age INT,
    sex VARCHAR(10),
    address VARCHAR(255),
    COMMENT '读者表，存储借书人的基本信息'
);

-- ============================================
-- 借阅表
-- ============================================
CREATE TABLE borrow (
    browBookId INT NOT NULL,
    bookId INT NOT NULL,
    borrowDate DATETIME NOT NULL,
    returnDate DATETIME,
    PRIMARY KEY (browBookId, bookId, borrowDate),
    FOREIGN KEY (browBookId) REFERENCES reader(borrowBookId) ON DELETE CASCADE,
    FOREIGN KEY (bookId) REFERENCES books(bookId) ON DELETE CASCADE,
    COMMENT '借阅表，存储图书借阅记录'
);

-- ============================================
-- 插入初始数据
-- ============================================

-- 插入默认用户（密码为明文，实际应用应使用加密）
INSERT INTO users (username, password, role) VALUES
('admin', 'admin123', 'admin'),
('operator', 'oper123', 'operator'),
('reader1', 'read123', 'reader');

-- 插入示例书架数据
INSERT INTO bookrack (bookrackId, roomId) VALUES
(1, 101),
(2, 101),
(3, 102),
(4, 102),
(5, 103);

-- 插入示例图书数据
INSERT INTO books (bookName, publicationDate, publisher, bookrackId, roomId) VALUES
('C++ Primer', '2020-01-15 00:00:00', 'Addison-Wesley', 1, 101),
('Database System Concepts', '2019-06-20 00:00:00', 'McGraw-Hill', 1, 101),
('Operating System Concepts', '2018-12-10 00:00:00', 'Wiley', 2, 101),
('Computer Networks', '2021-03-25 00:00:00', 'Pearson', 3, 102),
('Data Structures and Algorithms', '2020-08-30 00:00:00', 'MIT Press', 4, 102);

-- 插入示例读者数据
INSERT INTO reader (borrowBookId, name, age, sex, address) VALUES
(1001, '张三', 25, '男', '北京市朝阳区'),
(1002, '李四', 23, '女', '上海市浦东新区'),
(1003, '王五', 27, '男', '广州市天河区');

-- 插入示例借阅记录
INSERT INTO borrow (browBookId, bookId, borrowDate, returnDate) VALUES
(1001, 1, '2024-01-10 10:00:00', '2024-02-10 10:00:00'),
(1001, 2, '2024-02-15 14:30:00', NULL),
(1002, 3, '2024-01-20 09:00:00', '2024-02-20 09:00:00'),
(1003, 4, '2024-02-01 11:00:00', NULL);

-- ============================================
-- 创建索引以提高查询性能
-- ============================================
CREATE INDEX idx_books_name ON books(bookName);
CREATE INDEX idx_borrow_bookid ON borrow(bookId);
CREATE INDEX idx_borrow_returndate ON borrow(returnDate);
CREATE INDEX idx_users_username ON users(username);

-- 查询验证
SELECT 'Schema created successfully!' AS Status;
