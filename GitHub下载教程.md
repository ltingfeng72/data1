# 从 GitHub PR 页面下载所有项目文件 - 完整图文教程

## 方法一：下载整个分支（推荐 - 最简单）

### 步骤 1: 访问仓库主页
1. 打开浏览器，访问：`https://github.com/ltingfeng72/data1`

### 步骤 2: 切换到 PR 分支
1. 点击页面上方的 **"Branch: main"** 下拉菜单
2. 在搜索框中输入或选择：`copilot/implement-rbac-library-system`
3. 点击该分支名称

### 步骤 3: 下载整个分支
1. 点击绿色的 **"Code"** 按钮
2. 在下拉菜单中，选择 **"Download ZIP"**
3. 文件会下载为 `data1-copilot-implement-rbac-library-system.zip`
4. 解压此 ZIP 文件即可获得所有项目文件

```
下载后的文件结构：
data1-copilot-implement-rbac-library-system/
├── database/
│   └── schema.sql
├── src/
│   └── main.cpp
├── README.md
├── 下载说明.md
├── 文件清单.md
├── 部署指南.md
├── .gitignore
└── 大作业：安全数据库开发项目(1).docx
```

---

## 方法二：从 PR 页面查看和下载文件

### 步骤 1: 访问 PR 页面
1. 访问：`https://github.com/ltingfeng72/data1/pull/[PR编号]`
   - 或从仓库主页点击 "Pull requests" 标签
   - 找到标题为 "Implement RBAC library management system" 的 PR

### 步骤 2: 查看已修改的文件
1. 在 PR 页面，点击 **"Files changed"** 标签
2. 这里会显示所有在此 PR 中添加或修改的文件

### 步骤 3: 下载单个文件
对于每个文件：
1. 点击文件名右侧的 **"..."** 按钮
2. 选择 **"View file"**
3. 在打开的页面中，点击 **"Raw"** 按钮
4. 右键选择 **"另存为..."** 保存文件

**重要提示**：
- `.sql` 文件保存时要保留 `.sql` 扩展名
- `.cpp` 文件保存时要保留 `.cpp` 扩展名
- `.md` 文件保存时要保留 `.md` 扩展名

### 步骤 4: 创建相同的文件夹结构
手动创建以下文件夹并将文件放入对应位置：
```
项目文件夹/
├── database/
│   └── schema.sql (下载并放入此文件夹)
└── src/
    └── main.cpp (下载并放入此文件夹)
```

---

## 方法三：使用 Git 命令克隆（适合熟悉 Git 的用户）

### 如果已安装 Git：

```bash
# 克隆仓库
git clone https://github.com/ltingfeng72/data1.git

# 进入项目目录
cd data1

# 切换到 PR 分支
git checkout copilot/implement-rbac-library-system

# 查看所有文件
ls -la
```

### 如果只想下载特定分支而不要完整历史：

```bash
# 仅克隆特定分支（浅克隆）
git clone --branch copilot/implement-rbac-library-system --single-branch --depth 1 https://github.com/ltingfeng72/data1.git library_system

# 进入目录
cd library_system

# 查看文件
ls -la
```

---

## 方法四：使用 GitHub CLI（gh 工具）

如果安装了 GitHub CLI：

```bash
# 克隆仓库
gh repo clone ltingfeng72/data1

# 进入目录
cd data1

# 切换到分支
git checkout copilot/implement-rbac-library-system
```

---

## 方法五：直接下载特定文件（通过浏览器）

### 访问以下直接链接下载各个文件：

**数据库文件：**
```
https://raw.githubusercontent.com/ltingfeng72/data1/copilot/implement-rbac-library-system/database/schema.sql
```

**C++ 源代码：**
```
https://raw.githubusercontent.com/ltingfeng72/data1/copilot/implement-rbac-library-system/src/main.cpp
```

**文档文件：**
```
https://raw.githubusercontent.com/ltingfeng72/data1/copilot/implement-rbac-library-system/README.md
https://raw.githubusercontent.com/ltingfeng72/data1/copilot/implement-rbac-library-system/下载说明.md
https://raw.githubusercontent.com/ltingfeng72/data1/copilot/implement-rbac-library-system/文件清单.md
https://raw.githubusercontent.com/ltingfeng72/data1/copilot/implement-rbac-library-system/部署指南.md
```

