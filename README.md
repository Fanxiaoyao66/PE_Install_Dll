# PE_Install_Dll

解释一下文件中的一些关键函数：

**以下三个为一组，达到了找到调用此函数窗口句柄的目的**

- GetCurrentProcessId 返回调用此函数的进程的PID

- EnumWindows 枚举窗口，将每个窗口的句柄传递给回调函数EnumWindowsProc()，并且把需要对比的dwPID也传递给回调函数。

- GetWindowThreadProcessId 通过某一窗口句柄，检索创建窗口的线程/进程标识符

**以下三个为一组，达到下载网页的目的**

- InternetOpen 设置User Agent，Proxy等，初始化操作

- InternetOpenUrl 使用InternetOpen()返回当前Internet会话的句柄打开资源

	- INTERNET_FLAG_RELOAD 参数表示强制从源服务器下载请求的文件、对象或目录列表

- InternetReadFile 从InternetOpenUrl()打开的句柄中读取数据