**使用方法：**
1. 复制以上链接到浏览器地址栏
2. 浏览器会显示文件内容
3. 按 `Ctrl+S` (Windows) 或 `Cmd+S` (Mac) 保存文件
4. 或右键页面选择 "另存为..."

---

## 常见问题解答

### Q1: 下载后文件名乱码或中文文件名显示不正常？
**A:** 
- Windows 用户：确保系统语言设置支持 UTF-8
- 解压时使用支持 UTF-8 的解压工具（如 7-Zip、WinRAR 最新版）
- 或重命名文件为英文名称

### Q2: 下载的 .cpp 文件用记事本打开是乱码？
**A:** 
- 使用支持 UTF-8 编码的编辑器打开
- 推荐：VS Code、Notepad++、Sublime Text
- 不要使用 Windows 自带的记事本

### Q3: 下载后如何验证文件完整性？
**A:** 检查以下文件是否都存在：
```
✓ database/schema.sql (约 4KB)
✓ src/main.cpp (约 19KB)
✓ README.md (约 7KB)
✓ 下载说明.md
✓ 文件清单.md
✓ 部署指南.md
```

### Q4: 可以只下载源代码不下载文档吗？
**A:** 可以，最少需要下载：
- `database/schema.sql` - 数据库脚本
- `src/main.cpp` - C++ 源代码
- 其他文档文件是可选的

### Q5: GitHub 无法访问怎么办？
**A:** 
- 使用 GitHub 镜像站点
- 或使用 VPN/代理
- 或请项目维护者发送文件给您

### Q6: 下载后如何使用这些文件？
**A:** 按照以下顺序：
1. 先阅读 `下载说明.md` 了解项目
2. 再阅读 `文件清单.md` 了解文件结构
3. 然后按照 `部署指南.md` 的步骤操作
4. 遇到问题查看 `README.md`

---

## 推荐下载方式总结

| 方式 | 难度 | 速度 | 推荐度 |
|------|------|------|--------|
| 方法一：下载分支 ZIP | ⭐ 最简单 | 快 | ⭐⭐⭐⭐⭐ 强烈推荐 |
| 方法二：PR 页面单个下载 | ⭐⭐ 简单 | 中 | ⭐⭐⭐ 适合查看代码 |
| 方法三：Git 克隆 | ⭐⭐⭐ 需要 Git | 快 | ⭐⭐⭐⭐ 推荐开发者 |
| 方法四：GitHub CLI | ⭐⭐⭐ 需要工具 | 快 | ⭐⭐⭐ 高级用户 |
| 方法五：直接链接 | ⭐⭐ 简单 | 慢 | ⭐⭐ 备用方案 |

---

## 快速操作指南（最简单的方式）

如果您不熟悉 GitHub，请按以下步骤操作：

### Windows 用户：
1. 打开浏览器，访问：`https://github.com/ltingfeng72/data1`
2. 点击页面左上角的 **"Branch: main"** 切换到 `copilot/implement-rbac-library-system`
3. 点击绿色的 **"Code"** 按钮
4. 点击 **"Download ZIP"**
5. 等待下载完成
6. 找到下载的 ZIP 文件（通常在"下载"文件夹）
7. 右键 ZIP 文件，选择 **"解压到当前文件夹"** 或 **"Extract Here"**
8. 打开解压后的文件夹，您会看到所有项目文件

### Mac/Linux 用户：
1. 打开终端
2. 运行以下命令：
```bash
cd ~/Downloads
git clone --branch copilot/implement-rbac-library-system https://github.com/ltingfeng72/data1.git library_system
cd library_system
ls -la
```

---

## 需要帮助？

如果在下载过程中遇到任何问题：
1. 查看本文档的"常见问题解答"部分
2. 在 GitHub PR 页面留言求助
3. 提供具体的错误信息或截图

---

**文档版本**: v1.1  
**最后更新**: 2024-12-18  
**适用分支**: copilot/implement-rbac-library-system
